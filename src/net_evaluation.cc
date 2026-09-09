/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2016 Jonas Kuratli, Jonathan Maurer, Jonathan Rosenthal
 *  Copyright (C) 2017-2026 Jonathan Rosenthal
 *
 *  Winter is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Winter is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "net_evaluation.h"
#include "general/types.h"
#include "linear_algebra.h"
#include "incbin/incbin.h"
#include "search_thread.h"
#include "net_types.h"

#include <algorithm>
#include <array>
#include <vector>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <list>

INCBIN(uint8_t, NetWeights, "rn24HD64_rew503_d24f64b_ep40.qbin");

std::array<int32_t, 2> contempt = { 0, 0 };

namespace {

// NN weights

std::vector<NetLayerType> net_input_weights(12 * 12 * (15 * 15), 0);
std::vector<NetLayerType> bias_layer_one(12 * (8 * 8), 0);

std::vector<NetLayerType> output_weights(3 * 12 * (8 * 8), 0);
std::vector<NetLayerType> m_output_weights(3 * 12 * (8 * 8), 0);
std::array<float_t, 3> output_bias;

std::vector<FullLayerType> full_layer_weights(12 * 64, 0);
FullLayerType full_layer_bias(0);

std::vector<FullLayerType> full_output_weights(3, 0);
std::vector<FullLayerType> m_full_output_weights(3, 0);

// Upper bounds of the clipped relus
NetLayerType piece_relu_bound(0);
FullLayerType full_relu_bound(0);

// Quantization scaler
float_t inverse_output_scale = 0;

// Winter net format
constexpr uint32_t kNetFormatVersion = 1;
constexpr uint32_t kNetFlagQuantized = 1;
constexpr float_t kClippedReluBound = 8;

struct NetHeader {
  uint32_t version;
  uint32_t header_bytes;
  uint32_t flags;
  uint32_t d;
  uint32_t fd;
  uint32_t num_inputs;
  uint32_t num_piece_types;
  uint32_t grid;
  uint32_t num_outputs;
  float output_scale;
  float relu_bound;
};
static_assert(sizeof(NetHeader) == 44, "Net header is padded and cannot be read as a blob");

// 16 bit quantized net weights
const int16_t *net_data = nullptr;
std::array<int32_t, block_size / 2> piece_scales;
std::array<int32_t, full_block_size / 2> full_scales;

Array2d<int32_t, 64, 64> square_offset;

void init_square_offset() {
  for (size_t src = 0; src < 64; ++src) {
    for (size_t des = 0; des < 64; ++des) {
      int32_t x = 7 + GetSquareX(src) - GetSquareX(des);
      int32_t y = 7 + GetSquareY(src) - GetSquareY(des);
      square_offset[src][des] = y * 15 + x;
    }
  }
}

void AddRelative(const NetPieceModule &p_src, const NetPieceModule &p_des, NetLayerType &features) {
  size_t idx = (p_src.pt * 12 + p_des.pt) * (15 * 15) + square_offset[p_src.sq][p_des.sq];
  features += net_input_weights[idx];
}

void RemoveRelative(const std::tuple<Piece, Square> &p_src, const NetPieceModule &p_des, NetLayerType &features) {
  const auto [src_pt, src_sq] = p_src;
  size_t idx = (src_pt * 12 + p_des.pt) * (15 * 15) + square_offset[src_sq][p_des.sq];
  features -= net_input_weights[idx];
}

void EvalPieceRelations(PieceList &piece_modules) {
  for (size_t i = 0; i < piece_modules.size(); ++i) {
    NetLayerType features = piece_modules[i].features;
    for (size_t j = 0; j < i; ++j) {
      AddRelative(piece_modules[j], piece_modules[i], features);
    }
    for (size_t j = i+1; j < piece_modules.size(); ++j) {
      AddRelative(piece_modules[j], piece_modules[i], features);
    }
    piece_modules[i].features = features;
  }
}

template<Color color>
inline void AddPieceType(const Board &board, const PieceType pt,
                         PieceList &piece_modules,
                         FullLayerType &full_layer) {
  constexpr int32_t c_offset = color == kWhite ? 0 : 6;

  for (BitBoard pieces = board.get_piece_bitboard(color, pt); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    size_t bias_idx = (pt + c_offset) * 8 * 8 + piece_square;
    NetPieceModule npm = {bias_layer_one[bias_idx], (pt + c_offset), piece_square};
    piece_modules.push_back(npm);

    full_layer += full_layer_weights[(pt + c_offset) * 64 + piece_square];
  }
}

inline void AddAllPieceTypes(const Board &board,
                         PieceList &piece_modules,
                         FullLayerType &full_layer) {
  for (PieceType piece_type = kPawn; piece_type <= kKing; ++piece_type) {
      AddPieceType<kWhite>(board, piece_type, piece_modules, full_layer);
      AddPieceType<kBlack>(board, piece_type, piece_modules, full_layer);
  }
}

template<Color color>
inline void AddPieceType(const Board &board, const PieceType pt,
                         PieceList &piece_modules,
                         FullLayerType &full_layer, const BitBoard mask) {
  constexpr int32_t c_offset = color == kWhite ? 0 : 6;

  for (BitBoard pieces = board.get_piece_bitboard(color, pt) & mask; pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    size_t bias_idx = (pt + c_offset) * 8 * 8 + piece_square;
    NetPieceModule npm = {bias_layer_one[bias_idx], (pt + c_offset), piece_square};
    piece_modules.push_back(npm);

    full_layer += full_layer_weights[(pt + c_offset) * 64 + piece_square];
  }
}

inline void AddAllPieceTypes(const Board &board,
                         PieceList &piece_modules,
                         FullLayerType &full_layer, const BitBoard mask) {
  for (PieceType piece_type = kPawn; piece_type <= kKing; ++piece_type) {
      AddPieceType<kWhite>(board, piece_type, piece_modules, full_layer, mask);
      AddPieceType<kBlack>(board, piece_type, piece_modules, full_layer, mask);
  }
}

}

namespace net_evaluation {

template<Color color>
Score PerspectiveNetForward(const PieceList &piece_modules,
                 const FullLayerType &_full_layer) {
  std::array<NetOutputType, 3> output_helpers{0, 0, 0};
  for (size_t piece_idx = 0; piece_idx < piece_modules.size(); piece_idx++) {
    auto features = piece_modules[piece_idx].features.clipped_relu(piece_relu_bound);
    size_t idx = 3 * (piece_modules[piece_idx].pt * 8 * 8 + piece_modules[piece_idx].sq);
    assert(idx + 2 < output_weights.size());
    for (size_t output_idx = 0; output_idx < 3; output_idx++) {
      if constexpr (color == kWhite) {
        output_helpers[output_idx].madd(output_weights[idx + output_idx], features);
      }
      else {
        output_helpers[output_idx].madd(m_output_weights[idx + output_idx], features);
      }
    }
  }
  
  auto full_layer = _full_layer.clipped_relu(full_relu_bound);
  
  float_t sum = 0;
  std::array<float_t, 3> outcomes;
  for (size_t i = 0; i < 3; ++i) {
    int32_t logit;
    if constexpr (color == kWhite) {
      logit = full_layer.dot(full_output_weights[i]);
    }
    else {
      logit = full_layer.dot(m_full_output_weights[i]);
    }
    logit += output_helpers[i].sum();
    outcomes[i] = logit * inverse_output_scale + output_bias[i];
  }
  
  float_t max_logit = *std::max_element(outcomes.begin(), outcomes.end());
  
  for (size_t i = 0; i < 3; ++i) {
    outcomes[i] = std::exp(outcomes[i] - max_logit);
    sum += outcomes[i];
  }
  
  float_t win = outcomes[0] / sum;
  float_t loss = outcomes[2] / sum;
  return WDLScore::from_pct(win, loss);
}

Score NetForward(const PieceList &piece_modules,
                 const FullLayerType &_full_layer, const Color color) {
    if (color == kWhite)
      return PerspectiveNetForward<kWhite>(piece_modules, _full_layer);
    return PerspectiveNetForward<kBlack>(piece_modules, _full_layer);
}

Score ScoreBoard(const Board &board) {
  PieceList piece_modules;
  FullLayerType full_layer = full_layer_bias;
  AddAllPieceTypes(board, piece_modules, full_layer);
  EvalPieceRelations(piece_modules);
  if (contempt[board.get_turn()] != 0) {
    return AddContempt(NetForward(piece_modules, full_layer, board.get_turn()),
                       board.get_turn());
  }
  return NetForward(piece_modules, full_layer, board.get_turn());
}

//inline std::tuple<NetPieceModule, NetPieceModule> get_partial()
//                                          Thread &t, const Depth depth,
//                                          const OptEntry &entry) {
//  return {rAlpha, rBeta, rDepth};
//}

Score FromScratch(search::Thread &t) {
  const Depth h = t.get_height();
  PartialEvaluation &eval = t.evaluations[h];
  eval.pieces.clear();
  eval.global_features = full_layer_bias;
  AddAllPieceTypes(t.board, eval.pieces, eval.global_features);
  EvalPieceRelations(eval.pieces);
  if (contempt[t.board.get_turn()] != 0) {
    return AddContempt(NetForward(eval.pieces, eval.global_features, t.board.get_turn()),
                       t.board.get_turn());
  }
  return NetForward(eval.pieces, eval.global_features, t.board.get_turn());
}

Score ScoreThread(search::Thread &t) {
  if (bitops::PopCount(t.board.get_all_pieces()) < 6) {
    return ScoreBoard(t.board);
  }
  const Depth h = t.get_height();
  if (h==0) {
    return FromScratch(t);
  }

  // Pieces from previous iteration which have not moved
  PieceList pieces;
  // Pieces which are no longer on the same squares
  std::vector<std::tuple<Piece, Square>> no_longer;
  no_longer.reserve(4);
  BitBoard mask = 0;
  for (const NetPieceModule &piece : t.evaluations[h-1].pieces) {
    Piece b_piece = t.board.get_piece(piece.sq);
    if (b_piece == kNoPiece) {
      no_longer.emplace_back(piece.pt, piece.sq);
      continue;
    }
    if (GetPieceColor(b_piece) == kBlack) {
      b_piece = GetPieceType(b_piece) + 6;
    }
    if (b_piece == piece.pt) {
      pieces.emplace_back(piece);
      mask |= GetSquareBitBoard(piece.sq);
    }
    else {
      no_longer.emplace_back(piece.pt, piece.sq);
    }
  }
  
  // Only continue if we expect to save any work.
  if (pieces.size() <= no_longer.size()) {
    return FromScratch(t);
  }
  
  // Get partially computed full layer.
  FullLayerType full_layer = t.evaluations[h-1].global_features;
  for (const std::tuple<Piece, Square> &removed_piece : no_longer) {
    const auto [piece, square] = removed_piece;
    full_layer -= full_layer_weights[piece * 64 + square];
  }
  
  // Add missing pieces
  const size_t kept = pieces.size();
  mask = ~mask;
  AddAllPieceTypes(t.board, pieces, full_layer, mask);
  
  // Finish partial evaluation
  for (size_t i = 0; i < kept; ++i) {
    NetLayerType features = pieces[i].features;
    // Remove influence from moved pieces
    for (const std::tuple<Piece, Square> &removed_piece : no_longer) {
      RemoveRelative(removed_piece, pieces[i], features);
    }
    // Add influence from new pieces
    for (size_t j = kept; j < pieces.size(); ++j) {
      AddRelative(pieces[j], pieces[i], features);
    }
    pieces[i].features = features;
  }
  for (size_t i = kept; i < pieces.size(); ++i) {
    NetLayerType features = pieces[i].features;
    for (size_t j = 0; j < i; ++j) {
      AddRelative(pieces[j], pieces[i], features);
    }
    for (size_t j = i+1; j < pieces.size(); ++j) {
      AddRelative(pieces[j], pieces[i], features);
    }
    pieces[i].features = features;
  }
  
  // Store partial evaluation
  t.evaluations[h].pieces = pieces;
  t.evaluations[h].global_features = full_layer;
  // Return full evaluation
  if (contempt[t.board.get_turn()] != 0) {
    return AddContempt(NetForward(pieces, full_layer, t.board.get_turn()),
                       t.board.get_turn());
  }
  return NetForward(pieces, full_layer, t.board.get_turn());
}

using IP = std::pair<size_t, size_t>;

size_t wrapped_idx(const std::list<IP> &values) {
  size_t idx = 0;
  for (const IP &ip : values) {
    idx = idx * ip.second + ip.first;
  }
  return idx;
}

// Expected network size
constexpr size_t expected_weight_count() {
  constexpr size_t d = block_size / 2;
  constexpr size_t fd = full_block_size / 2;
  return 12 * 12 * (15 * 15) * d    // relation weights
       + 12 * (8 * 8) * d           // relation bias
       + 3 * 12 * (8 * 8) * (2 * d) // piece output head (full width)
       + 12 * (8 * 8) * fd + fd     // global accumulator and its bias
       + 3 * (2 * fd);              // global output head (full width)
}


[[noreturn]] void net_load_error(const std::string &msg) {
  std::cerr << "Could not load network: " << msg << std::endl;
  std::exit(EXIT_FAILURE);
}

// Sanity checks to ensure the loaded network is valid
NetHeader read_header() {
  if (gNetWeightsSize < 4 + sizeof(NetHeader)) {
    net_load_error("file is smaller than its header");
  }
  if (std::memcmp(gNetWeightsData, "WNET", 4) != 0) {
    net_load_error("bad magic, this is not a quantized net");
  }
  
  NetHeader header;
  std::memcpy(&header, gNetWeightsData + 4, sizeof(header));

  if (header.version != kNetFormatVersion) {
    net_load_error("format version " + std::to_string(header.version) + ", expected "
              + std::to_string(kNetFormatVersion));
  }
  if ((header.flags & kNetFlagQuantized) == 0) {
    net_load_error("weights are not quantized");
  }
  const auto size_mismatch = [](const std::string &name, uint32_t got, size_t want) {
    if (got != want) {
      net_load_error(std::string(name) + " is " + std::to_string(got) + ", this build expects "
                + std::to_string(want));
    }
  };
  size_mismatch("block width d", header.d, block_size / 2);
  size_mismatch("full block width fd", header.fd, full_block_size / 2);
  size_mismatch("input count", header.num_inputs, 12 * 64);
  size_mismatch("piece type count", header.num_piece_types, 12);
  size_mismatch("relation grid size", header.grid, 15);
  size_mismatch("output count", header.num_outputs, 3);
  
  if (header.relu_bound != kClippedReluBound) {
    net_load_error("clipped relu bound is " + std::to_string(header.relu_bound) + ", this build expects "
              + std::to_string(kClippedReluBound));
  }
  const size_t expected = header.header_bytes + 2 * expected_weight_count();
  if (gNetWeightsSize != expected) {
    net_load_error("file is " + std::to_string(gNetWeightsSize) + " bytes, expected "
              + std::to_string(expected));
  }

  size_t offset = 4 + sizeof(NetHeader);
  std::memcpy(piece_scales.data(), gNetWeightsData + offset, sizeof(piece_scales));
  offset += sizeof(piece_scales);
  std::memcpy(full_scales.data(), gNetWeightsData + offset, sizeof(full_scales));
  offset += sizeof(full_scales);
  std::memcpy(output_bias.data(), gNetWeightsData + offset, sizeof(output_bias));

  net_data = reinterpret_cast<const int16_t*>(gNetWeightsData + header.header_bytes);
  return header;
}

void init_conv_weights(size_t &offset) {
  constexpr size_t d = block_size / 2;
  for (size_t piece_in = 0; piece_in < 12; ++piece_in) {
    size_t m_piece_in = (piece_in + 6) % 12;
    for (size_t piece_out = 0; piece_out < 12; ++piece_out) {
      size_t m_piece_out = (piece_out + 6) % 12;
      for (size_t h = 0; h < 15; ++h) {
        size_t m_h = 15 - h - 1;
        for (size_t w = 0; w < 15; ++w) {
          size_t idx = wrapped_idx({IP(piece_in,12), IP(piece_out,12), IP(h,15), IP(w,15)});
          for (size_t i = 0; i < d; ++i) {
            size_t idx2 = wrapped_idx({IP(piece_out,12), IP(i,d), IP(piece_in,12),
                                       IP(h,15), IP(w,15)});
            size_t m_idx = wrapped_idx({IP(m_piece_out,12), IP(i,d), IP(m_piece_in,12),
                                        IP(m_h,15), IP(w,15)});
            net_input_weights[idx][i] = net_data[idx2 + offset];
            net_input_weights[idx][i + d] = net_data[m_idx + offset];
          }
        }
      }
    }
  }
  offset += 12 * 12 * (15 * 15) * d;
}

void init_conv_bias_weights(size_t &offset) {
  constexpr size_t d = block_size / 2;
  for (size_t pt = 0; pt < 12; ++pt) {
    size_t m_pt = (pt + 6) % 12;
    for (size_t h = 0; h < 8; ++h) {
      size_t m_h = 8 - h - 1;
      for (size_t w = 0; w < 8; ++w) {
        size_t idx = wrapped_idx({IP(pt,12), IP(h,8), IP(w,8)});
        for (size_t i = 0; i < d; ++i) {
          size_t idx2 = wrapped_idx({IP(pt,12), IP(i,d), IP(h,8), IP(w,8)});
          size_t m_idx = wrapped_idx({IP(m_pt,12), IP(i,d), IP(m_h,8), IP(w,8)});
          bias_layer_one[idx][i] = net_data[idx2 + offset];
          bias_layer_one[idx][i + d] = net_data[m_idx + offset];
        }
      }
    }
  }
  offset += 12 * (8 * 8) * d;
}

void init_out_weights(size_t &offset) {
  constexpr size_t d = block_size / 2;
  for (size_t pt = 0; pt < 12; ++pt) {
    size_t m_pt = (pt + 6) % 12;
    for (size_t h = 0; h < 8; ++h) {
      size_t m_h = 8 - h - 1;
      for (size_t w = 0; w < 8; ++w) {
        for (size_t res = 0; res < 3; ++res) {
          size_t idx = wrapped_idx({IP(pt,12), IP(h,8), IP(w,8), IP(res,3)});
          for (size_t i = 0; i < d; ++i) {
            size_t idx2 = wrapped_idx({IP(res,3), IP(0,2), IP(pt,12), IP(i,d), IP(h,8), IP(w,8)});
            size_t m_idx = wrapped_idx({IP(res,3), IP(1,2), IP(m_pt,12), IP(i,d), IP(m_h,8), IP(w,8)});
            output_weights[idx][i] = net_data[idx2 + offset];
            output_weights[idx][i + d] = net_data[m_idx + offset];
          }
        }
      }
    }
  }
  offset += 3 * 12 * (8 * 8) * (2 * d);
}

void init_full_layer_weights(size_t &offset) {
  constexpr size_t fd = full_block_size / 2;
  for (size_t pt = 0; pt < 12; ++pt) {
    size_t m_pt = (pt + 6) % 12;
    for (size_t sq = 0; sq < 64; ++sq) {
      size_t m_sq = GetMirroredSquare(sq);
      size_t idx = wrapped_idx({IP(pt,12), IP(sq,64)});
      for (size_t i = 0; i < fd; ++i) {
        size_t idx2 = wrapped_idx({IP(i,fd), IP(pt,12), IP(sq,64)});
        size_t m_idx = wrapped_idx({IP(i,fd), IP(m_pt,12), IP(m_sq,64)});
        full_layer_weights[idx][i] = net_data[idx2 + offset];
        full_layer_weights[idx][i + fd] = net_data[m_idx + offset];
      }
    }
  }
  offset += fd * 12 * 64;
  for (size_t i = 0; i < fd; ++i) {
    full_layer_bias[i] = net_data[i + offset];
    full_layer_bias[i + fd] = net_data[i + offset];
  }
  offset += fd;
}

void init_full_output_weights(size_t &offset) {
  for (size_t res = 0; res < 3; ++res) {
    for (size_t i = 0; i < full_block_size; ++i) {
      size_t idx = wrapped_idx({IP(res,3), IP(i,full_block_size)});
      full_output_weights[res][i] = net_data[idx + offset];
    }
  }
  offset += 3 * full_block_size;
}

// Weights adjusted for positions where it is black to move.
void init_mirrored_outputs() {
  constexpr size_t d = block_size / 2, fd = full_block_size / 2;
  for (size_t pt = 0; pt < 12; ++pt) {
    size_t m_pt = (pt + 6) % 12;
    for (Square sq = 0; sq < 64; ++sq) {
      Square m_sq = GetMirroredSquare(sq);
      for (size_t res = 0; res < 3; ++res) {
        size_t idx = res + (pt * 64 + sq) * 3;
        size_t m_idx = res + (m_pt * 64 + m_sq) * 3;
        for (size_t i = 0; i < d; ++i) {
          m_output_weights[idx][i] = output_weights[m_idx][i + d];
          m_output_weights[idx][i + d] = output_weights[m_idx][i];
        }
      }
    }
  }
  for (size_t idx = 0; idx < full_output_weights.size(); ++idx) {
    for (size_t i = 0; i < fd; ++i) {
      m_full_output_weights[idx][i] = full_output_weights[idx][i + fd];
      m_full_output_weights[idx][i + fd] = full_output_weights[idx][i];
    }
  }
}

#ifndef NDEBUG
// This NDEBUG section is a sanity check for network quantization.
// It is written by Claude Opus 5.
constexpr std::array<size_t, 6> kMaxPieceCount = { 8, 10, 10, 10, 10, 1 };
constexpr size_t kMaxNonKing = 15;
constexpr size_t kMaxCandidates = 10;
constexpr int32_t kMaxInt16 = 32767;

int32_t bound_side(const std::array<std::array<int32_t, kMaxCandidates>, 6> &candidates) {
  std::array<int32_t, 5 * kMaxCandidates> pool;
  size_t n = 0;
  for (size_t t = 0; t < 5; ++t) {
    for (size_t i = 0; i < kMaxPieceCount[t] && candidates[t][i] > 0; ++i) {
      pool[n++] = candidates[t][i];
    }
  }
  const size_t take = std::min(kMaxNonKing, n);
  std::partial_sort(pool.begin(), pool.begin() + take, pool.begin() + n, std::greater<int32_t>());
  int32_t total = candidates[5][0];
  for (size_t i = 0; i < take; ++i) {
    total += pool[i];
  }
  return total;
}

// Splits values into the kMaxCandidates largest in descending order, followed
// by the kMaxCandidates smallest in ascending order.
template<size_t n>
void sort_extremes(std::array<int32_t, n> &values) {
  static_assert(n >= 2 * kMaxCandidates, "Not enough values to take extremes from");
  std::partial_sort(values.begin(), values.begin() + kMaxCandidates, values.end(),
                    std::greater<int32_t>());
  std::partial_sort(values.begin() + kMaxCandidates, values.begin() + 2 * kMaxCandidates,
                    values.end());
}

void verify_accumulator_bounds() {
  std::vector<std::array<int32_t, 225>> grid(12 * block_size);
  for (size_t pt_des = 0; pt_des < 12; ++pt_des) {
    for (size_t pt_src = 0; pt_src < 12; ++pt_src) {
      for (size_t o = 0; o < 225; ++o) {
        const NetLayerType &w = net_input_weights[(pt_src * 12 + pt_des) * 225 + o];
        for (size_t c = 0; c < block_size; ++c) {
          grid[pt_src * block_size + c][o] = w[c];
        }
      }
      for (size_t c = 0; c < block_size; ++c) {
        sort_extremes(grid[pt_src * block_size + c]);
      }
    }
    for (size_t c = 0; c < block_size; ++c) {
      int32_t hi = bias_layer_one[pt_des * 64][c], lo = hi;
      for (size_t sq = 1; sq < 64; ++sq) {
        hi = std::max<int32_t>(hi, bias_layer_one[pt_des * 64 + sq][c]);
        lo = std::min<int32_t>(lo, bias_layer_one[pt_des * 64 + sq][c]);
      }
      for (size_t side = 0; side < 2; ++side) {
        std::array<std::array<int32_t, kMaxCandidates>, 6> up, down;
        for (size_t t = 0; t < 6; ++t) {
          const std::array<int32_t, 225> &v = grid[(side * 6 + t) * block_size + c];
          for (size_t i = 0; i < kMaxCandidates; ++i) {
            up[t][i] = v[i];
            down[t][i] = -v[kMaxCandidates + i];
          }
        }
        hi += bound_side(up);
        lo -= bound_side(down);
      }
      assert(std::max(hi, -lo) <= kMaxInt16);
    }
  }
  for (size_t c = 0; c < full_block_size; ++c) {
    int32_t hi = full_layer_bias[c], lo = hi;
    for (size_t side = 0; side < 2; ++side) {
      std::array<std::array<int32_t, kMaxCandidates>, 6> up, down;
      for (size_t t = 0; t < 6; ++t) {
        std::array<int32_t, 64> v;
        for (size_t sq = 0; sq < 64; ++sq) {
          v[sq] = full_layer_weights[(side * 6 + t) * 64 + sq][c];
        }
        sort_extremes(v);
        for (size_t i = 0; i < kMaxCandidates; ++i) {
          up[t][i] = v[i];
          down[t][i] = -v[kMaxCandidates + i];
        }
      }
      hi += bound_side(up);
      lo -= bound_side(down);
    }
    assert(std::max(hi, -lo) <= kMaxInt16);
  }
}
#endif

void init_weights() {
  init_square_offset();
  const NetHeader header = read_header();
  size_t offset = 0;
  
  init_conv_weights(offset);
  init_conv_bias_weights(offset);
  
  init_out_weights(offset);
  
  init_full_layer_weights(offset);
  init_full_output_weights(offset);
  assert(offset == expected_weight_count());
  
  init_mirrored_outputs();

  for (size_t i = 0; i < block_size / 2; ++i) {
    piece_relu_bound[i] = kClippedReluBound * piece_scales[i];
    piece_relu_bound[i + block_size / 2] = piece_relu_bound[i];
  }
  
  for (size_t i = 0; i < full_block_size / 2; ++i) {
    full_relu_bound[i] = kClippedReluBound * full_scales[i];
    full_relu_bound[i + full_block_size / 2] = full_relu_bound[i];
  }
  inverse_output_scale = 1.0 / header.output_scale;

#ifndef NDEBUG
  verify_accumulator_bounds();
#endif
}

void SetContempt(Color color, int32_t value) {
  contempt[color] = value;
  contempt[other_color(color)] = -value;
  if (value == 0) {
    contempt[other_color(color)] = 0;
  }
}

std::array<Score, 2> GetDrawArray() {
  if (contempt[kWhite] == 0) {
    return std::array<Score, 2> { kDrawScore, kDrawScore };
  }
  return std::array<Score, 2> { AddContempt(kDrawScore, kWhite), AddContempt(kDrawScore, kBlack) };
}

Score AddContempt(Score score, Color color) {
  assert(score.is_static_eval());
  int32_t draw = WDLScore::scale - score.win - score.loss;
  if (contempt[color] > 0) { // Contempt is positive, draws are counted as losses
    int32_t diff = (draw * contempt[color]) / 100;
    return WDLScore { score.win, score.loss + diff };
  }
  // contempt is negative, draws are counted as wins
  int32_t diff = -(draw * contempt[color]) / 100;
  return WDLScore { score.win + diff, score.loss};
}

Score RemoveContempt(Score score, Color color) {
  if (!score.is_static_eval() || contempt[color] == 0
      || contempt[color] >= 100 || contempt[color] <= -100) {
    return score;
  }
  int32_t draw = WDLScore::scale - score.win - score.loss;
  if (contempt[color] >= 0) {
    int32_t orig_draw = (draw * 100) / (100 - contempt[color]);
    int32_t diff = orig_draw - draw;
    int32_t loss = std::max(score.loss - diff, 0);
    return WDLScore { score.win, loss };
  }
  int32_t orig_draw = (draw * 100) / (100 + contempt[color]);
  int32_t diff = orig_draw - draw;
  int32_t win = std::max(score.win - diff, 0);
  return WDLScore { win, score.loss };
}

}

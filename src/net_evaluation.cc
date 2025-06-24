#include "net_evaluation.h"
#include "general/types.h"
#include "linear_algebra.h"
#include "incbin/incbin.h"
#include "search_thread.h"
#include "net_types.h"

#include <array>
#include <vector>
#include <cmath>
#include <list>

//INCBIN(float_t, NetWeights, "rnet16H64h.bin");
//INCBIN(float_t, NetWeights, "rnet8H96e.bin");
//INCBIN(float_t, NetWeights, "rnet16H64o.bin");
INCBIN(float_t, NetWeights, "rn16HD64b.bin");

std::array<int32_t, 2> contempt = { 0, 0 };

namespace {

// NN weights

std::vector<NetLayerType> net_input_weights(12 * 12 * 15 * 15, 0);
std::vector<NetLayerType> bias_layer_one(12 * 8 * 8, 0);

std::vector<NetLayerType> output_weights(3 * 12 * 8 * 8, 0);
std::vector<NetLayerType> m_output_weights(3 * 12 * 8 * 8, 0);
std::array<float_t, 3> output_bias;

std::vector<FullLayerType> full_layer_weights(12 * 64, 0);
FullLayerType full_layer_bias(0);

std::vector<FullLayerType> full_output_weights(3, 0);
std::vector<FullLayerType> m_full_output_weights(3, 0);

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
  size_t idx = (p_src.pt * 12 + p_des.pt) * 225 + square_offset[p_src.sq][p_des.sq];
  features += net_input_weights[idx];
}

void RemoveRelative(const std::tuple<Piece, Square> &p_src, const NetPieceModule &p_des, NetLayerType &features) {
  const auto [src_pt, src_sq] = p_src;
  size_t idx = (src_pt * 12 + p_des.pt) * 225 + square_offset[src_sq][p_des.sq];
  features -= net_input_weights[idx];
}

void EvalPieceRelations(std::vector<NetPieceModule> &piece_modules) {
  // Create a temporary copy of the initial biases to read from.
  std::vector<NetLayerType> initial_features;
  initial_features.reserve(piece_modules.size());
  for (const auto& pm : piece_modules) {
    initial_features.push_back(pm.features);
  }

  for (size_t i = 0; i < piece_modules.size(); ++i) {
    // Start with the initial bias for piece i
    NetLayerType final_features = initial_features[i];
    for (size_t j = 0; j < piece_modules.size(); ++j) {
      if (i == j) continue;
      // Add influence from every other piece j onto piece i
      AddRelative(piece_modules[j], piece_modules[i], final_features);
    }
    // Write the final computed value back
    piece_modules[i].features = final_features;
  }
}

template<Color color>
inline void AddPieceType(const Board &board, const PieceType pt,
                         std::vector<NetPieceModule> &piece_modules,
                         FullLayerType &full_layer) {
  constexpr int32_t c_offset = color == kWhite ? 0 : 6;
  
  for (BitBoard pieces = board.get_piece_bitboard(color, pt); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    size_t bias_idx = (pt + c_offset) * 8 * 8 + piece_square;
    NetPieceModule npm = {(pt + c_offset), piece_square, bias_layer_one[bias_idx]};
    piece_modules.push_back(npm);
    
    full_layer += full_layer_weights[(pt + c_offset) * 64 + piece_square];
  }
}

template<Color color>
inline void AddPieceType(const Board &board, const PieceType pt,
                         std::vector<NetPieceModule> &piece_modules,
                         FullLayerType &full_layer, const BitBoard mask) {
  constexpr int32_t c_offset = color == kWhite ? 0 : 6;
  
  for (BitBoard pieces = board.get_piece_bitboard(color, pt) & mask; pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    size_t bias_idx = (pt + c_offset) * 8 * 8 + piece_square;
    NetPieceModule npm = {(pt + c_offset), piece_square, bias_layer_one[bias_idx]};
    piece_modules.push_back(npm);
    
    full_layer += full_layer_weights[(pt + c_offset) * 64 + piece_square];
  }
}

inline void AddAllPieceTypes(const Board &board,
                         std::vector<NetPieceModule> &piece_modules,
                         FullLayerType &full_layer, const BitBoard mask) {
  for (PieceType piece_type = kPawn; piece_type <= kKing; ++piece_type) {
      AddPieceType<kWhite>(board, piece_type, piece_modules, full_layer, mask);
      AddPieceType<kBlack>(board, piece_type, piece_modules, full_layer, mask);
  }
}

inline void AddAllPieceTypes(const Board &board,
                         std::vector<NetPieceModule> &piece_modules,
                         FullLayerType &full_layer) {
  AddAllPieceTypes(board, piece_modules, full_layer, ~BitBoard(0));
}

}

namespace net_evaluation {

template<Color color>
Score PerspectiveNetForward(const std::vector<NetPieceModule> &piece_modules,
                 const FullLayerType &_full_layer) {
  std::array<NetLayerType, 3> output_helpers{0, 0, 0};
  for (size_t piece_idx = 0; piece_idx < piece_modules.size(); piece_idx++) {
    auto features = piece_modules[piece_idx].features.clipped_relu(8);
    size_t idx = 3 * (piece_modules[piece_idx].pt * 8 * 8 + piece_modules[piece_idx].sq);
    assert(idx + 2 < output_weights.size());
    for (size_t output_idx = 0; output_idx < 3; output_idx++) {
      if constexpr (color == kWhite) {
        output_helpers[output_idx].FMA(output_weights[idx + output_idx], features);
      }
      else {
        output_helpers[output_idx].FMA(m_output_weights[idx + output_idx], features);
      }
    }
  }
  
  auto full_layer = _full_layer.clipped_relu(8);
  
  float_t sum = 0;
  std::array<float_t, 3> outcomes;
  for (size_t i = 0; i < 3; ++i) {
    if constexpr (color == kWhite) {
      outcomes[i] = full_layer.dot(full_output_weights[i]);
    }
    else {
      outcomes[i] = full_layer.dot(m_full_output_weights[i]);
    }
    outcomes[i] += output_helpers[i].sum() + output_bias[i];
    outcomes[i] = std::exp(outcomes[i]);
    sum += outcomes[i];
  }
  
  for (size_t i = 0; i < 3; ++i) {
    outcomes[i] /= sum;
  }
  
  float win = outcomes[0];
  float win_draw = outcomes[0] + outcomes[1];
  return WDLScore::from_pct_valid(win, win_draw);
}

Score NetForward(const std::vector<NetPieceModule> &piece_modules,
                 const FullLayerType &_full_layer, const Color color) {
    if (color == kWhite)
      return PerspectiveNetForward<kWhite>(piece_modules, _full_layer);
    return PerspectiveNetForward<kBlack>(piece_modules, _full_layer);
}

Score ScoreBoard(const Board &board) {
  std::vector<NetPieceModule> piece_modules;
  piece_modules.reserve(32);
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
  //~ std::cout << "------------------------"<< "From scratch!" << "------------------------" << std::endl;
  const Depth h = t.get_height();
  std::vector<NetPieceModule> piece_modules;
  piece_modules.reserve(32);
  FullLayerType full_layer = full_layer_bias;
  AddAllPieceTypes(t.board, piece_modules, full_layer);
  EvalPieceRelations(piece_modules);
  t.evaluations[h].pieces = piece_modules;
  t.evaluations[h].global_features = full_layer;
  if (contempt[t.board.get_turn()] != 0) {
    return AddContempt(NetForward(piece_modules, full_layer, t.board.get_turn()),
                       t.board.get_turn());
  }
  return NetForward(piece_modules, full_layer, t.board.get_turn());
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
  std::vector<NetPieceModule> pieces;
  pieces.reserve(32);
  // Pieces which are no longer on the same squares
  std::vector<std::tuple<Piece, Square>> no_longer;
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

void init_conv_weights(size_t &offset) {
  assert(offset == 0);
  for (size_t piece_in = 0; piece_in < 12; ++piece_in) {
    for (size_t piece_out = 0; piece_out < 12; ++piece_out) {
      for (size_t h = 0; h < 15; ++h) {
        for (size_t w = 0; w < 15; ++w) {
          size_t idx = wrapped_idx({IP(piece_in,12), IP(piece_out,12),
                                    IP(h,15), IP(w,15)});
          assert(idx < net_input_weights.size());
          for (size_t d = 0; d < block_size; ++d) {
            size_t idx2 = wrapped_idx({IP(piece_out,12), IP(d,block_size),
                                       IP(piece_in,12), IP(h,15), IP(w,15)});
            assert(idx2 < net_input_weights.size() * block_size);
            net_input_weights[idx][d] = gNetWeightsData[idx2];
          }
        }
      }
    }
  }
  offset += 12 * 12 * 15 * 15 * block_size;
}

void init_mirrored_conv_weights(size_t &offset) {
  assert(offset == 0);
  for (size_t piece_in = 0; piece_in < 12; ++piece_in) {
    for (size_t piece_out = 0; piece_out < 12; ++piece_out) {
      size_t m_piece_in = (piece_in + 6) % 12;
      size_t m_piece_out = (piece_out + 6) % 12;
      for (size_t h = 0; h < 15; ++h) {
        size_t m_h = 15 - h - 1;
        for (size_t w = 0; w < 15; ++w) {
          size_t idx = wrapped_idx({IP(piece_in,12), IP(piece_out,12),
                                    IP(h,15), IP(w,15)});
          assert(idx < net_input_weights.size());
          for (size_t d = 0; d < (block_size / 2); ++d) {
            size_t idx2 = wrapped_idx({IP(piece_out,12), IP(d, (block_size / 2)),
                                       IP(piece_in,12), IP(h,15), IP(w,15)});
            size_t m_idx = wrapped_idx({IP(m_piece_out,12),
                                        IP(d, (block_size / 2)),
                                        IP(m_piece_in,12),
                                        IP(m_h,15), IP(w,15)});
            net_input_weights[idx][d] = gNetWeightsData[idx2];
            net_input_weights[idx][d + (block_size/2)] = gNetWeightsData[m_idx];
          }
        }
      }
    }
  }
  offset += 12 * 12 * 15 * 15 * block_size / 2;
}

void init_conv_bias_weights(size_t &offset) {
  assert(offset > 0);
  for (size_t pt = 0; pt < 12; ++pt) {
    for (size_t h = 0; h < 8; ++h) {
      for (size_t w = 0; w < 8; ++w) {
        size_t idx = wrapped_idx({IP(pt,12), IP(h,8), IP(w,8)});
        assert(idx < bias_layer_one.size());
        size_t idx_c = wrapped_idx({IP(pt,12), IP(pt,12),
                                    IP(7,15), IP(7,15)});
        for (size_t d = 0; d < block_size; ++d) {
          size_t idx2 = wrapped_idx({IP(pt,12), IP(d, block_size), IP(h, 8), IP(w, 8)});
          assert(idx2 < bias_layer_one.size() * block_size);
          bias_layer_one[idx][d] = gNetWeightsData[idx2 + offset];
          bias_layer_one[idx][d] += net_input_weights[idx_c][d];
        }
      }
    }
  }
  offset += 12 * 8 * 8 * block_size;
}

void init_mirrored_conv_bias_weights(size_t &offset) {
  assert(offset > 0);
  for (size_t pt = 0; pt < 12; ++pt) {
    size_t m_pt = (pt + 6) % 12;
    for (size_t h = 0; h < 8; ++h) {
      size_t m_h = 8 - h - 1;
      for (size_t w = 0; w < 8; ++w) {
        size_t idx = wrapped_idx({IP(pt,12), IP(h,8), IP(w,8)});
        assert(idx < bias_layer_one.size());
        size_t idx_c = wrapped_idx({IP(pt,12), IP(pt,12),
                                    IP(7,15), IP(7,15)});
        size_t m_idx_c = wrapped_idx({IP(m_pt,12), IP(m_pt,12),
                                    IP(7,15), IP(7,15)});
        for (size_t d = 0; d < block_size / 2; ++d) {
          size_t idx2 = wrapped_idx({IP(pt,12), IP(d, block_size / 2), IP(h, 8), IP(w, 8)});
          assert(idx2 < bias_layer_one.size() * block_size);
          bias_layer_one[idx][d] = gNetWeightsData[idx2 + offset];
          bias_layer_one[idx][d] += net_input_weights[idx_c][d];
          idx2 = wrapped_idx({IP(m_pt,12), IP(d, block_size / 2), IP(m_h, 8), IP(w, 8)});
          assert(idx2 < bias_layer_one.size() * block_size);
          bias_layer_one[idx][d + (block_size/2)] = gNetWeightsData[idx2 + offset];
          bias_layer_one[idx][d + (block_size/2)] += net_input_weights[m_idx_c][d];
        }
      }
    }
  }
  offset += 12 * 8 * 8 * block_size / 2;
}

void init_out_weights(size_t &offset) {
  assert(offset > 0);
  for (size_t pt = 0; pt < 12; ++pt) {
    for (size_t h = 0; h < 8; ++h) {
      for (size_t w = 0; w < 8; ++w) {
        for (size_t res = 0; res < 3; ++res) {
          size_t idx = wrapped_idx({IP(pt,12), IP(h,8), IP(w,8), IP(res,3)});
          assert(idx < output_weights.size());
          for (size_t d = 0; d < block_size; ++d) {
            size_t idx2 = wrapped_idx({IP(res,3), IP(pt,12), IP(d, block_size),
                                       IP(h, 8), IP(w, 8)});
            assert(idx2 < output_weights.size() * block_size);
            output_weights[idx][d] = gNetWeightsData[idx2 + offset];
          }
        }
      }
    }
  }
  offset += 12 * 8 * 8 * 3 * block_size;
}

void init_mirrored_out_weights(size_t &offset) {
  assert(offset > 0);
  for (size_t pt = 0; pt < 12; ++pt) {
    size_t m_pt = (pt + 6) % 12;
    for (size_t h = 0; h < 8; ++h) {
      size_t m_h = 8 - h - 1;
      for (size_t w = 0; w < 8; ++w) {
        for (size_t res = 0; res < 3; ++res) {
          size_t idx = wrapped_idx({IP(pt,12), IP(h,8), IP(w,8), IP(res,3)});
          assert(idx < output_weights.size());
          for (size_t d = 0; d < block_size; ++d) {
            size_t c = (2*d) / block_size;
            size_t idx2 = wrapped_idx({IP(res,3), IP(c,2), IP(pt,12),
                                       IP(d, block_size/2), IP(h, 8), IP(w, 8)});
            if (c) {
              idx2 = wrapped_idx({IP(res,3), IP(c,2), IP(m_pt,12),
                                  IP(d-(block_size/2), block_size/2), IP(m_h, 8), IP(w, 8)});
            }
            assert(idx2 < output_weights.size() * block_size);
            output_weights[idx][d] = gNetWeightsData[idx2 + offset];
          }
        }
      }
    }
  }
  offset += 12 * 8 * 8 * 3 * block_size;
}

void init_out_bias(size_t &offset) {
  output_bias[0] = gNetWeightsData[0 + offset];
  output_bias[1] = gNetWeightsData[1 + offset];
  output_bias[2] = gNetWeightsData[2 + offset];
  offset += 3;
}

void init_full_layer_weights(size_t &offset) {
  for (size_t pt = 0; pt < 12; ++pt) {
    for (size_t sq = 0; sq < 64; ++sq) {
      size_t idx = wrapped_idx({IP(pt,12), IP(sq,64)});
      assert(idx < full_layer_weights.size());
      for (size_t d = 0; d < full_block_size; ++d) {
        size_t idx2 = wrapped_idx({IP(d, full_block_size), IP(pt,12), IP(sq,64)});
        full_layer_weights[idx][d] = gNetWeightsData[idx2 + offset];
      }
    }
  }
  offset += 12 * 64 * full_block_size;
  for (size_t d = 0; d < full_block_size; ++d) {
    full_layer_bias[d] = gNetWeightsData[d + offset];
  }
  offset += full_block_size;
}

void init_mirrored_full_layer_weights(size_t &offset) {
  for (size_t pt = 0; pt < 12; ++pt) {
    size_t m_pt = (pt + 6) % 12;
    for (size_t sq = 0; sq < 64; ++sq) {
      size_t m_sq = GetMirroredSquare(sq);
      size_t idx = wrapped_idx({IP(pt,12), IP(sq,64)});
      assert(idx < full_layer_weights.size());
      for (size_t d = 0; d < full_block_size/2; ++d) {
        size_t idx2 = wrapped_idx({IP(d, full_block_size/2), IP(pt,12), IP(sq,64)});
        full_layer_weights[idx][d] = gNetWeightsData[idx2 + offset];
        idx2 = wrapped_idx({IP(d, full_block_size/2), IP(m_pt,12), IP(m_sq,64)});
        full_layer_weights[idx][d + (full_block_size/2)] = gNetWeightsData[idx2 + offset];
      }
    }
  }
  offset += 12 * 64 * full_block_size / 2;
  for (size_t d = 0; d < full_block_size/2; ++d) {
    full_layer_bias[d] = gNetWeightsData[d + offset];
    full_layer_bias[d+(full_block_size/2)] = gNetWeightsData[d + offset];
  }
  offset += full_block_size / 2;
}

void init_full_output_weights(size_t &offset) {
  for (size_t res = 0; res < 3; ++res) {
    for (size_t d = 0; d < full_block_size; ++d) {
      size_t idx2 = wrapped_idx({IP(res, 3), IP(d, full_block_size)});
      full_output_weights[res][d] = gNetWeightsData[idx2 + offset];
    }
  }
  offset += 3 * full_block_size;
}

void init_mirrored_outputs() {
  for (size_t pt = 0; pt < 12; ++pt) {
    size_t m_pt = (pt + 6) % 12;
    for (Square sq = 0; sq < 64; ++sq) {
      Square m_sq = GetMirroredSquare(sq);
      for (size_t res = 0; res < 3; ++res) {
        size_t idx = res + (pt * 64 + sq) * 3;
        size_t m_idx = res + (m_pt * 64 + m_sq) * 3;
        for (size_t w_idx = 0; w_idx < block_size / 2; ++w_idx) {
          m_output_weights[idx][w_idx] = output_weights[m_idx][w_idx + (block_size / 2)];
          m_output_weights[idx][w_idx + (block_size / 2)] = output_weights[m_idx][w_idx];
        }
      }
    }
  }
  
  for (size_t idx = 0; idx < full_output_weights.size(); ++idx) {
    for (size_t w_idx = 0; w_idx < full_block_size / 2; ++w_idx) {
      size_t m_idx = w_idx + (full_block_size / 2);
      m_full_output_weights[idx][w_idx] = full_output_weights[idx][m_idx];
      m_full_output_weights[idx][m_idx] = full_output_weights[idx][w_idx];
    }
  }
}

void init_weights() {
  init_square_offset();
  size_t offset = 0;
  
  init_mirrored_conv_weights(offset);
  init_mirrored_conv_bias_weights(offset);
  init_mirrored_out_weights(offset);
  init_out_bias(offset);
  init_mirrored_full_layer_weights(offset);
  init_full_output_weights(offset);
  init_mirrored_outputs();
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
  int32_t diff = score.win_draw - score.win;
  if (contempt[color] > 0) { // Contempt is positive, draws are counted as losses
    diff = (diff * contempt[color]) / 100;
    return WDLScore { score.win, score.win_draw - diff };
  }
  // contempt is negative, draws are counted as wins
  diff = -(diff * contempt[color]) / 100;
  return WDLScore { score.win + diff, score.win_draw};
}

Score RemoveContempt(Score score, Color color) {
  if (!score.is_static_eval() || contempt[color] == 0
      || contempt[color] >= 100 || contempt[color] <= -100) {
    return score;
  }
  int32_t diff = score.win_draw - score.win;
  if (contempt[color] >= 0) {
    int32_t orig_diff = (diff * 100) / (100 - contempt[color]);
    diff = orig_diff - diff;
    return WDLScore { score.win, score.win_draw + diff };
  }
  int32_t orig_diff = (diff * 100) / (100 + contempt[color]);
  diff = orig_diff - diff;
  return WDLScore { score.win - diff, score.win_draw };
}

}

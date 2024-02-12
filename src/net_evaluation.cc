#include "net_evaluation.h"
#include "general/types.h"
#include "incbin/incbin.h"

#include <array>
#include <vector>
#include <cmath>
#include <list>

//INCBIN(float_t, NetWeights, "rnet16H64g.bin");
//INCBIN(float_t, NetWeights, "rnet8H96e.bin");
//INCBIN(float_t, NetWeights, "rnet8H64j.bin");
INCBIN(float_t, NetWeights, "rnet8HD64.bin");


// NN types
constexpr size_t block_size = 16;
using NetLayerType = Vec<float_t, block_size>;

constexpr size_t full_block_size = 128;
using FullLayerType = Vec<float_t, full_block_size>;

std::array<int32_t, 2> contempt = { 0, 0 };

namespace {

// NN weights

std::vector<NetLayerType> net_input_weights(12 * 12 * 15 * 15, 0);
std::vector<NetLayerType> bias_layer_one(12 * 8 * 8, 0);

std::vector<NetLayerType> output_weights(3 * 12 * 8 * 8, 0);
std::array<float_t, 3> output_bias;

std::vector<FullLayerType> full_layer_weights(12 * 64, 0);
FullLayerType full_layer_bias(0);

std::vector<FullLayerType> full_output_weights(3, 0);

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

struct NetPieceModule {
  PieceType pt;
  Square sq;
  NetLayerType features;
  NetPieceModule() = default;
};

void AddRelative(const NetPieceModule &p_src, NetPieceModule &p_des) {
  size_t idx = (p_src.pt * 12 + p_des.pt) * 225 + square_offset[p_src.sq][p_des.sq];
  p_des.features += net_input_weights[idx];
}

void EvalPieceRelations(std::vector<NetPieceModule> &piece_modules) {
  for (size_t i = 0; i < piece_modules.size(); ++i) {
    for (size_t j = i+1; j < piece_modules.size(); ++j) {
      AddRelative(piece_modules[i], piece_modules[j]);
      AddRelative(piece_modules[j], piece_modules[i]);
    }
  }
}

template<Color color, Color our_color>
inline void AddPieceType(const Board &board, const PieceType pt,
                         std::vector<NetPieceModule> &piece_modules,
                         FullLayerType &full_layer) {
  constexpr size_t c_offset = color == our_color ? 0 : 6;
  
  for (BitBoard pieces = board.get_piece_bitboard(color, pt); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    if (our_color == kBlack) {
        piece_square = GetMirroredSquare(piece_square);
    }
    size_t bias_idx = (pt + c_offset) * 8 * 8 + piece_square;
    NetPieceModule npm;
    npm.pt = pt + c_offset;
    npm.sq = piece_square;
    npm.features = bias_layer_one[bias_idx];
    piece_modules.push_back(npm);
    
    full_layer += full_layer_weights[(pt + c_offset) * 64 + piece_square];
  }
}

template<Color our_color>
inline void AddAllPieceTypes(const Board &board,
                         std::vector<NetPieceModule> &piece_modules,
                         FullLayerType &full_layer) {
  for (PieceType piece_type = kPawn; piece_type <= kKing; ++piece_type) {
      AddPieceType<kWhite, our_color>(board, piece_type, piece_modules, full_layer);
      AddPieceType<kBlack, our_color>(board, piece_type, piece_modules, full_layer);
  }
}

}

namespace net_evaluation {

Score NetForward(std::vector<NetPieceModule> &piece_modules, FullLayerType &full_layer) {
  std::vector<NetLayerType> output_helpers(3, 0);
  for (size_t piece_idx = 0; piece_idx < piece_modules.size(); piece_idx++) {
    piece_modules[piece_idx].features.clipped_relu(8);
    size_t idx = 3 * (piece_modules[piece_idx].pt * 8 * 8 + piece_modules[piece_idx].sq);
    assert(idx + 2 < output_weights.size());
    for (size_t output_idx = 0; output_idx < 3; output_idx++) {
      output_helpers[output_idx].FMA(output_weights[idx + output_idx], piece_modules[piece_idx].features);
    }
  }
  
  full_layer.clipped_relu(8);
  
  float_t sum = 0;
  std::array<float_t, 3> outcomes;
  for (size_t i = 0; i < 3; ++i) {
    outcomes[i] = output_helpers[i].sum() + output_bias[i]
                  + full_layer.dot(full_output_weights[i]);
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

Score ScoreBoard(const Board &board) {
  std::vector<NetPieceModule> piece_modules;
  piece_modules.reserve(32);
  FullLayerType full_layer = full_layer_bias;
  if (board.get_turn() == kWhite) {
    AddAllPieceTypes<kWhite>(board, piece_modules, full_layer);
  }
  else {
    AddAllPieceTypes<kBlack>(board, piece_modules, full_layer);
  }
  EvalPieceRelations(piece_modules);
  if (contempt[board.get_turn()] != 0) {
    return AddContempt(NetForward(piece_modules, full_layer), board.get_turn());
  }
  return NetForward(piece_modules, full_layer);
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
          // size_t idx = (((((piece_in * 12) + piece_out) * 15) + h) * 15) + w;
          //idx *= block_size;
          for (size_t d = 0; d < block_size ; ++d) {
            size_t idx2 = wrapped_idx({IP(piece_out,12), IP(d,block_size),
                                       IP(piece_in,12), IP(h,15), IP(w,15)});
            //size_t idx2 = (((piece_out * block_size) + d) * 12) + piece_in;
            //idx2 = (idx2 * 15 + h) * 15 + w;
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
          // size_t idx = (((((piece_in * 12) + piece_out) * 15) + h) * 15) + w;
          //idx *= block_size;
          for (size_t d = 0; d < (block_size / 2); ++d) {
            size_t idx2 = wrapped_idx({IP(piece_out,12), IP(d, (block_size / 2)),
                                       IP(piece_in,12), IP(h,15), IP(w,15)});
            size_t m_idx = wrapped_idx({IP(m_piece_out,12),
                                        IP(d, (block_size / 2)),
                                        IP(m_piece_in,12),
                                        IP(m_h,15), IP(w,15)});
            //size_t idx2 = (((piece_out * block_size) + d) * 12) + piece_in;
            //idx2 = (idx2 * 15 + h) * 15 + w;
            //assert(idx2 < net_input_weights.size() * block_size);
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
            //  idx2 = wrapped_idx({IP(res,3), IP(pt,12), IP(d, block_size),
            //                           IP(m_h, 8), IP(w, 8)});
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

void init_weights() {
  init_square_offset();
  size_t offset = 0;
  init_mirrored_conv_weights(offset);
  init_mirrored_conv_bias_weights(offset);
  init_mirrored_out_weights(offset);
  init_out_bias(offset);
  init_mirrored_full_layer_weights(offset);
  init_full_output_weights(offset);
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

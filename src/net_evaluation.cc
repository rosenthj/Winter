#include "net_evaluation.h"
#include "general/types.h"
#include "incbin/incbin.h"

#include <array>
#include <vector>
#include <cmath>

INCBIN(float_t, NetWeights, "f160rS02_ep8.bin");

// NN types
constexpr size_t block_size = 160;
using NetLayerType = Vec<float, block_size>;
using FNetLayerType = Vec<SIMDFloat, block_size>;

std::array<int32_t, 2> contempt = { 0, 0 };

namespace {

// NN weights

std::vector<NetLayerType> net_input_weights(772, 0);
NetLayerType bias_layer_one(0);

std::vector<NetLayerType> castling_right_weights(1 << 5, 0);

//std::vector<NetLayerType> second_layer_weights(16 * 16, 0);
//NetLayerType bias_layer_two(0);

std::vector<NetLayerType> output_weights;
std::array<float_t, 3> output_bias;

template<typename T>
T init() {
  return T(0);
}

//template<typename T> inline
//void AddFeature(T &s, const int index, const int value) {
//  s[index] += value;
//}

//template<> inline void AddFeature<NetLayerType>(NetLayerType &s, const int index, const int value) {
//  s.FMA(net_input_weights[index], value);
//}

template<typename T> inline
void AddFeature(T &s, const int index) {
  assert(index >= 0);
  assert(index < 772);
  s[index]++;
}

template<> inline void AddFeature<NetLayerType>(NetLayerType &s, const int index) {
  assert(index >= 0);
  assert(index < 772);
  s += net_input_weights[index];
}

template<typename T> inline
void AddCastlingFeatures(T &s, const int index) {
  assert(index >= 0);
  assert(index < (1<<5));
  s[index]++;
}

template<> inline void AddCastlingFeatures<NetLayerType>(NetLayerType &s, const int index) {
  assert(index >= 0);
  assert(index < (1<<5));
  s += castling_right_weights[index];
}

template<typename T, Color color, Color our_color>
inline void AddPieceType(T &score, const Board &board, const PieceType pt) {
  constexpr size_t c_offset = color == our_color ? 0 : 64 * 6;
  const size_t offset = 64 * pt + c_offset;
  
  for (BitBoard pieces = board.get_piece_bitboard(color, pt); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    if (our_color == kBlack) {
        piece_square = GetMirroredSquare(piece_square);
    }
    //score += net_input_weights[piece_square + offset];
    AddFeature<T>(score, piece_square + offset);
  }
}

template<typename T, Color color, Color our_color>
inline void ScorePieces(T &score, const Board &board) {
  for (PieceType piece_type = kPawn; piece_type <= kKing; ++piece_type) {
      AddPieceType<T, color, our_color>(score, board, piece_type);
  }
}

template<typename T, Color color>
inline void ScoreCastlingRights(T &score, const Board &board) {
  const CastlingRights castling_rights = board.get_castling_rights();
  if (color == kWhite) {
    AddCastlingFeatures<T>(score, castling_rights);
  }
  else {
    AddCastlingFeatures<T>(score, castling_rights + (1<<4));
  }
}

}

namespace net_evaluation {

template<typename T, Color our_color>
T ScoreBoard(const Board &board) {
  T score = init<T>();

  // Piece evaluations
  ScorePieces<T, kWhite, our_color>(score, board);
  ScorePieces<T, kBlack, our_color>(score, board);
  
  ScoreCastlingRights<T, our_color>(score, board);

  return score;
}

Score NetForward(NetLayerType &layer_one) {
  layer_one += bias_layer_one;
  layer_one.relu();
  
  float_t sum = 0;
  std::array<float_t, 3> outcomes;
  for (size_t i = 0; i < 3; ++i) {
      outcomes[i] = layer_one.dot(output_weights[i]) + output_bias[i];
      outcomes[i] = std::exp(outcomes[i]);
      sum += outcomes[i];
  }
  
  for (size_t i = 0; i < 3; ++i) {
      outcomes[i] /= sum;
  }

  //NetLayerType layer_two = bias_layer_two;
  //for (size_t i = 0; i < layer_one.size(); ++i) {
  //  layer_two.FMA(second_layer_weights[i], layer_one[i]);
  //}
  //layer_two.relu();
  
  float win = outcomes[0];
  float win_draw = outcomes[0] + outcomes[1];
  return WDLScore::from_pct_valid(win, win_draw);

  //float wpct = sigmoid(win) * c + sigmoid(win_draw) * (1 - c);
  //return wpct_to_score(wpct);
}

Score ScoreBoard(const Board &board) {

  NetLayerType layer_one = init<NetLayerType>();
  if (board.get_turn() == kWhite) {
    layer_one = ScoreBoard<NetLayerType, kWhite>(board);
  }
  else {
    layer_one = ScoreBoard<NetLayerType, kBlack>(board);
  }
  if (contempt[board.get_turn()] != 0) {
    return AddContempt(NetForward(layer_one), board.get_turn());
  }
  return NetForward(layer_one);
}

std::vector<NetLayerType> load_weights(size_t in_size, size_t &offset) {
  std::vector<NetLayerType> weights = std::vector<NetLayerType>(in_size);
  for (size_t i = 0; i < in_size; ++i) {
    for (size_t k = 0; k < block_size; ++k) {
      weights[i][k] = gNetWeightsData[offset + i + in_size * k];
    }
  }
  offset += in_size * block_size;
  return weights;
}

void init_weights() {
  // Init regular net weights
  
  size_t offset = 0;
  
  // L1 Weights
  net_input_weights = load_weights(772, offset);
  
  // L1 Bias
  for (size_t k = 0; k < block_size; ++k) {
    bias_layer_one[k] = gNetWeightsData[offset+k];
  }
  offset += block_size;
  
  
  // Output Weights
  output_weights = std::vector<NetLayerType>(3);
  for (size_t i = 0; i < block_size; ++i) {
    //size_t j = i * block_size;
    for (size_t k = 0; k < 3; ++k) {
      output_weights[k][i] = gNetWeightsData[offset+i+block_size*k];
    }
  }
  offset += 3 * block_size;
  
  // Output Bias
  for (size_t k = 0; k < 3; ++k) {
    output_bias[k] = gNetWeightsData[offset+k];
  }
  
  for (int castling_rights = 0; castling_rights < (1 << 5); ++castling_rights) {
    Color color = castling_rights <= 0xF ? kWhite : kBlack;
    int offset_us = color == kWhite ? 0 : 2;
    int offset_them = color == kWhite? 2 : 0;
    size_t base_idx = 12 * 64;
    if (castling_rights & (kWLCastle << offset_us)) {
      castling_right_weights[castling_rights] += net_input_weights[base_idx + 0];
    }
    if (castling_rights & (kWSCastle << offset_us)) {
      castling_right_weights[castling_rights] += net_input_weights[base_idx + 1];
    }
    if (castling_rights & (kWLCastle << offset_them)) {
      castling_right_weights[castling_rights] += net_input_weights[base_idx + 2];
    }
    if (castling_rights & (kWSCastle << offset_them)) {
      castling_right_weights[castling_rights] += net_input_weights[base_idx + 3];
    }
    
  }
  
}

std::vector<int32_t> GetNetInputs(const Board &board) {
  if (board.get_turn() == kWhite) {
    return ScoreBoard<std::vector<int32_t>, kWhite>(board);
  }
  return ScoreBoard<std::vector<int32_t>, kBlack>(board);
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

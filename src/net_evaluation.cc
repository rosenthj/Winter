#include "net_evaluation.h"
#include "general/types.h"
#include "incbin/incbin.h"

#include <array>
#include <vector>
#include <cmath>

INCBIN(float_t, NetWeights, "g224rS07_ep3.bin");
//INCBIN(float_t, NetWeights, "f224rS15_ep4.bin");
//INCBIN(float_t, NetWeights, "f192rS12_ep4.bin");
//INCBIN(float_t, NetWeights, "f256G32rS01b_ep3.bin");
//INCBIN(float_t, NetWeights, "f256A32rS06_ep4.bin");
//INCBIN(float_t, NetWeights, "f224rS08_ep3.bin");

// NN types
constexpr size_t block_size = 224;
using NetLayerType = Vec<float, block_size>;
constexpr size_t reduced_block_size = 224;
using ReducedNetLayerType = Vec<float, reduced_block_size>;

std::array<int32_t, 2> contempt = { 0, 0 };

namespace {

// NN weights

std::vector<NetLayerType> net_input_weights(772, 0);
NetLayerType bias_layer_one(0);

//std::vector<NetLayerType> second_layer_weights(16 * 16, 0);
//NetLayerType bias_layer_two(0);

//NetLayerType reduction_weights(0);
//ReducedNetLayerType reduction_bias(0);

std::vector<ReducedNetLayerType> output_weights;
std::array<float_t, 3> output_bias;

template<typename T>
T init() {
  return T(0);
}

template<typename T> inline
void AddFeature(T &s, const int index) {
  assert(index >= 0);
  assert(index < 772);
  s[index]++;
}

template<> void AddFeature<NetLayerType>(NetLayerType &s, const int index) {
  assert(index >= 0);
  assert(index < 772);
  s += net_input_weights[index];
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
  constexpr int offset_us = color == kWhite ? 0 : 2;
  constexpr int offset_them = color == kWhite? 2 : 0;
  constexpr size_t base_castling_right_idx = 12 * 64;
  
  const CastlingRights castling_rights = board.get_castling_rights();
  if (castling_rights & (kWLCastle << offset_us)) {
      AddFeature<T>(score, base_castling_right_idx + 0);
  }
  if (castling_rights & (kWSCastle << offset_us)) {
      AddFeature<T>(score, base_castling_right_idx + 1);
  }
    if (castling_rights & (kWLCastle << offset_them)) {
      AddFeature<T>(score, base_castling_right_idx + 2);
  }
  if (castling_rights & (kWSCastle << offset_them)) {
      AddFeature<T>(score, base_castling_right_idx + 3);
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

Score NetForward(NetLayerType &layer_one_) {
  layer_one_ += bias_layer_one;
  layer_one_.clipped_relu(8);
  
  //ReducedNetLayerType layer_one = layer_one_.reduce_sum<32>();
  
  //ReducedNetLayerType layer_one = layer_one_.reduce_weighted<32>(reduction_weights);
  //layer_one += reduction_bias;
  //layer_one.relu();
  
  float_t sum = 0;
  std::array<float_t, 3> outcomes;
  for (size_t i = 0; i < 3; ++i) {
      outcomes[i] = layer_one_.dot(output_weights[i]) + output_bias[i];
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
  
  /*const size_t group_size = block_size / reduced_block_size;
  for (size_t k = 0; k < block_size; ++k) {
    size_t group_id = k / group_size;
    size_t member_id = k % group_size;
    size_t id = member_id * reduced_block_size + group_id;
    reduction_weights[id] = gNetWeightsData[offset+k];
  }
  offset += block_size;
  
  for (size_t k = 0; k < reduced_block_size; ++k) {
    reduction_bias[k] = gNetWeightsData[offset+k];
  }
  offset += reduced_block_size;*/
  
  
  // Output Weights
  output_weights = std::vector<ReducedNetLayerType>(3);
  for (size_t i = 0; i < reduced_block_size; ++i) {
    //size_t j = i * block_size;
    for (size_t k = 0; k < 3; ++k) {
      output_weights[k][i] = gNetWeightsData[offset+i+reduced_block_size*k];
    }
  }
  offset += 3 * reduced_block_size;
  
  // Output Bias
  for (size_t k = 0; k < 3; ++k) {
    output_bias[k] = gNetWeightsData[offset+k];
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

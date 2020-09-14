#include "cnn_net_weights.h"
#include "data.h"
#include "net_evaluation.h"
#include "net_weights.h"
#include "general/magic.h"
#include "general/types.h"

#include <random>
#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace net_features;

// NN types
constexpr size_t block_size = 16;
constexpr size_t cnn_dense_in = 4 * block_size;
// The (post-) activation block size is only needed if dimension is different from preactivation
//constexpr size_t act_block_size = 2 * block_size;
using NetLayerType = Vec<float, block_size>;
using FNetLayerType = Vec<SIMDFloat, block_size>;

// CNN types
using CNNLayerType = Array2d<NetLayerType, 8, 8>;
using Filter = Array2d<NetLayerType, 3, 3>;
// The net inputs are very sparse (less than 18/640)
// so it makes sense to rely on deconvolution instead of regular conv.
using DeconvFilter = Array2d<NetLayerType, 3, 3>;

//using CReLULayerType = Vec<float, act_block_size>;
std::array<float, 2> contempt = { 0.5, 0.5 };

struct CNNHelper {
  std::vector<Square> our_p;
  Square our_k;
  std::vector<Square> opp_p;
  Square opp_k;
};

namespace pawn_hash {

// long hits = 0;
// long misses = 0;

struct PawnEntry {
  NetLayerType output;
  HashType hash;
};

size_t size = 10000;

std::vector<PawnEntry> table(size);

PawnEntry GetEntry(const HashType hash_p) {
  return table[hash_p % table.size()];
}
void SaveEntry(const NetLayerType &output, const HashType hash_p) {
  PawnEntry entry;
  entry.output = output;
  entry.hash = hash_p ^ (HashType)std::round(output[0] * 1024);
  table[hash_p % table.size()] = entry;
}

bool ValidateHash(const PawnEntry &entry, const HashType hash_p) {
  return hash_p == (entry.hash ^ (HashType)std::round(entry.output[0] * 1024));
}

}

namespace {
const int net_version = 20051601; // Unused warning is expected.

constexpr bool kUseQueenActivity = false;

inline float sigmoid(float x) {
  return 1 / (1 + std::exp(-x));
}

// CNN Weights

// Filters for non-const channels
Array3d<NetLayerType, 5, 5, 10> cnn_l1_filters;

// After the first convolution of the CNN, there is a constant bias.
// This corresponds to the bias b at every location as well as
// the output of the convolutions on the constant inputs.
CNNLayerType cnn_input_bias;

Array3d<FNetLayerType, 3, 3, 16> cnn_our_p_filters, cnn_our_k_filters, cnn_opp_p_filters, cnn_opp_k_filters;
NetLayerType cnn_our_p_bias, cnn_our_k_bias, cnn_opp_p_bias, cnn_opp_k_bias;

std::vector<NetLayerType> cnn_dense_out(cnn_dense_in, 0);

// NN weights

std::vector<NetLayerType> net_input_weights(kTotalNumFeatures, 0);
NetLayerType bias_layer_one(0);

std::vector<NetLayerType> second_layer_weights(16 * 16, 0);
NetLayerType bias_layer_two(0);

NetLayerType win_weights(0);
float win_bias;
NetLayerType win_draw_weights(0);
float win_draw_bias;

constexpr std::array<int, 64> kPSTindex = {
    0, 1, 2, 3, 3, 2, 1, 0,
    1, 4, 5, 6, 6, 5, 4, 1,
    2, 5, 7, 8, 8, 7, 5, 2,
    3, 6, 8, 9, 9, 8, 6, 3,
    3, 6, 8, 9, 9, 8, 6, 3,
    2, 5, 7, 8, 8, 7, 5, 2,
    1, 4, 5, 6, 6, 5, 4, 1,
    0, 1, 2, 3, 3, 2, 1, 0
};

const Array2d<BitBoard, 2, 64> get_king_pawn_coverage() {
  Array2d<BitBoard, 2, 64> coverage;
  for (Square square = 0; square < 64; square++) {
    BitBoard bb = GetSquareBitBoard(square);
    for (int i = GetSquareY(square); i; --i) {
      bb |= bitops::E(bb) | bitops::W(bb);
    }
    bb = bitops::FillNorthEast(bb, ~0) | bitops::FillNorthWest(bb, ~0);
    bb = bitops::FillNorth(bb, ~0);
    bb &= ~bitops::seventh_rank | bitops::N(bb);
    coverage[kWhite][square] = bb;
    bb = GetSquareBitBoard(square);
    for (int i = 7 - GetSquareY(square); i; --i) {
      bb |= bitops::E(bb) | bitops::W(bb);
    }
    bb = bitops::FillSouthEast(bb, ~0) | bitops::FillSouthWest(bb, ~0);
    bb = bitops::FillSouth(bb, ~0);
    bb &= ~bitops::second_rank | bitops::S(bb);
    coverage[kBlack][square] = bb;
  }
  return coverage;
}

const Array2d<BitBoard, 2, 64> king_pawn_coverage = get_king_pawn_coverage();

const std::array<int, 15*15> relative_king_map = {
    0,  1,  2,  3,  4,  5,  6,  7,  6,  5,  4,  3,  2,  1, 8,
    1,  8,  9, 10, 11, 12, 13, 14, 13, 12, 11, 10,  9,  8, 1,
    2,  9, 15, 16, 17, 18, 19, 20, 19, 18, 17, 16, 15,  9, 2,
    3, 10, 16, 21, 22, 23, 24, 25, 24, 23, 22, 21, 16, 10, 3,
    4, 11, 17, 22, 26, 27, 28, 29, 28, 27, 26, 22, 17, 11, 4,
    5, 12, 18, 23, 27, 30, 31, 32, 31, 30, 27, 23, 18, 12, 5,
    6, 13, 19, 24, 28, 31, 33, 34, 33, 31, 28, 24, 19, 13, 6,
    7, 14, 20, 25, 29, 32, 34, 35, 34, 32, 29, 25, 20, 14, 7,
    6, 13, 19, 24, 28, 31, 33, 34, 33, 31, 28, 24, 19, 13, 6,
    5, 12, 18, 23, 27, 30, 31, 32, 31, 30, 27, 23, 18, 12, 5,
    4, 11, 17, 22, 26, 27, 28, 29, 28, 27, 26, 22, 17, 11, 4,
    3, 10, 16, 21, 22, 23, 24, 25, 24, 23, 22, 21, 16, 10, 3,
    2,  9, 15, 16, 17, 18, 19, 20, 19, 18, 17, 16, 15,  9, 2,
    1,  8,  9, 10, 11, 12, 13, 14, 13, 12, 11, 10,  9,  8, 1,
    8,  1,  2,  3,  4,  5,  6,  7,  6,  5,  4,  3,  2,  1, 0
};

template<typename T>
T init() {
  return T(kTotalNumFeatures);
}

template<typename T>
T init_cnn_in() {
  return T(kBoardSize * kNumChannels);
}

template<>
CNNLayerType init_cnn_in() {
  return cnn_input_bias;
}

template<>
NetLayerType init() {
  return NetLayerType(0);
}

//template<> PScore init<PScore>() { return PScore(0); }

template<typename T> inline
void AddFeature(T &s, const int index, const int value) {
  s[index] += value;
}

template<> inline void AddFeature<NetLayerType>(NetLayerType &s, const int index, const int value) {
  s.FMA(net_input_weights[index], value);
}

// TODO refactor AddFeaturePair to utilize AddFeature instead.
template<typename T, Color our_color> inline
void AddFeaturePair(T &s, const int index, const int value_white, const int value_black) {
  if (our_color == kWhite) {
    s[index] += value_white;
    s[index + kSideDependentFeatureCount] += value_black;
  }
  else {
    s[index] += value_black;
    s[index + kSideDependentFeatureCount] += value_white;
  }
}

template<> inline
void AddFeaturePair<NetLayerType, kWhite>(NetLayerType &s, const int index, const int value_white, const int value_black) {
  s.FMA(net_input_weights[index], value_white);
  s.FMA(net_input_weights[index + kSideDependentFeatureCount], value_black);
}

template<> inline
void AddFeaturePair<NetLayerType, kBlack>(NetLayerType &s, const int index,
                                          const int value_white, const int value_black) {
  s.FMA(net_input_weights[index], value_black);
  s.FMA(net_input_weights[index + kSideDependentFeatureCount], value_white);
}

template<typename T> inline
void AddRawBoardFeature(T &s, const int channel, const Square square) {
  s[square * kNumChannels + channel]++;
}

template<> inline
void AddRawBoardFeature(CNNLayerType &s, const int channel, const Square square) {
  int h = GetSquareY(square);
  int w = GetSquareX(square);
  for (int i = 0; i < 5; ++i) {
    if (h+i <= 1 || h+i >= 10) {
      continue;
    }
    for (int j = 0; j < 5; ++j) {
      if (w+j <= 1 || w+j >= 10) {
        continue;
      }
      s[h+i-2][w+j-2] += cnn_l1_filters[4-i][4-j][channel];
    }
  }
}

BitBoard get_all_major_pieces(const Board &board) {
  BitBoard all_major_pieces = 0;
  for (PieceType piece = kRook; piece <= kKing; piece++) {
    all_major_pieces |= board.get_piecetype_bitboard(piece);
  }
  return all_major_pieces;
}

std::array<BitBoard, 2> get_p_forward(const std::array<BitBoard, 2> &pawn_bb, const BitBoard empty) {
  std::array<BitBoard, 2> p_forward {
      bitops::N(pawn_bb[kWhite]) & empty,
      bitops::S(pawn_bb[kBlack]) & empty
  };

  p_forward[kWhite] |= bitops::N(p_forward[kWhite]) & empty & bitops::fourth_rank;
  p_forward[kBlack] |= bitops::S(p_forward[kBlack]) & empty & bitops::fifth_rank;
  return p_forward;
}

std::array<BitBoard, 2> get_passed(const std::array<BitBoard, 2> &p_fill_forward,
                                   const std::array<BitBoard, 2> &pawn_bb) {
  BitBoard n_filled = bitops::NW(p_fill_forward[kWhite]) | bitops::NE(p_fill_forward[kWhite]) | p_fill_forward[kWhite];
  BitBoard s_filled = bitops::SW(p_fill_forward[kBlack]) | bitops::SE(p_fill_forward[kBlack]) | p_fill_forward[kBlack];
  return {
    pawn_bb[kWhite] & (~(s_filled | bitops::S(bitops::FillSouth(pawn_bb[kWhite], ~0)))),
    pawn_bb[kBlack] & (~(n_filled | bitops::N(bitops::FillNorth(pawn_bb[kBlack], ~0))))
  };
}

struct EvalConstants;

struct CheckingSquares {
  CheckingSquares(const BitBoard all_pieces, const std::array<Square, 2> king_squares,
                  const std::array<BitBoard, 2> c_pieces, const std::array<BitBoard, 2> controlled) :
    safe{{ {0, 0, 0, 0}, {0, 0, 0, 0} }},
    unsafe{{ {0, 0, 0, 0}, {0, 0, 0, 0} }}
  {
    for (Color color = kWhite; color <= kBlack; color++) {
      Color not_color = color ^ 0x1;
      Square enemy_king = king_squares[not_color];
      unsafe[color][kKnight - kKnight] = magic::GetAttackMap<kKnight>(enemy_king, all_pieces);
      unsafe[color][kBishop - kKnight] = magic::GetAttackMap<kBishop>(enemy_king, all_pieces);
      unsafe[color][kRook - kKnight] = magic::GetAttackMap<kRook>(enemy_king, all_pieces);
      unsafe[color][kQueen - kKnight] = unsafe[color][kRook - kKnight]
                                                 | unsafe[color][kBishop - kKnight];
      BitBoard safe_squares = ~(c_pieces[color] | controlled[not_color]);
      for (PieceType pt = kKnight - kKnight; pt <= kQueen - kKnight; pt++) {
        safe[color][pt] = safe_squares & unsafe[color][pt];
        unsafe[color][pt] &= ~(c_pieces[color]);
        unsafe[color][pt] ^= safe[color][pt];
      }
    }
  }
  Array2d<BitBoard, 2, 4> safe;
  Array2d<BitBoard, 2, 4> unsafe;
};

struct EvalConstants {
  EvalConstants(const Board &board) :
    king_squares({
        bitops::NumberOfTrailingZeros(board.get_piece_bitboard(kWhite, kKing)),
        bitops::NumberOfTrailingZeros(board.get_piece_bitboard(kBlack, kKing))
    }),
    all_major_pieces(get_all_major_pieces(board)),
    major_pieces({ all_major_pieces & board.get_color_bitboard(kWhite),
                   all_major_pieces & board.get_color_bitboard(kBlack) }),
    c_pieces({board.get_color_bitboard(kWhite), board.get_color_bitboard(kBlack)}),
    all_pieces(board.get_color_bitboard(kWhite) | board.get_color_bitboard(kBlack)),
    empty(~all_pieces),
    pawn_bb({
        board.get_piece_bitboard(kWhite, kPawn),
        board.get_piece_bitboard(kBlack, kPawn)
    }),
    all_pawns_bb(pawn_bb[kWhite] | pawn_bb[kBlack]),
    covered_once({
        bitops::NE(pawn_bb[kWhite]) | bitops::NW(pawn_bb[kWhite]),
        bitops::SE(pawn_bb[kBlack]) | bitops::SW(pawn_bb[kBlack])
    }),
    covered_potentially({
        bitops::FillNorth(covered_once[kWhite], ~0),
        bitops::FillSouth(covered_once[kBlack], ~0)
    }),
    covered_twice({
        bitops::NE(pawn_bb[kWhite]) & bitops::NW(pawn_bb[kWhite]),
        bitops::SE(pawn_bb[kBlack]) & bitops::SW(pawn_bb[kBlack])
    }),
    hard_block({
        (pawn_bb[kWhite] & ~(bitops::S(empty) | bitops::SE(c_pieces[kBlack])
                                             | bitops::SW(c_pieces[kBlack])))
        | (pawn_bb[kBlack] & ((covered_once[kBlack] & ~covered_once[kWhite])
                              |(covered_twice[kBlack] & ~covered_twice[kWhite]))),
        (pawn_bb[kBlack] & ~(bitops::N(empty) | bitops::NE(c_pieces[kWhite])
                                             | bitops::NW(c_pieces[kWhite])))
        | (pawn_bb[kWhite] & ((covered_once[kWhite] & ~covered_once[kBlack])
                              |(covered_twice[kWhite] & ~covered_twice[kBlack]))),
    }),
    p_forward(get_p_forward(pawn_bb, empty)),
    p_fill_forward({bitops::FillNorth(pawn_bb[kWhite], ~0),
                    bitops::FillSouth(pawn_bb[kBlack], ~0)}),
    opposed_pawns((p_fill_forward[kBlack] & pawn_bb[kWhite])
                  | (p_fill_forward[kWhite] & pawn_bb[kBlack])),
    neighbored_pawns({
                      (bitops::E(pawn_bb[kWhite]) | bitops::W(pawn_bb[kWhite])) & pawn_bb[kWhite],
                      (bitops::E(pawn_bb[kBlack]) | bitops::W(pawn_bb[kBlack])) & pawn_bb[kBlack]
                  }),
    passed(get_passed(p_fill_forward, pawn_bb)),
    controlled({
                board.PlayerBitBoardControl(kWhite, all_pieces),
                board.PlayerBitBoardControl(kBlack, all_pieces)
              }),
    nbr_bitboard(board.get_piecetype_bitboard(kKnight)
                 | board.get_piecetype_bitboard(kBishop)
                 | board.get_piecetype_bitboard(kRook)),
    checks(all_pieces, king_squares, c_pieces, controlled) {}

  const std::array<Square, 2> king_squares;           // Squares of each respective king
  const BitBoard all_major_pieces;                    // Kings, Queens and Rooks
  const std::array<BitBoard, 2> major_pieces;         // Kings, Queens and Rooks, by color
  const std::array<BitBoard, 2> c_pieces;             // All pieces, by color
  const BitBoard all_pieces;                          // BitBoard of all pieces
  const BitBoard empty;                               // ~all_pieces
  const std::array<BitBoard, 2> pawn_bb;              // Pawn BitBoard, by color
  const BitBoard all_pawns_bb;                        // BitBoard of all pawns
  const std::array<BitBoard, 2> covered_once;         // BitBoard of squares covered by pawns at least once, by color
  const std::array<BitBoard, 2> covered_potentially;
  const std::array<BitBoard, 2> covered_twice;
  const std::array<BitBoard, 2> hard_block;
  const std::array<BitBoard, 2> p_forward;
  const std::array<BitBoard, 2> p_fill_forward;
  const BitBoard opposed_pawns;
  const std::array<BitBoard, 2> neighbored_pawns;
  const std::array<BitBoard, 2> passed;
  const std::array<BitBoard, 2> controlled;
  const BitBoard nbr_bitboard;
  const CheckingSquares checks;
};

struct EvalCounter {
  Vec<int, (kQueen - kPawn)> king_zone_attacks = Vec<int, (kQueen - kPawn)>(0);
  int king_attack_count = 0;
  int safe = 0;
  int unsafe = 0;
};

enum PawnCategory {
  Opposed = 0, Unopposed = 1, PassedCovered = 2, PassedUncovered = 3
};

template<typename T, Color color, Color our_color, PawnCategory cat>
inline void AddPawnsToInputByGroup(T &score, const EvalConstants &ec, BitBoard cat_pawns, CNNHelper &helper) {
  constexpr size_t offset = color == our_color ? 0 : kChannelsPerSide;

  for (; cat_pawns; bitops::PopLSB(cat_pawns)) {
    Square pawn_square = bitops::NumberOfTrailingZeros(cat_pawns);
    if (our_color == kBlack) {
      pawn_square = GetMirroredSquare(pawn_square);
    }
    if (color == our_color) {
      helper.our_p.emplace_back(pawn_square);
    }
    else {
      helper.opp_p.emplace_back(pawn_square);
    }
    AddRawBoardFeature<T>(score, cat + offset, pawn_square);
  }
}

template<typename T, Color color, Color our_color>
inline void ScorePawnThreats(T &score, const EvalConstants &ec) {
  constexpr Color not_color = color ^ 0x1;
  constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  AddFeature<T>(score, offset + kPawnAttackIndex,
                bitops::PopCount(ec.covered_once[color]
                                & (ec.c_pieces[not_color] ^ ec.pawn_bb[not_color])));
}

template<typename T, Color color, Color our_color>
inline void AddPawnsToInput(T &score, const EvalConstants &ec, CNNHelper &helper) {
  constexpr Color not_color = color ^ 0x1;

  AddPawnsToInputByGroup<T, color, our_color, PawnCategory::Opposed>(
      score, ec, ec.pawn_bb[color] & ec.opposed_pawns, helper);
  AddPawnsToInputByGroup<T, color, our_color, PawnCategory::Unopposed>(
      score, ec, ec.pawn_bb[color] & ~(ec.opposed_pawns | ec.passed[color]), helper);
  AddPawnsToInputByGroup<T, color, our_color, PawnCategory::PassedCovered>(
      score, ec, ec.passed[color] & king_pawn_coverage[not_color][ec.king_squares[not_color]], helper);
  AddPawnsToInputByGroup<T, color, our_color, PawnCategory::PassedUncovered>(
      score, ec, ec.passed[color] & ~king_pawn_coverage[not_color][ec.king_squares[not_color]], helper);
}

template<typename T, Color color, Color our_color>
inline void ScoreKnights(T &score, const Board &board, const EvalConstants &ec,
                        EvalCounter &counter, const BitBoard enemy_king_zone) {
  constexpr Color not_color = color ^ 0x1;
  constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  for (BitBoard pieces = board.get_piece_bitboard(color, kKnight); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    AddFeature<T>(score, offset + kKnightPSTIdx + kPSTindex[piece_square], 1);
    AddFeature<T>(score, offset + kKingAttackDistance + kKnight - 1,
        magic::GetSquareDistance(piece_square, ec.king_squares[not_color]));
//      int relative_x = GetSquareX(piece_square) - GetSquareX(enemy_king_square) + 7;
//      int relative_y = GetSquareY(piece_square) - GetSquareY(enemy_king_square) + 7;
//      AddFeature<T>(score, color, kKnightVsKingPosition + relative_king_map[relative_x + 15 * relative_y], 1);
    BitBoard attack_map = magic::GetAttackMap<kKnight>(piece_square, ec.all_pieces);
    counter.unsafe += bitops::PopCount(attack_map
                                             & ec.checks.unsafe[color][kKnight-kKnight]);
    counter.safe += bitops::PopCount(attack_map
                                           & ec.checks.safe[color][kKnight-kKnight]);
    AddFeature<T>(score, offset + kKnightSquaresIndex +
                  bitops::PopCount(attack_map & ~(ec.c_pieces[color] | ec.covered_once[not_color])), 1);
    AddFeature<T>(score, offset + kMinorAttackIndex,
        bitops::PopCount(attack_map & ec.major_pieces[not_color]));
    if (attack_map & enemy_king_zone) {
      counter.king_attack_count++;
      counter.king_zone_attacks[kKnight - kKnight]++;//= bitops::PopCount(attack_map & enemy_king_zone);
    }
  }

  AddFeature<T>(score, offset + kKnightOutpost,
      bitops::PopCount(board.get_piece_bitboard(color, kKnight)
          & ec.covered_once[color]
          & (~ec.covered_potentially[not_color])));
}

template<typename T, Color color, Color our_color>
inline void ScoreBishops(T &score, const Board &board, const EvalConstants &ec,
                        EvalCounter &counter, const BitBoard enemy_king_zone) {
  constexpr Color not_color = color ^ 0x1;
  constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  const Square enemy_king_square = ec.king_squares[not_color];
  BitBoard bishop_targets = 0;
  BitBoard abstract_targets = 0;
  for (BitBoard pieces = board.get_piece_bitboard(color, kBishop); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
//      AddFeature<T>(score, color, kKingAttackDistance + kBishop - 1,
//          magic::GetSquareDistance(piece_square, king_squares[not_color]));
    int relative_x = GetSquareX(piece_square) - GetSquareX(enemy_king_square) + 7;
    int relative_y = GetSquareY(piece_square) - GetSquareY(enemy_king_square) + 7;
    AddFeature<T>(score, offset + kBishopVsKingPosition + relative_king_map[relative_x + 15 * relative_y], 1);
    BitBoard attack_map = magic::GetAttackMap<kBishop>(piece_square, ec.all_pieces)
        & (~ec.covered_once[not_color] | (ec.c_pieces[not_color] ^ ec.pawn_bb[not_color]));
    counter.unsafe += bitops::PopCount(attack_map
                                           & ec.checks.unsafe[color][kBishop-kKnight]);
    counter.safe += bitops::PopCount(attack_map
                                           & ec.checks.safe[color][kBishop-kKnight]);
    bishop_targets |= attack_map;
    if (attack_map & enemy_king_zone) {
      counter.king_attack_count++;
      counter.king_zone_attacks[kBishop - kKnight]++;
    }
    AddFeature<T>(score, offset + kBishopMobility
                  + bitops::PopCount(attack_map & ~ec.c_pieces[color]), 1);
    abstract_targets |= magic::GetAttackMap<kBishop>(piece_square, ec.hard_block[color]);
  }
  bishop_targets &= ~ec.c_pieces[color];
  AddFeature<T>(score, offset + kMinorAttackIndex,
      bitops::PopCount(bishop_targets & ec.major_pieces[not_color]));
  abstract_targets &= ~bishop_targets;
  AddFeature<T>(score, offset + kBishop + kAbstractActivityIndex,
      bitops::PopCount(abstract_targets));
}

template<typename T, Color color, Color our_color>
inline void ScoreRooks(T &score, const Board &board, const EvalConstants &ec,
                       EvalCounter &counter, const BitBoard enemy_king_zone) {
  constexpr Color not_color = color ^ 0x1;
  constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  for (BitBoard pieces = board.get_piece_bitboard(color, kRook); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);

    BitBoard file = magic::GetSquareFile(piece_square);
    if (!(file & ec.all_pawns_bb)) {
      AddFeature<T>(score, offset + kRookOpenFile, 1);
    }

    AddFeature<T>(score, offset + kKingAttackDistance + kRook - 1,
        magic::GetSquareDistance(piece_square, ec.king_squares[not_color]));
    BitBoard attack_map = ~ec.c_pieces[color]
        & (~ec.covered_once[not_color] | (ec.c_pieces[not_color] ^ ec.pawn_bb[not_color]))
        & magic::GetAttackMap<kRook>(piece_square, ec.all_pieces);
    counter.unsafe += bitops::PopCount(attack_map
                                           & ec.checks.unsafe[color][kRook-kKnight]);
    counter.safe += bitops::PopCount(attack_map
                                           & ec.checks.safe[color][kRook-kKnight]);
    if (attack_map & enemy_king_zone) {
      counter.king_attack_count++;
      counter.king_zone_attacks[kRook - kKnight]++;
    }
    BitBoard abstract_attack = attack_map ^
        magic::GetAttackMap<kRook>(piece_square, ec.hard_block[color]);
    AddFeature<T>(score, offset + kRookMobility + bitops::PopCount(attack_map), 1);
    AddFeature<T>(score, offset + kRook + kAbstractActivityIndex,
        bitops::PopCount(abstract_attack));
  }
}

template<typename T, Color color, Color our_color>
inline void ScoreQueens(T &score, const Board &board, const EvalConstants &ec,
                        EvalCounter &counter, const BitBoard enemy_king_zone) {
  constexpr Color not_color = color ^ 0x1;
  constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  for (BitBoard pieces = board.get_piece_bitboard(color, kQueen); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    AddFeature<T>(score, offset + kKingAttackDistance + kQueen - 1,
        magic::GetSquareDistance(piece_square, ec.king_squares[not_color]));
    BitBoard attack_map = ~ec.c_pieces[color]
        & (~ec.covered_once[not_color] | (ec.c_pieces[not_color] ^ ec.pawn_bb[not_color]))
        & magic::GetAttackMap<kQueen>(piece_square, ec.all_pieces);
    counter.unsafe += bitops::PopCount(attack_map
                                           & ec.checks.unsafe[color][kQueen-kKnight]);
    counter.safe += bitops::PopCount(attack_map
                                           & ec.checks.safe[color][kQueen-kKnight]);
    if (attack_map & enemy_king_zone) {
      counter.king_attack_count++;
      counter.king_zone_attacks[kQueen - kKnight]++;
    }
    AddFeature<T>(score, offset + kQueenMobility + std::min(bitops::PopCount(attack_map), 24), 1);
    if (kUseQueenActivity) {
      BitBoard abstract_attack = attack_map ^
          magic::GetAttackMap<kQueen>(piece_square, ec.hard_block[color]);
      AddFeature<T>(score, offset + kQueen + kActivityBonusIndex,
                    bitops::PopCount(attack_map));
      AddFeature<T>(score, offset + kQueen + kAbstractActivityIndex,
                    bitops::PopCount(abstract_attack));
    }
  }
}

template<typename T, Color color, Color our_color>
inline void ScoreKings(T &score, const Board &board,
                       const EvalConstants &ec, const EvalCounter &counter) {
  constexpr Color not_color = color ^ 0x1;
  constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  Square king_square = bitops::NumberOfTrailingZeros(board.get_piece_bitboard(color,kKing));

  if (board.get_piece_bitboard(not_color, kQueen)) {
    AddFeature<T>(score, offset + kKingVectorExposure,
                  bitops::PopCount(magic::GetAttackMap<kBishop>(king_square,
                                       board.get_piece_bitboard(color, kPawn))));
    AddFeature<T>(score, offset + kKingVectorExposure + 1,
                  bitops::PopCount(magic::GetAttackMap<kRook>(king_square,
                                       board.get_piece_bitboard(color, kPawn))));
  }
  else {
    if (board.get_piece_bitboard(not_color, kBishop)) {
      AddFeature<T>(score, offset + kKingVectorExposure,
                    bitops::PopCount(magic::GetAttackMap<kBishop>(king_square,
                                         board.get_piece_bitboard(color, kPawn))));
    }
    if (board.get_piece_bitboard(not_color, kRook)) {
      AddFeature<T>(score, offset + kKingVectorExposure + 1,
                    bitops::PopCount(magic::GetAttackMap<kRook>(king_square,
                                         board.get_piece_bitboard(color, kPawn))));
    }
  }

  if (counter.king_attack_count >= 2) {
    int king_attack_scaling = kKingAttackerScaling[std::min(counter.king_attack_count, 6)];
    for (PieceType pt = kKnight - kKnight; pt <= kQueen - kKnight; pt++) {
      AddFeature<T>(score, offset + kKingAttack
                                  + pt, king_attack_scaling * counter.king_zone_attacks[pt]);
    }
  }
}

template<typename T, Color our_color>
inline void AddKingsToInput(T &score, const Board &board, CNNHelper &helper) {
  constexpr Color opp_color = our_color ^ 0x1;

  Square our_king_square = bitops::NumberOfTrailingZeros(board.get_piece_bitboard(our_color,kKing));
  helper.our_k = our_king_square;
  Square opp_king_square = bitops::NumberOfTrailingZeros(board.get_piece_bitboard(opp_color,kKing));

  if (our_color == kBlack) {
    our_king_square = GetMirroredSquare(our_king_square);
    opp_king_square = GetMirroredSquare(opp_king_square);
  }

  helper.our_k = our_king_square;
  helper.opp_k = opp_king_square;

  AddRawBoardFeature<T>(score, kChanKingsIdx, our_king_square);
  AddRawBoardFeature<T>(score, kChanKingsIdx + kChannelsPerSide, opp_king_square);
}

template<typename T, Color our_color>
inline void AddPawnCounts(T &score, const Board &board) {
  constexpr Color opponent_color = our_color ^ 0x1;
  constexpr size_t offset = kSideDependentFeatureCount;

  AddFeature<T>(score, kPawnCountIdx + board.get_piece_count(our_color, kPawn), 1);
  AddFeature<T>(score, kPawnCountIdx + offset + board.get_piece_count(opponent_color, kPawn), 1);
}

template<typename T>
inline T GetSuperStaticRawFeatures(const Board &board, const EvalConstants &ec, CNNHelper &helper) {
  T score = init_cnn_in<T>();
  if (board.get_turn() == kWhite) {
    AddPawnsToInput<T, kWhite, kWhite>(score, ec, helper);
    AddPawnsToInput<T, kBlack, kWhite>(score, ec, helper);
    AddKingsToInput<T, kWhite>(score, board, helper);
  }
  else {
    AddPawnsToInput<T, kBlack, kBlack>(score, ec, helper);
    AddPawnsToInput<T, kWhite, kBlack>(score, ec, helper);
    AddKingsToInput<T, kBlack>(score, board, helper);
  }
  return score;
}

template<typename T, Color color, Color our_color>
inline void ScorePieces(T &score, const Board &board, const EvalConstants &ec,
                        EvalCounter &counter) {
  const BitBoard enemy_king_zone = magic::GetKingArea(ec.king_squares[color ^ 0x1]);

  ScorePawnThreats<T, color, our_color>(score, ec);
  // ScorePawns<T, color, our_color>(score, ec);
  ScoreKnights<T, color, our_color>(score, board, ec, counter, enemy_king_zone);
  ScoreBishops<T, color, our_color>(score, board, ec, counter, enemy_king_zone);
  ScoreRooks<T, color, our_color>(score, board, ec, counter, enemy_king_zone);
  ScoreQueens<T, color, our_color>(score, board, ec, counter, enemy_king_zone);
  ScoreKings<T, color, our_color>(score, board, ec, counter);
}

template<typename T, Color color, Color our_color>
inline void AddPieceCountFeatures(T &score, const Board &board) {
  constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  AddFeature<T>(score, kKnightCountIdx + offset + std::min(board.get_piece_count(color, kKnight), 2), 1);
  AddFeature<T>(score, kBishopCountIdx + offset + std::min(board.get_piece_count(color, kBishop), 2), 1);
  AddFeature<T>(score, kRookCountIdx + offset + std::min(board.get_piece_count(color, kRook), 2), 1);
}

template<typename T, Color our_color>
inline void AddCommonFeatures(T &score, const Board &board) {

  AddPieceCountFeatures<T, kWhite, our_color>(score, board);
  AddPieceCountFeatures<T, kBlack, our_color>(score, board);

  AddFeaturePair<T, our_color>(score, kQueenCountIdx,
                               board.get_piece_count(kWhite, kQueen),
                               board.get_piece_count(kBlack, kQueen));

  if (board.get_piece_count(kWhite, kBishop) == 1 && board.get_piece_count(kBlack, kBishop) == 1
      && bitops::PopCount(board.get_piecetype_bitboard(kBishop) & bitops::light_squares) == 1) {
    AddFeature<T>(score, kOppositeColoredBishops, 1);
  }
}

template<typename T, Color our_color>
inline void AddCommonFeatures(T &score, const Board &board, const EvalConstants &ec) {
  AddFeaturePair<T, our_color>(score, kPawn + kActivityBonusIndex,
                               bitops::PopCount(ec.p_forward[kWhite]),
                               bitops::PopCount(ec.p_forward[kBlack]));

  // TODO remove double pawn penalty as it should be part of CNN
  AddFeaturePair<T, our_color>(score, kDoublePawnPenaltyIndex,
                               bitops::PopCount(bitops::N(ec.p_fill_forward[kWhite]) & ec.pawn_bb[kWhite]),
                               bitops::PopCount(bitops::S(ec.p_fill_forward[kBlack]) & ec.pawn_bb[kBlack]));

  AddFeaturePair<T, our_color>(score, kUnprotectedPieces,
                    bitops::PopCount(ec.nbr_bitboard & ec.c_pieces[kWhite] & (~ec.controlled[kWhite])),
                    bitops::PopCount(ec.nbr_bitboard & ec.c_pieces[kBlack] & (~ec.controlled[kBlack])));
}

}

namespace net_evaluation {

void SetPHashSize(const size_t bytes) {
  pawn_hash::size = bytes / 72;
  pawn_hash::table.resize(pawn_hash::size);
}

template<typename T, Color our_color>
T ScoreBoard(const Board &board, const EvalConstants &ec) {
  T score = init<T>();

  // Initialize general constants
//  const EvalConstants ec(board);

  // Pawn evaluations
  AddPawnCounts<T, our_color>(score, board);
//  AddSuperStaticFeatures<T>(score, board, ec);

  // Common features independent of specific individual piece placement.
  AddCommonFeatures<T, our_color>(score, board);
  AddCommonFeatures<T, our_color>(score, board, ec);

  // Piece evaluations
  std::array<EvalCounter, 2> check_counter;
  ScorePieces<T, kWhite, our_color>(score, board, ec, check_counter[kWhite]);
  ScorePieces<T, kBlack, our_color>(score, board, ec, check_counter[kBlack]);

  // Features picked up while iterating over pieces
  AddFeaturePair<T, our_color>(score, kSafeChecks, check_counter[kWhite].safe, check_counter[kBlack].safe);
  AddFeaturePair<T, our_color>(score, kUnSafeChecks, check_counter[kWhite].unsafe, check_counter[kBlack].unsafe);

  return score;
}

NetLayerType FiltersForward(const CNNLayerType &prev_cnn_layer, const Array3d<FNetLayerType, 3, 3, 16> &filters,
                            const NetLayerType &bias, Square s) {
  int h = GetSquareY(s);
  int w = GetSquareX(s);
  FNetLayerType result(bias);
  for (size_t i = 0; i < 3; ++i) {
    if (h+i == 0 || h+i > 8) {
      continue;
    }
    for (size_t j = 0; j < 3; ++j) {
      if (w+j == 0 || w+j > 8) {
        continue;
      }
      for (size_t c = 0; c < prev_cnn_layer[0][0].size(); ++c) {
        //result += prev_cnn_layer[h+i-1][w+j-1][c] * filters[i][j][c];
        result.FMA(filters[i][j][c], prev_cnn_layer[h+i-1][w+j-1][c]);
      }
    }
  }
  result.relu();
  return result.to_simple_vec();
}

NetLayerType NetForward(CNNLayerType &cnn_layer_one, const CNNHelper &helper) {
  // ReLU 1
  for (size_t h = 0; h < kBoardLength; ++h) {
    for (size_t w = 0; w < kBoardLength; ++w) {
      cnn_layer_one[h][w].relu();
    }
  }

  NetLayerType our_p_out(0);
  for (size_t i = 0; i < helper.our_p.size(); ++i) {
    our_p_out += FiltersForward(cnn_layer_one, cnn_our_p_filters, cnn_our_p_bias, helper.our_p[i]);
  }
  NetLayerType our_k_out = FiltersForward(cnn_layer_one, cnn_our_k_filters, cnn_our_k_bias, helper.our_k);

  NetLayerType opp_p_out(0);
  for (size_t i = 0; i < helper.opp_p.size(); ++i) {
    opp_p_out += FiltersForward(cnn_layer_one, cnn_opp_p_filters, cnn_opp_p_bias, helper.opp_p[i]);
  }
  NetLayerType opp_k_out = FiltersForward(cnn_layer_one, cnn_opp_k_filters, cnn_opp_k_bias, helper.opp_k);

  // Dense
  NetLayerType out(0);
  size_t i;
  for (i = 0; i < our_p_out.size(); ++i) {
    out += our_p_out[i] * cnn_dense_out[i];
    out += our_k_out[i] * cnn_dense_out[i + our_p_out.size()];
    out += opp_p_out[i] * cnn_dense_out[i + 2 * our_p_out.size()];
    out += opp_k_out[i] * cnn_dense_out[i + 3 * our_p_out.size()];
  }

  return out;
}

Score NetForward(NetLayerType &layer_one, float c = 0.5) {
  layer_one += bias_layer_one;
  layer_one.relu();

  NetLayerType layer_two = bias_layer_two;
  for (size_t i = 0; i < layer_one.size(); ++i) {
    layer_two.FMA(second_layer_weights[i], layer_one[i]);
  }
  layer_two.relu();

  float win = layer_two.dot(win_weights) + win_bias;
  float win_draw = layer_two.dot(win_draw_weights) + win_draw_bias;

  //float wpct = sigmoid(win) * c + sigmoid(win_draw) * (1 - c);

  //return wpct_to_score(wpct);
  return WDLScore::from_pct(sigmoid(win), sigmoid(win_draw));
}

Score ScoreBoard(const Board &board) {
  const EvalConstants ec(board);
  HashType p_hash = board.get_pawn_hash();
  pawn_hash::PawnEntry entry = pawn_hash::GetEntry(p_hash);
  NetLayerType cnn_out;
  if (pawn_hash::ValidateHash(entry, p_hash)) {
    cnn_out = entry.output;
  }
  else {
    CNNHelper helper;
    CNNLayerType cnn_input = GetSuperStaticRawFeatures<CNNLayerType>(board, ec, helper);
    cnn_out = NetForward(cnn_input, helper);
    pawn_hash::SaveEntry(cnn_out, p_hash);
  }

  NetLayerType layer_one = init<NetLayerType>();
  if (board.get_turn() == kWhite) {
    layer_one = ScoreBoard<NetLayerType, kWhite>(board, ec);
  }
  else {
    layer_one = ScoreBoard<NetLayerType, kBlack>(board, ec);
  }
  layer_one += cnn_out;
  return NetForward(layer_one, contempt[board.get_turn()]);
}

template<size_t size>
void init_cnn_weights(Array3d<FNetLayerType, 3, 3, 16> &cnn_filters, const std::array<float, size> &weights,
                      double multiplier = 1.0) {
  size_t idx = 0;
  for (size_t h = 0; h < cnn_filters.size(); ++h) {
    for (size_t w = 0; w < cnn_filters[0].size(); ++w) {
      for (size_t i = 0; i < cnn_filters[0][0].size(); ++i) {
        NetLayerType tmp(0);
        for (size_t j = 0; j < cnn_filters[0][0][0].size(); ++j) {
          tmp[j] = weights[idx++] * multiplier;
          // cnn_filters[h][w][i][j] = weights[idx++] * multiplier;
        }
        cnn_filters[h][w][i] = FNetLayerType(tmp);
      }
    }
  }
  assert(idx == size);
}

template<size_t size>
void init_cnn_bias(NetLayerType &cnn_bias, const std::array<float, size> &bias_weights,
                      double multiplier = 1.0) {
  for (size_t i = 0; i < net_hardcode::cnn_l1_bias.size() && i < cnn_bias.size(); ++i) {
    cnn_bias[i] = bias_weights[i] * multiplier;
  }
}

void init_weights() {
  // Init cnn net weights
  Array3d<NetLayerType, 5, 5, 3> const_channel_filters;

  size_t idx = 0;
  for (size_t h = 0; h < cnn_l1_filters.size(); ++h) {
    for (size_t w = 0; w < cnn_l1_filters[0].size(); ++w) {
      for (size_t i = 0; i < cnn_l1_filters[0][0].size(); ++i) {
        for (size_t j = 0; j < cnn_l1_filters[0][0][0].size(); ++j) {
          cnn_l1_filters[h][w][i][j] = net_hardcode::cnn_l1_weights[idx++];
        }
      }
      for (size_t i = 0; i < const_channel_filters[0][0].size(); ++i) {
        for (size_t j = 0; j < const_channel_filters[0][0][0].size(); ++j) {
          const_channel_filters[h][w][i][j] = net_hardcode::cnn_l1_weights[idx++];
        }
      }
    }
  }

  NetLayerType cnn_input_bias_b(0);
  for (size_t i = 0; i < net_hardcode::cnn_l1_bias.size() && i < cnn_input_bias_b.size(); ++i) {
    cnn_input_bias_b[i] = net_hardcode::cnn_l1_bias[i];
  }

  Array3d<float, 8, 8, 3> const_channel_inputs;
  for (size_t i = 0; i < 8; ++i) {
    for (size_t j = 0; j < 8; ++j) {
      cnn_input_bias[i][j] = cnn_input_bias_b;
      const_channel_inputs[i][j][0] = 1;
      const_channel_inputs[i][j][1] = i / 7.0;
      const_channel_inputs[i][j][2] = j / 3.0;
      if (j >= 4) {
        const_channel_inputs[i][j][2] = (7-j) / 3.0;
      }
    }
  }

  for (size_t h = 0; h < cnn_input_bias.size(); ++h) {
    for (size_t w = 0; w < cnn_input_bias[0].size(); ++w) {
      for (size_t i = 0; i < 5; ++i) {
        if (h+i <= 1 || h+i >= 10) {
          continue;
        }
        for (size_t j = 0; j < 5; ++j) {
          if (w+j <= 1 || w+j >= 10) {
            continue;
          }
          for (size_t c = 0; c < const_channel_filters[i][j].size(); ++c) {
            cnn_input_bias[h][w] += const_channel_inputs[h+i-2][w+j-2][c]
                                            * const_channel_filters[i][j][c];
          }
        }
      }
    }
  }

  constexpr size_t cnn_weights_size = net_hardcode::cnn_our_k_weights.size();
  init_cnn_weights<cnn_weights_size>(cnn_our_p_filters, net_hardcode::cnn_our_p_weights, 1.0 / 8);
  init_cnn_weights<cnn_weights_size>(cnn_our_k_filters, net_hardcode::cnn_our_k_weights);
  init_cnn_weights<cnn_weights_size>(cnn_opp_p_filters, net_hardcode::cnn_opp_p_weights, 1.0 / 8);
  init_cnn_weights<cnn_weights_size>(cnn_opp_k_filters, net_hardcode::cnn_opp_k_weights);

  constexpr size_t cnn_bias_size = net_hardcode::cnn_our_k_bias.size();
  init_cnn_bias<cnn_bias_size>(cnn_our_p_bias, net_hardcode::cnn_our_p_bias, 1.0 / 8);
  init_cnn_bias<cnn_bias_size>(cnn_our_k_bias, net_hardcode::cnn_our_k_bias);
  init_cnn_bias<cnn_bias_size>(cnn_opp_p_bias, net_hardcode::cnn_opp_p_bias, 1.0 / 8);
  init_cnn_bias<cnn_bias_size>(cnn_opp_k_bias, net_hardcode::cnn_opp_k_bias);

  for (size_t i = 0; i < cnn_dense_in; ++i) {
    size_t offset = (net_hardcode::l1_weights.size() / block_size) - cnn_dense_in;
    size_t j = (offset + i) * block_size;
    for (size_t k = 0; k < block_size; ++k) {
      assert(j + k < net_hardcode::l1_weights.size());
      cnn_dense_out[i][k] = net_hardcode::l1_weights[j+k];
    }
  }

  // Init regular net weights
  net_input_weights = std::vector<NetLayerType>(net_hardcode::l1_weights.size() / block_size);
  for (size_t i = 0; i < (net_hardcode::l1_weights.size() / block_size) - block_size; ++i) {
    size_t j = i * block_size;
    for (size_t k = 0; k < block_size; ++k) {
      net_input_weights[i][k] = net_hardcode::l1_weights[j+k];
    }
  }

  second_layer_weights = std::vector<NetLayerType>(net_hardcode::l2_weights.size() / block_size);
  for (size_t i = 0; i < net_hardcode::l2_weights.size() / block_size; ++i) {
    size_t j = i * block_size;
    for (size_t k = 0; k < block_size; ++k) {
      second_layer_weights[i][k] = net_hardcode::l2_weights[j+k];
    }
  }

  for (size_t k = 0; k < block_size; ++k) {
    win_weights[k]  = net_hardcode::output_weights[k + 0 * block_size];
    win_draw_weights[k] = net_hardcode::output_weights[k + 1 * block_size];

    bias_layer_one[k] = net_hardcode::l1_bias[k];
    bias_layer_two[k] = net_hardcode::l2_bias[k];
  }

  const size_t out_block_size = win_weights.size();
  for (size_t k = 0; k < out_block_size; ++k) {
    win_weights[k]  = net_hardcode::output_weights[k + 0 * out_block_size];
    win_draw_weights[k] = net_hardcode::output_weights[k + 1 * out_block_size];
  }

  win_bias = net_hardcode::bias_win;
  win_draw_bias = net_hardcode::bias_win_draw;
}

std::vector<int32_t> GetCNNInputs(const Board &board) {
  const EvalConstants ec(board);
  CNNHelper helper;
  return GetSuperStaticRawFeatures<std::vector<int32_t> >(board, ec, helper);
}

std::vector<int32_t> GetNetInputs(const Board &board) {
  const EvalConstants ec(board);
  if (board.get_turn() == kWhite) {
    return ScoreBoard<std::vector<int32_t>, kWhite>(board, ec);
  }
  return ScoreBoard<std::vector<int32_t>, kBlack>(board, ec);
}

#ifdef EVAL_TRAINING
void AddHeader(std::ofstream &file, std::string feature_name_prefix, int feature_count) {
  file << feature_name_prefix << "0";
  for (int i = 1; i < feature_count; ++i) {
    file << "," << feature_name_prefix << i;
  }
  file << std::endl;
}

void AddGame(const Game &game, std::ofstream &res_file, std::ofstream &dynamic_features_file,
             std::ofstream &static_features_file) {
  WDLScore result = game.result;
  if (game.board.get_turn() == kBlack) {
    result = -result;
  }
  res_file << result.get_win_probability() << ","
      << result.get_win_draw_probability() << std::endl;

  std::vector<int> dynamic_features = GetNetInputs(game.board);
  dynamic_features_file << dynamic_features[0];
  for (int i = 1; i < dynamic_features.size(); ++i) {
    dynamic_features_file << "," << dynamic_features[i];
  }
  dynamic_features_file << std::endl;

  std::vector<int> static_features = GetCNNInputs(game.board);
  static_features_file << static_features[0];
  for (int i = 1; i < static_features.size(); ++i) {
    static_features_file << "," << static_features[i];
  }
  static_features_file << std::endl;
}

void StoreEvalDataset(const std::vector<Game> &games, std::string out_file_name) {
  std::ofstream res_file(out_file_name + ".res.csv");
  AddHeader(res_file, "result_feature_", 2);
  std::ofstream dynamic_features_file(out_file_name + ".dynamic.csv");
  AddHeader(dynamic_features_file, "dy_fe", kTotalNumFeatures);
  std::ofstream static_features_file(out_file_name + ".static.csv");
  AddHeader(static_features_file, "st_fe", kBoardSize * kNumChannels);

  Game game_start;
  game_start.board.SetStartBoard();
  game_start.result = WDLScore::from_pct(0.0, 1.0);
  AddGame(game_start, res_file, dynamic_features_file, static_features_file);

  size_t samples = 1;
  for (Game game : games) {
    AddGame(game, res_file, dynamic_features_file, static_features_file);
    samples++;
    if (samples % 10000 == 0) {
      std::cout << "Processed " << samples << " samples!" << std::endl;
    }
  }

  res_file.flush();
  res_file.close();
  dynamic_features_file.flush();
  dynamic_features_file.close();
  static_features_file.flush();
  static_features_file.close();
}


void StoreEvalDatasetOld(const std::vector<Game> &games, std::string out_file_name) {
  std::ofstream dfile(out_file_name);
  size_t samples = 0;
  if (true) {
    Board board;
    std::vector<int> features = GetNetInputs(board);
    if (samples == 0) {
      dfile << "win,windraw";
      for (size_t i = 0; i < features.size(); i++) {
        dfile << ",fe" << i;
      }
      dfile << std::endl;
    }
    dfile << "0, 1";
    for (int feature : features) {
      dfile << ", " << feature;
    }
    dfile << std::endl;
    samples++;
  }

  for (Game game : games) {
    WDLScore result = game.result;
    if (game.board.get_turn() == kBlack) {
      result = -result;
    }
    std::vector<int> features = GetNetInputs(game.board);
    if (samples == 0) {
      dfile << "win,windraw";
      for (size_t i = 0; i < features.size(); i++) {
        dfile << ",fe" << i;
      }
      dfile << std::endl;
    }
    dfile << result.get_win_probability() << ", " << result.get_win_draw_probability();
    for (int feature : features) {
      dfile << ", " << feature;
    }
    dfile << std::endl;
    samples++;
    if (samples % 10000 == 0) {
      std::cout << "Processed " << samples << " samples!" << std::endl;
    }
  }
  dfile.flush();
  dfile.close();
}

void RerollCommonFeatureGames(std::vector<Game> &games, size_t reroll_pct) {
  reroll_pct = reroll_pct % 100;
  if (reroll_pct > 0) {
    std::vector<double> feature_counts(kTotalNumFeatures, 1);
    for (const Game &game : games) {
      const auto features = GetNetInputs(game.board);
      assert(features.size() == feature_counts.size());
      for (size_t idx = 0; idx < features.size(); ++idx) {
        feature_counts[idx] += (features[idx] != 0);
      }
    }
    std::cout << "Got feature counts" << std::endl;
    std::vector<double> feature_values(kTotalNumFeatures, 0);
    for (size_t idx = 0; idx < feature_counts.size(); ++idx) {
      feature_values[idx] = 1.0 / (feature_counts[idx] / games.size());
    }
    std::cout << "Estimated feature values" << std::endl;
    std::vector<double> game_value_estimate(games.size(), 0);
    for (size_t game_idx = 0; game_idx < games.size(); ++game_idx) {
      const auto features = GetNetInputs(games[game_idx].board);
      for (size_t idx = 0; idx < features.size(); ++idx) {
        game_value_estimate[game_idx] += (features[idx] != 0) * feature_values[idx];
      }
    }
    std::cout << "Estimated game values" << std::endl;
    std::vector<double> values(game_value_estimate.size(), 0);
    for (size_t idx = 0; idx < values.size(); ++idx) {
      values[idx] = game_value_estimate[idx];
    }
    std::cout << "Copied games" << std::endl;
    std::cout << "Values size: " << values.size() << std::endl;
    std::sort(values.begin(), values.end(), std::less<double>());
    std::cout << "Sorted game values" << std::endl;
    double threshold = values[(reroll_pct * games.size()) / 100];
    std::cout << "Threshold: " << threshold << std::endl;
    size_t reroll_count = 0;
    for (size_t game_idx = 0; game_idx < games.size(); ++game_idx) {
      if (game_value_estimate[game_idx] < threshold) {
        data::SetGameToRandomQuiescent(games[game_idx]);
        reroll_count++;
        if (reroll_count % 10000 == 0) {
          std::cout << "rerolled " << reroll_count << " positions" << std::endl;
        }
      }
    }
  }
}


std::vector<Game> LoadTrainingGamesForTraining(std::string filename, size_t reroll_pct) {
  std::vector<Game> games = data::LoadGames(30000000, filename);
  data::SetGamesToRandomQuiescent(games);
  for (int i = 0; i < 5; i++) {
//    RerollCommonFeatureGames(games, reroll_pct);
  }
  return games;
}

void AddArasanEPDs(std::vector<Game> &games) {
  std::string line;
  std::ifstream file("lichess-new-labeled.epd");

  while(std::getline(file, line)) {
    std::vector<std::string> tokens = parse::split(line, ' ');
    std::string fen = tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + tokens[3];
    Board board(fen);
    if (board.InCheck()) {
      continue;
    }
    Game game;
    game.board = board;
    if (tokens[8].compare("\"1.000\";") == 0) {
      game.result = WDLScore::from_pct(1.0, 1.0);
    }
    else if (tokens[8].compare("\"0.000\";") == 0) {
      game.result = WDLScore::from_pct(0.0, 0.0);
    }
    else if (tokens[8].compare("\"0.500\";") == 0) {
      game.result = WDLScore::from_pct(0.0, 1.0);
    }
    else {
      std::cout << "Error detected! Line:" << std::endl;
      std::cout << line << std::endl;
      file.close();
      return;
    }
    games.emplace_back(game);
    if (games.size() % 10000 == 0) {
      std::cout << "Loaded " << games.size() << " samples!" << std::endl;
    }
  }
  file.close();
}

void AddZCEPDs(std::vector<Game> &games) {
  std::string line;
  std::ifstream file("quiet-labeled.epd");

  while(std::getline(file, line)) {
    std::vector<std::string> tokens = parse::split(line, ' ');
    std::string fen = tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + tokens[3];
    Board board(fen);
    if (board.InCheck()) {
      continue;
    }
    Game game;
    game.board = board;
    if (tokens[5].compare("\"1-0\";") == 0) {
      game.result = WDLScore::from_pct(1.0, 1.0);
    }
    else if (tokens[5].compare("\"0-1\";") == 0) {
      game.result = WDLScore::from_pct(0.0, 0.0);
    }
    else if (tokens[5].compare("\"1/2-1/2\";") == 0) {
      game.result = WDLScore::from_pct(0.0, 1.0);
    }
    else {
      std::cout << "Error detected! Line:" << std::endl;
      std::cout << line << std::endl;
      file.close();
      return;
    }
    games.emplace_back(game);
    if (games.size() % 10000 == 0) {
      std::cout << "Loaded " << games.size() << " samples!" << std::endl;
    }
  }
  file.close();
}

void GenerateDatasetFromEPD() {

  std::vector<Game> games = LoadTrainingGamesForTraining("Games.ucig", 3000000);
  std::cout << "Added positions from Games.ucig: " << games.size() << std::endl;
  AddZCEPDs(games);
  std::cout << "Added Zurichess EPDs. Position count: " << games.size() << std::endl;
  AddArasanEPDs(games);
  std::cout << "Added Lichess EPDs from Arasan. Position count: " << games.size() << std::endl;

  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(games.begin(), games.end(), g);

  StoreEvalDataset(games, "eval_dataset");
}

void GenerateDatasetFromUCIGames(std::string filename, std::string out_name, size_t reroll_pct) {
  std::vector<Game> games = LoadTrainingGamesForTraining(filename, reroll_pct);
  StoreEvalDataset(games, out_name);
}

void EstimateFeatureImpact() {
  std::cout << "Currently EstimateFeatureImpact() is disabled!" << std::endl;
//  std::string line;
//  std::ifstream file("quiet-labeled.epd");
//  std::vector<int64_t> feature_sums(kTotalNumFeatures, 0);
//  std::vector<int64_t> score_difference_sums(kTotalNumFeatures, 0);
//  size_t count = 0;
//  std::vector<int64_t> f_counts(kTotalNumFeatures, 0);
//  while(std::getline(file, line)) {
//    std::vector<std::string> tokens = parse::split(line, ' ');
//    std::string fen = tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + tokens[3];
//    Board board(fen);
//    const EvalConstants ec(board);
//    NetLayerType layer_one;
//    if (board.get_turn() == kWhite) {
//      layer_one = ScoreBoard<NetLayerType, kWhite>(board, ec);
//    }
//    else {
//      layer_one = ScoreBoard<NetLayerType, kBlack>(board, ec);
//    }
//    const Score eval = NetForward(layer_one);
//    std::vector<int32_t> features = GetNetInputs(board);
//    for (size_t i = 0; i < kTotalNumFeatures; ++i) {
//      if (!features[i]) {
//        continue;
//      }
//      feature_sums[i] += features[i];
//      f_counts[i] += features[i] > 0;
//      if (i >= kSideDependentFeatureCount) {
//        continue;
//      }
//      NetLayerType modified_input(0);
//      for (size_t j = 0; j < modified_input.size(); ++j) {
//        modified_input[j] = layer_one[j] - features[i] * net_input_weights[i][j];
//        modified_input[j] -= features[i+kSideDependentFeatureCount] * net_input_weights[i+kSideDependentFeatureCount][j];
//      }
//      score_difference_sums[i] += std::abs(eval - NetForward(modified_input));
//    }
//    count++;
//    if (count % 10000 == 0) {
//      std::cout << "Loaded " << count << " samples!" << std::endl;
//    }
//  }
//  file.close();
//
//  for (size_t i = 0; i < kTotalNumFeatures; ++i) {
//    float feature_weight_sums = 0;
//    for (size_t j = 0; j < net_input_weights[i].size(); ++j) {
//      feature_weight_sums += std::abs(net_input_weights[i][j]);
//    }
//    std::cout << "Feature: " << i << " weight: " << feature_weight_sums << " count: " << f_counts[i]
//              << " weighted count: " << (feature_sums[i]*feature_weight_sums / f_counts[i]);
//    if (i < kSideDependentFeatureCount) {
//      std::cout << " weighted score diff: " << ((1.0 * score_difference_sums[i]) / f_counts[i]);
//    }
//    std::cout << std::endl;
//  }
}
#endif

}

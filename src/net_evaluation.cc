#include "data.h"
#include "net_evaluation.h"
#include "general/magic.h"
#include "general/types.h"
#include "incbin/incbin.h"

#include <random>
#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

INCBIN(float_t, NetWeights, "f128rS06_ep7.bin"); // o
//INCBIN(float_t, NetWeights, "f128rS01_ep4.bin"); // n
//INCBIN(float_t, NetWeights, "f128r3_ep2.bin");
//INCBIN(float_t, NetWeights, "f128r4_ep2.bin");
// f128r3_ep2 -> 128i
// f128r4_ep2 -> 128j


// NN types
constexpr size_t block_size = 128;
using NetLayerType = Vec<float, block_size>;
using FNetLayerType = Vec<SIMDFloat, block_size>;

constexpr bool use_pk_net = false;
constexpr size_t pk_l2_layer_size = use_pk_net ? block_size : 0;

std::array<int32_t, 2> contempt = { 0, 0 };

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

//inline float sigmoid(float x) {
//  return 1 / (1 + std::exp(-x));
//}

// PK weights

NetLayerType pk_layer_one_bias(0);
std::vector<NetLayerType> pk_layer_two_weights(pk_l2_layer_size, 0);

//Array3d<FNetLayerType, 3, 3, 16> cnn_our_p_filters, cnn_our_k_filters, cnn_opp_p_filters, cnn_opp_k_filters;
//NetLayerType cnn_our_p_bias, cnn_our_k_bias, cnn_opp_p_bias, cnn_opp_k_bias;

//std::vector<NetLayerType> cnn_dense_out(cnn_dense_in, 0);

// NN weights

std::vector<NetLayerType> net_input_weights(772, 0);
NetLayerType bias_layer_one(0);

//std::vector<NetLayerType> second_layer_weights(16 * 16, 0);
//NetLayerType bias_layer_two(0);

std::vector<NetLayerType> output_weights;
std::array<float_t, 3> output_bias;

//NetLayerType win_weights(0);
//float win_bias;
//NetLayerType win_draw_weights(0);
//float win_draw_bias;

//constexpr std::array<int, 64> kPSTindex = {
//    0, 1, 2, 3, 3, 2, 1, 0,
//    1, 4, 5, 6, 6, 5, 4, 1,
//    2, 5, 7, 8, 8, 7, 5, 2,
//    3, 6, 8, 9, 9, 8, 6, 3,
//    3, 6, 8, 9, 9, 8, 6, 3,
//    2, 5, 7, 8, 8, 7, 5, 2,
//    1, 4, 5, 6, 6, 5, 4, 1,
//    0, 1, 2, 3, 3, 2, 1, 0
//};

//const Array2d<BitBoard, 2, 64> get_king_pawn_coverage() {
//  Array2d<BitBoard, 2, 64> coverage;
//  for (Square square = 0; square < 64; square++) {
//    BitBoard bb = GetSquareBitBoard(square);
//    for (int i = GetSquareY(square); i; --i) {
//      bb |= bitops::E(bb) | bitops::W(bb);
//    }
//    bb = bitops::FillNorthEast(bb, ~0) | bitops::FillNorthWest(bb, ~0);
//    bb = bitops::FillNorth(bb, ~0);
//    bb &= ~bitops::seventh_rank | bitops::N(bb);
//    coverage[kWhite][square] = bb;
//    bb = GetSquareBitBoard(square);
//    for (int i = 7 - GetSquareY(square); i; --i) {
//      bb |= bitops::E(bb) | bitops::W(bb);
//    }
//    bb = bitops::FillSouthEast(bb, ~0) | bitops::FillSouthWest(bb, ~0);
//    bb = bitops::FillSouth(bb, ~0);
//    bb &= ~bitops::second_rank | bitops::S(bb);
//    coverage[kBlack][square] = bb;
//  }
//  return coverage;
//}

//const Array2d<BitBoard, 2, 64> king_pawn_coverage = get_king_pawn_coverage();

//const std::array<int, 15*15> relative_king_map = {
//    0,  1,  2,  3,  4,  5,  6,  7,  6,  5,  4,  3,  2,  1, 8,
//    1,  8,  9, 10, 11, 12, 13, 14, 13, 12, 11, 10,  9,  8, 1,
//    2,  9, 15, 16, 17, 18, 19, 20, 19, 18, 17, 16, 15,  9, 2,
//    3, 10, 16, 21, 22, 23, 24, 25, 24, 23, 22, 21, 16, 10, 3,
//    4, 11, 17, 22, 26, 27, 28, 29, 28, 27, 26, 22, 17, 11, 4,
//    5, 12, 18, 23, 27, 30, 31, 32, 31, 30, 27, 23, 18, 12, 5,
//    6, 13, 19, 24, 28, 31, 33, 34, 33, 31, 28, 24, 19, 13, 6,
//    7, 14, 20, 25, 29, 32, 34, 35, 34, 32, 29, 25, 20, 14, 7,
//    6, 13, 19, 24, 28, 31, 33, 34, 33, 31, 28, 24, 19, 13, 6,
//    5, 12, 18, 23, 27, 30, 31, 32, 31, 30, 27, 23, 18, 12, 5,
//    4, 11, 17, 22, 26, 27, 28, 29, 28, 27, 26, 22, 17, 11, 4,
//    3, 10, 16, 21, 22, 23, 24, 25, 24, 23, 22, 21, 16, 10, 3,
//    2,  9, 15, 16, 17, 18, 19, 20, 19, 18, 17, 16, 15,  9, 2,
//    1,  8,  9, 10, 11, 12, 13, 14, 13, 12, 11, 10,  9,  8, 1,
//    8,  1,  2,  3,  4,  5,  6,  7,  6,  5,  4,  3,  2,  1, 0
//};

template<typename T>
T init() {
  return T(0);
}

//template<typename T>
//T init_cnn_in() {
//  return T(kBoardSize * kNumChannels);
//}

//template<>
//CNNLayerType init_cnn_in() {
//  return cnn_input_bias;
//}

//template<>
//NetLayerType init() {
//  return NetLayerType(0);
//}

//template<> PScore init<PScore>() { return PScore(0); }

template<typename T> inline
void AddFeature(T &s, const int index, const int value) {
  s[index] += value;
}

template<> inline void AddFeature<NetLayerType>(NetLayerType &s, const int index, const int value) {
  s.FMA(net_input_weights[index], value);
}

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

// TODO refactor AddFeaturePair to utilize AddFeature instead.
//template<typename T, Color our_color> inline
//void AddFeaturePair(T &s, const int index, const int value_white, const int value_black) {
  //if (our_color == kWhite) {
    //s[index] += value_white;
    //s[index + kSideDependentFeatureCount] += value_black;
  //}
  //else {
    //s[index] += value_black;
    //s[index + kSideDependentFeatureCount] += value_white;
  //}
//}

//template<> inline
//void AddFeaturePair<NetLayerType, kWhite>(NetLayerType &s, const int index, const int value_white, const int value_black) {
  //s.FMA(net_input_weights[index], value_white);
  //s.FMA(net_input_weights[index + kSideDependentFeatureCount], value_black);
//}

//template<> inline
//void AddFeaturePair<NetLayerType, kBlack>(NetLayerType &s, const int index,
                                          //const int value_white, const int value_black) {
  //s.FMA(net_input_weights[index], value_black);
  //s.FMA(net_input_weights[index + kSideDependentFeatureCount], value_white);
//}

//template<typename T> inline
//void AddRawBoardFeature(T &s, const int channel, const Square square) {
  //s[square * kNumChannels + channel]++;
//}

//template<> inline
//void AddRawBoardFeature(CNNLayerType &s, const int channel, const Square square) {
//  int h = GetSquareY(square);
//  int w = GetSquareX(square);
//  for (int i = 0; i < 5; ++i) {
//    if (h+i <= 1 || h+i >= 10) {
//      continue;
//    }
//    for (int j = 0; j < 5; ++j) {
//      if (w+j <= 1 || w+j >= 10) {
//        continue;
//      }
//      s[h+i-2][w+j-2] += cnn_l1_filters[4-i][4-j][channel];
//    }
//  }
//}

//BitBoard get_all_major_pieces(const Board &board) {
  //BitBoard all_major_pieces = 0;
  //for (PieceType piece = kRook; piece <= kKing; piece++) {
    //all_major_pieces |= board.get_piecetype_bitboard(piece);
  //}
  //return all_major_pieces;
//}

//std::array<BitBoard, 2> get_p_forward(const std::array<BitBoard, 2> &pawn_bb, const BitBoard empty) {
  //std::array<BitBoard, 2> p_forward {
      //bitops::N(pawn_bb[kWhite]) & empty,
      //bitops::S(pawn_bb[kBlack]) & empty
  //};

  //p_forward[kWhite] |= bitops::N(p_forward[kWhite]) & empty & bitops::fourth_rank;
  //p_forward[kBlack] |= bitops::S(p_forward[kBlack]) & empty & bitops::fifth_rank;
  //return p_forward;
//}

//std::array<BitBoard, 2> get_passed(const std::array<BitBoard, 2> &p_fill_forward,
                                   //const std::array<BitBoard, 2> &pawn_bb) {
  //BitBoard n_filled = bitops::NW(p_fill_forward[kWhite]) | bitops::NE(p_fill_forward[kWhite]) | p_fill_forward[kWhite];
  //BitBoard s_filled = bitops::SW(p_fill_forward[kBlack]) | bitops::SE(p_fill_forward[kBlack]) | p_fill_forward[kBlack];
  //return {
    //pawn_bb[kWhite] & (~(s_filled | bitops::S(bitops::FillSouth(pawn_bb[kWhite], ~0)))),
    //pawn_bb[kBlack] & (~(n_filled | bitops::N(bitops::FillNorth(pawn_bb[kBlack], ~0))))
  //};
//}

//struct EvalConstants;

//struct CheckingSquares {
  //CheckingSquares(const BitBoard all_pieces, const std::array<Square, 2> king_squares,
                  //const std::array<BitBoard, 2> c_pieces, const std::array<BitBoard, 2> controlled) :
    //safe{{ {0, 0, 0, 0}, {0, 0, 0, 0} }},
    //unsafe{{ {0, 0, 0, 0}, {0, 0, 0, 0} }}
  //{
    //for (Color color = kWhite; color <= kBlack; color++) {
      //Color not_color = color ^ 0x1;
      //Square enemy_king = king_squares[not_color];
      //unsafe[color][kKnight - kKnight] = magic::GetAttackMap<kKnight>(enemy_king, all_pieces);
      //unsafe[color][kBishop - kKnight] = magic::GetAttackMap<kBishop>(enemy_king, all_pieces);
      //unsafe[color][kRook - kKnight] = magic::GetAttackMap<kRook>(enemy_king, all_pieces);
      //unsafe[color][kQueen - kKnight] = unsafe[color][kRook - kKnight]
                                                 //| unsafe[color][kBishop - kKnight];
      //BitBoard safe_squares = ~(c_pieces[color] | controlled[not_color]);
      //for (PieceType pt = kKnight - kKnight; pt <= kQueen - kKnight; pt++) {
        //safe[color][pt] = safe_squares & unsafe[color][pt];
        //unsafe[color][pt] &= ~(c_pieces[color]);
        //unsafe[color][pt] ^= safe[color][pt];
      //}
    //}
  //}
  //Array2d<BitBoard, 2, 4> safe;
  //Array2d<BitBoard, 2, 4> unsafe;
//};

//struct EvalConstants {
  //EvalConstants(const Board &board) :
    //king_squares({
        //bitops::NumberOfTrailingZeros(board.get_piece_bitboard(kWhite, kKing)),
        //bitops::NumberOfTrailingZeros(board.get_piece_bitboard(kBlack, kKing))
    //}),
    //all_major_pieces(get_all_major_pieces(board)),
    //major_pieces({ all_major_pieces & board.get_color_bitboard(kWhite),
                   //all_major_pieces & board.get_color_bitboard(kBlack) }),
    //c_pieces({board.get_color_bitboard(kWhite), board.get_color_bitboard(kBlack)}),
    //all_pieces(board.get_color_bitboard(kWhite) | board.get_color_bitboard(kBlack)),
    //empty(~all_pieces),
    //pawn_bb({
        //board.get_piece_bitboard(kWhite, kPawn),
        //board.get_piece_bitboard(kBlack, kPawn)
    //}),
    //all_pawns_bb(pawn_bb[kWhite] | pawn_bb[kBlack]),
    //covered_once({
        //bitops::NE(pawn_bb[kWhite]) | bitops::NW(pawn_bb[kWhite]),
        //bitops::SE(pawn_bb[kBlack]) | bitops::SW(pawn_bb[kBlack])
    //}),
    //covered_potentially({
        //bitops::FillNorth(covered_once[kWhite], ~0),
        //bitops::FillSouth(covered_once[kBlack], ~0)
    //}),
    //covered_twice({
        //bitops::NE(pawn_bb[kWhite]) & bitops::NW(pawn_bb[kWhite]),
        //bitops::SE(pawn_bb[kBlack]) & bitops::SW(pawn_bb[kBlack])
    //}),
    //hard_block({
        //(pawn_bb[kWhite] & ~(bitops::S(empty) | bitops::SE(c_pieces[kBlack])
                                             //| bitops::SW(c_pieces[kBlack])))
        //| (pawn_bb[kBlack] & ((covered_once[kBlack] & ~covered_once[kWhite])
                              //|(covered_twice[kBlack] & ~covered_twice[kWhite]))),
        //(pawn_bb[kBlack] & ~(bitops::N(empty) | bitops::NE(c_pieces[kWhite])
                                             //| bitops::NW(c_pieces[kWhite])))
        //| (pawn_bb[kWhite] & ((covered_once[kWhite] & ~covered_once[kBlack])
                              //|(covered_twice[kWhite] & ~covered_twice[kBlack]))),
    //}),
    //p_forward(get_p_forward(pawn_bb, empty)),
    //p_fill_forward({bitops::FillNorth(pawn_bb[kWhite], ~0),
                    //bitops::FillSouth(pawn_bb[kBlack], ~0)}),
    //opposed_pawns((p_fill_forward[kBlack] & pawn_bb[kWhite])
                  //| (p_fill_forward[kWhite] & pawn_bb[kBlack])),
    //neighbored_pawns({
                      //(bitops::E(pawn_bb[kWhite]) | bitops::W(pawn_bb[kWhite])) & pawn_bb[kWhite],
                      //(bitops::E(pawn_bb[kBlack]) | bitops::W(pawn_bb[kBlack])) & pawn_bb[kBlack]
                  //}),
    //passed(get_passed(p_fill_forward, pawn_bb)),
    //controlled({
                //board.PlayerBitBoardControl(kWhite, all_pieces),
                //board.PlayerBitBoardControl(kBlack, all_pieces)
              //}),
    //nbr_bitboard(board.get_piecetype_bitboard(kKnight)
                 //| board.get_piecetype_bitboard(kBishop)
                 //| board.get_piecetype_bitboard(kRook)),
    //checks(all_pieces, king_squares, c_pieces, controlled) {}

  //const std::array<Square, 2> king_squares;           // Squares of each respective king
  //const BitBoard all_major_pieces;                    // Kings, Queens and Rooks
  //const std::array<BitBoard, 2> major_pieces;         // Kings, Queens and Rooks, by color
  //const std::array<BitBoard, 2> c_pieces;             // All pieces, by color
  //const BitBoard all_pieces;                          // BitBoard of all pieces
  //const BitBoard empty;                               // ~all_pieces
  //const std::array<BitBoard, 2> pawn_bb;              // Pawn BitBoard, by color
  //const BitBoard all_pawns_bb;                        // BitBoard of all pawns
  //const std::array<BitBoard, 2> covered_once;         // BitBoard of squares covered by pawns at least once, by color
  //const std::array<BitBoard, 2> covered_potentially;
  //const std::array<BitBoard, 2> covered_twice;
  //const std::array<BitBoard, 2> hard_block;
  //const std::array<BitBoard, 2> p_forward;
  //const std::array<BitBoard, 2> p_fill_forward;
  //const BitBoard opposed_pawns;
  //const std::array<BitBoard, 2> neighbored_pawns;
  //const std::array<BitBoard, 2> passed;
  //const std::array<BitBoard, 2> controlled;
  //const BitBoard nbr_bitboard;
  //const CheckingSquares checks;
//};

//struct EvalCounter {
  //Vec<int, (kQueen - kPawn)> king_zone_attacks = Vec<int, (kQueen - kPawn)>(0);
  //int king_attack_count = 0;
  //int safe = 0;
  //int unsafe = 0;
//};

//enum PawnCategory {
  //Opposed = 0, Unopposed = 1, PassedCovered = 2, PassedUncovered = 3
//};

//template<typename T, Color color, Color our_color, PawnCategory cat>
//inline void AddPawnsToInputByGroup(T &score, const EvalConstants &ec, BitBoard cat_pawns, CNNHelper &helper) {
  //constexpr size_t offset = color == our_color ? 0 : kChannelsPerSide;

  //for (; cat_pawns; bitops::PopLSB(cat_pawns)) {
    //Square pawn_square = bitops::NumberOfTrailingZeros(cat_pawns);
    //if (our_color == kBlack) {
      //pawn_square = GetMirroredSquare(pawn_square);
    //}
    //if (color == our_color) {
      //helper.our_p.emplace_back(pawn_square);
    //}
    //else {
      //helper.opp_p.emplace_back(pawn_square);
    //}
    //AddRawBoardFeature<T>(score, cat + offset, pawn_square);
  //}
//}

//template<typename T, Color color, Color our_color>
//inline void ScorePawnThreats(T &score, const EvalConstants &ec) {
  //constexpr Color not_color = color ^ 0x1;
  //constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  //AddFeature<T>(score, offset + kPawnAttackIndex,
                //bitops::PopCount(ec.covered_once[color]
                                //& (ec.c_pieces[not_color] ^ ec.pawn_bb[not_color])));
//}

//template<typename T, Color color, Color our_color>
//inline void AddPawnsToInput(T &score, const EvalConstants &ec, CNNHelper &helper) {
  //constexpr Color not_color = color ^ 0x1;

  //AddPawnsToInputByGroup<T, color, our_color, PawnCategory::Opposed>(
      //score, ec, ec.pawn_bb[color] & ec.opposed_pawns, helper);
  //AddPawnsToInputByGroup<T, color, our_color, PawnCategory::Unopposed>(
      //score, ec, ec.pawn_bb[color] & ~(ec.opposed_pawns | ec.passed[color]), helper);
  //AddPawnsToInputByGroup<T, color, our_color, PawnCategory::PassedCovered>(
      //score, ec, ec.passed[color] & king_pawn_coverage[not_color][ec.king_squares[not_color]], helper);
  //AddPawnsToInputByGroup<T, color, our_color, PawnCategory::PassedUncovered>(
      //score, ec, ec.passed[color] & ~king_pawn_coverage[not_color][ec.king_squares[not_color]], helper);
//}

//template<typename T, Color color, Color our_color>
//inline void ScoreKnights(T &score, const Board &board, const EvalConstants &ec,
                        //EvalCounter &counter, const BitBoard enemy_king_zone) {
  //constexpr Color not_color = color ^ 0x1;
  //constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  //for (BitBoard pieces = board.get_piece_bitboard(color, kKnight); pieces; bitops::PopLSB(pieces)) {
    //Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    //AddFeature<T>(score, offset + kKnightPSTIdx + kPSTindex[piece_square], 1);
    //AddFeature<T>(score, offset + kKingAttackDistance + kKnight - 1,
        //magic::GetSquareDistance(piece_square, ec.king_squares[not_color]));
////      int relative_x = GetSquareX(piece_square) - GetSquareX(enemy_king_square) + 7;
////      int relative_y = GetSquareY(piece_square) - GetSquareY(enemy_king_square) + 7;
////      AddFeature<T>(score, color, kKnightVsKingPosition + relative_king_map[relative_x + 15 * relative_y], 1);
    //BitBoard attack_map = magic::GetAttackMap<kKnight>(piece_square, ec.all_pieces);
    //counter.unsafe += bitops::PopCount(attack_map
                                             //& ec.checks.unsafe[color][kKnight-kKnight]);
    //counter.safe += bitops::PopCount(attack_map
                                           //& ec.checks.safe[color][kKnight-kKnight]);
    //AddFeature<T>(score, offset + kKnightSquaresIndex +
                  //bitops::PopCount(attack_map & ~(ec.c_pieces[color] | ec.covered_once[not_color])), 1);
    //AddFeature<T>(score, offset + kMinorAttackIndex,
        //bitops::PopCount(attack_map & ec.major_pieces[not_color]));
    //if (attack_map & enemy_king_zone) {
      //counter.king_attack_count++;
      //counter.king_zone_attacks[kKnight - kKnight]++;//= bitops::PopCount(attack_map & enemy_king_zone);
    //}
  //}

  //AddFeature<T>(score, offset + kKnightOutpost,
      //bitops::PopCount(board.get_piece_bitboard(color, kKnight)
          //& ec.covered_once[color]
          //& (~ec.covered_potentially[not_color])));
//}

//template<typename T, Color color, Color our_color>
//inline void ScoreBishops(T &score, const Board &board, const EvalConstants &ec,
                        //EvalCounter &counter, const BitBoard enemy_king_zone) {
  //constexpr Color not_color = color ^ 0x1;
  //constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  //const Square enemy_king_square = ec.king_squares[not_color];
  //BitBoard bishop_targets = 0;
  //BitBoard abstract_targets = 0;
  //for (BitBoard pieces = board.get_piece_bitboard(color, kBishop); pieces; bitops::PopLSB(pieces)) {
    //Square piece_square = bitops::NumberOfTrailingZeros(pieces);
////      AddFeature<T>(score, color, kKingAttackDistance + kBishop - 1,
////          magic::GetSquareDistance(piece_square, king_squares[not_color]));
    //int relative_x = GetSquareX(piece_square) - GetSquareX(enemy_king_square) + 7;
    //int relative_y = GetSquareY(piece_square) - GetSquareY(enemy_king_square) + 7;
    //AddFeature<T>(score, offset + kBishopVsKingPosition + relative_king_map[relative_x + 15 * relative_y], 1);
    //BitBoard attack_map = magic::GetAttackMap<kBishop>(piece_square, ec.all_pieces)
        //& (~ec.covered_once[not_color] | (ec.c_pieces[not_color] ^ ec.pawn_bb[not_color]));
    //counter.unsafe += bitops::PopCount(attack_map
                                           //& ec.checks.unsafe[color][kBishop-kKnight]);
    //counter.safe += bitops::PopCount(attack_map
                                           //& ec.checks.safe[color][kBishop-kKnight]);
    //bishop_targets |= attack_map;
    //if (attack_map & enemy_king_zone) {
      //counter.king_attack_count++;
      //counter.king_zone_attacks[kBishop - kKnight]++;
    //}
    //AddFeature<T>(score, offset + kBishopMobility
                  //+ bitops::PopCount(attack_map & ~ec.c_pieces[color]), 1);
    //abstract_targets |= magic::GetAttackMap<kBishop>(piece_square, ec.hard_block[color]);
  //}
  //bishop_targets &= ~ec.c_pieces[color];
  //AddFeature<T>(score, offset + kMinorAttackIndex,
      //bitops::PopCount(bishop_targets & ec.major_pieces[not_color]));
  //abstract_targets &= ~bishop_targets;
  //AddFeature<T>(score, offset + kBishop + kAbstractActivityIndex,
      //bitops::PopCount(abstract_targets));
//}

//template<typename T, Color color, Color our_color>
//inline void ScoreRooks(T &score, const Board &board, const EvalConstants &ec,
                       //EvalCounter &counter, const BitBoard enemy_king_zone) {
  //constexpr Color not_color = color ^ 0x1;
  //constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  //for (BitBoard pieces = board.get_piece_bitboard(color, kRook); pieces; bitops::PopLSB(pieces)) {
    //Square piece_square = bitops::NumberOfTrailingZeros(pieces);

    //BitBoard file = magic::GetSquareFile(piece_square);
    //if (!(file & ec.all_pawns_bb)) {
      //AddFeature<T>(score, offset + kRookOpenFile, 1);
    //}

    //AddFeature<T>(score, offset + kKingAttackDistance + kRook - 1,
        //magic::GetSquareDistance(piece_square, ec.king_squares[not_color]));
    //BitBoard attack_map = ~ec.c_pieces[color]
        //& (~ec.covered_once[not_color] | (ec.c_pieces[not_color] ^ ec.pawn_bb[not_color]))
        //& magic::GetAttackMap<kRook>(piece_square, ec.all_pieces);
    //counter.unsafe += bitops::PopCount(attack_map
                                           //& ec.checks.unsafe[color][kRook-kKnight]);
    //counter.safe += bitops::PopCount(attack_map
                                           //& ec.checks.safe[color][kRook-kKnight]);
    //if (attack_map & enemy_king_zone) {
      //counter.king_attack_count++;
      //counter.king_zone_attacks[kRook - kKnight]++;
    //}
    //BitBoard abstract_attack = attack_map ^
        //magic::GetAttackMap<kRook>(piece_square, ec.hard_block[color]);
    //AddFeature<T>(score, offset + kRookMobility + bitops::PopCount(attack_map), 1);
    //AddFeature<T>(score, offset + kRook + kAbstractActivityIndex,
        //bitops::PopCount(abstract_attack));
  //}
//}

//template<typename T, Color color, Color our_color>
//inline void ScoreQueens(T &score, const Board &board, const EvalConstants &ec,
                        //EvalCounter &counter, const BitBoard enemy_king_zone) {
  //constexpr Color not_color = color ^ 0x1;
  //constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  //for (BitBoard pieces = board.get_piece_bitboard(color, kQueen); pieces; bitops::PopLSB(pieces)) {
    //Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    //AddFeature<T>(score, offset + kKingAttackDistance + kQueen - 1,
        //magic::GetSquareDistance(piece_square, ec.king_squares[not_color]));
    //BitBoard attack_map = ~ec.c_pieces[color]
        //& (~ec.covered_once[not_color] | (ec.c_pieces[not_color] ^ ec.pawn_bb[not_color]))
        //& magic::GetAttackMap<kQueen>(piece_square, ec.all_pieces);
    //counter.unsafe += bitops::PopCount(attack_map
                                           //& ec.checks.unsafe[color][kQueen-kKnight]);
    //counter.safe += bitops::PopCount(attack_map
                                           //& ec.checks.safe[color][kQueen-kKnight]);
    //if (attack_map & enemy_king_zone) {
      //counter.king_attack_count++;
      //counter.king_zone_attacks[kQueen - kKnight]++;
    //}
    //AddFeature<T>(score, offset + kQueenMobility + std::min(bitops::PopCount(attack_map), 24), 1);
    //if (kUseQueenActivity) {
      //BitBoard abstract_attack = attack_map ^
          //magic::GetAttackMap<kQueen>(piece_square, ec.hard_block[color]);
      //AddFeature<T>(score, offset + kQueen + kActivityBonusIndex,
                    //bitops::PopCount(attack_map));
      //AddFeature<T>(score, offset + kQueen + kAbstractActivityIndex,
                    //bitops::PopCount(abstract_attack));
    //}
  //}
//}

//template<typename T, Color color, Color our_color>
//inline void ScoreKings(T &score, const Board &board,
                       //const EvalConstants &ec, const EvalCounter &counter) {
  //constexpr Color not_color = color ^ 0x1;
  //constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  //Square king_square = bitops::NumberOfTrailingZeros(board.get_piece_bitboard(color,kKing));

  //if (board.get_piece_bitboard(not_color, kQueen)) {
    //AddFeature<T>(score, offset + kKingVectorExposure,
                  //bitops::PopCount(magic::GetAttackMap<kBishop>(king_square,
                                       //board.get_piece_bitboard(color, kPawn))));
    //AddFeature<T>(score, offset + kKingVectorExposure + 1,
                  //bitops::PopCount(magic::GetAttackMap<kRook>(king_square,
                                       //board.get_piece_bitboard(color, kPawn))));
  //}
  //else {
    //if (board.get_piece_bitboard(not_color, kBishop)) {
      //AddFeature<T>(score, offset + kKingVectorExposure,
                    //bitops::PopCount(magic::GetAttackMap<kBishop>(king_square,
                                         //board.get_piece_bitboard(color, kPawn))));
    //}
    //if (board.get_piece_bitboard(not_color, kRook)) {
      //AddFeature<T>(score, offset + kKingVectorExposure + 1,
                    //bitops::PopCount(magic::GetAttackMap<kRook>(king_square,
                                         //board.get_piece_bitboard(color, kPawn))));
    //}
  //}

  //if (counter.king_attack_count >= 2) {
    //int king_attack_scaling = kKingAttackerScaling[std::min(counter.king_attack_count, 6)];
    //for (PieceType pt = kKnight - kKnight; pt <= kQueen - kKnight; pt++) {
      //AddFeature<T>(score, offset + kKingAttack
                                  //+ pt, king_attack_scaling * counter.king_zone_attacks[pt]);
    //}
  //}
//}

//template<typename T, Color our_color>
//inline void AddKingsToInput(T &score, const Board &board, CNNHelper &helper) {
  //constexpr Color opp_color = our_color ^ 0x1;

  //Square our_king_square = bitops::NumberOfTrailingZeros(board.get_piece_bitboard(our_color,kKing));
  //helper.our_k = our_king_square;
  //Square opp_king_square = bitops::NumberOfTrailingZeros(board.get_piece_bitboard(opp_color,kKing));

  //if (our_color == kBlack) {
    //our_king_square = GetMirroredSquare(our_king_square);
    //opp_king_square = GetMirroredSquare(opp_king_square);
  //}

  //helper.our_k = our_king_square;
  //helper.opp_k = opp_king_square;

  //AddRawBoardFeature<T>(score, kChanKingsIdx, our_king_square);
  //AddRawBoardFeature<T>(score, kChanKingsIdx + kChannelsPerSide, opp_king_square);
//}

//template<typename T, Color our_color>
//inline void AddPawnCounts(T &score, const Board &board) {
  //constexpr Color opponent_color = our_color ^ 0x1;
  //constexpr size_t offset = kSideDependentFeatureCount;

  //AddFeature<T>(score, kPawnCountIdx + board.get_piece_count(our_color, kPawn), 1);
  //AddFeature<T>(score, kPawnCountIdx + offset + board.get_piece_count(opponent_color, kPawn), 1);
//}

//template<typename T>
//inline T GetSuperStaticRawFeatures(const Board &board, const EvalConstants &ec, CNNHelper &helper) {
  //T score = init_cnn_in<T>();
  //if (board.get_turn() == kWhite) {
    //AddPawnsToInput<T, kWhite, kWhite>(score, ec, helper);
    //AddPawnsToInput<T, kBlack, kWhite>(score, ec, helper);
    //AddKingsToInput<T, kWhite>(score, board, helper);
  //}
  //else {
    //AddPawnsToInput<T, kBlack, kBlack>(score, ec, helper);
    //AddPawnsToInput<T, kWhite, kBlack>(score, ec, helper);
    //AddKingsToInput<T, kBlack>(score, board, helper);
  //}
  //return score;
//}

//template<typename T, Color color, Color our_color>
//inline void ScorePieces(T &score, const Board &board, const EvalConstants &ec,
                        //EvalCounter &counter) {
  //const BitBoard enemy_king_zone = magic::GetKingArea(ec.king_squares[color ^ 0x1]);

  //ScorePawnThreats<T, color, our_color>(score, ec);
  //// ScorePawns<T, color, our_color>(score, ec);
  //ScoreKnights<T, color, our_color>(score, board, ec, counter, enemy_king_zone);
  //ScoreBishops<T, color, our_color>(score, board, ec, counter, enemy_king_zone);
  //ScoreRooks<T, color, our_color>(score, board, ec, counter, enemy_king_zone);
  //ScoreQueens<T, color, our_color>(score, board, ec, counter, enemy_king_zone);
  //ScoreKings<T, color, our_color>(score, board, ec, counter);
//}

//template<typename T, Color color, Color our_color>
//inline void AddPieceCountFeatures(T &score, const Board &board) {
  //constexpr size_t offset = color == our_color ? 0 : kSideDependentFeatureCount;

  //AddFeature<T>(score, kKnightCountIdx + offset + std::min(board.get_piece_count(color, kKnight), 2), 1);
  //AddFeature<T>(score, kBishopCountIdx + offset + std::min(board.get_piece_count(color, kBishop), 2), 1);
  //AddFeature<T>(score, kRookCountIdx + offset + std::min(board.get_piece_count(color, kRook), 2), 1);
//}

//template<typename T, Color our_color>
//inline void AddCommonFeatures(T &score, const Board &board) {

  //AddPieceCountFeatures<T, kWhite, our_color>(score, board);
  //AddPieceCountFeatures<T, kBlack, our_color>(score, board);

  //AddFeaturePair<T, our_color>(score, kQueenCountIdx,
                               //board.get_piece_count(kWhite, kQueen),
                               //board.get_piece_count(kBlack, kQueen));

  //if (board.get_piece_count(kWhite, kBishop) == 1 && board.get_piece_count(kBlack, kBishop) == 1
      //&& bitops::PopCount(board.get_piecetype_bitboard(kBishop) & bitops::light_squares) == 1) {
    //AddFeature<T>(score, kOppositeColoredBishops, 1);
  //}
//}

//template<typename T, Color our_color>
//inline void AddCommonFeatures(T &score, const Board &board, const EvalConstants &ec) {
  //AddFeaturePair<T, our_color>(score, kPawn + kActivityBonusIndex,
                               //bitops::PopCount(ec.p_forward[kWhite]),
                               //bitops::PopCount(ec.p_forward[kBlack]));

  //// TODO remove double pawn penalty as it should be part of CNN
  //AddFeaturePair<T, our_color>(score, kDoublePawnPenaltyIndex,
                               //bitops::PopCount(bitops::N(ec.p_fill_forward[kWhite]) & ec.pawn_bb[kWhite]),
                               //bitops::PopCount(bitops::S(ec.p_fill_forward[kBlack]) & ec.pawn_bb[kBlack]));

  //AddFeaturePair<T, our_color>(score, kUnprotectedPieces,
                    //bitops::PopCount(ec.nbr_bitboard & ec.c_pieces[kWhite] & (~ec.controlled[kWhite])),
                    //bitops::PopCount(ec.nbr_bitboard & ec.c_pieces[kBlack] & (~ec.controlled[kBlack])));
//}

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
  for (PieceType piece_type = kKnight; piece_type <= kQueen; ++piece_type) {
      AddPieceType<T, color, our_color>(score, board, piece_type);
      //std::cout << score[0] << std::endl;
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

template<typename T, Color our_color>
inline T _ScorePawnsAndKings(const Board &board) {
  T score = init<T>();
  AddPieceType<T, kWhite, our_color>(score, board, kPawn);
  AddPieceType<T, kWhite, our_color>(score, board, kKing);
  AddPieceType<T, kBlack, our_color>(score, board, kPawn);
  AddPieceType<T, kBlack, our_color>(score, board, kKing);
  return score;
}

template<typename T>
inline T ScorePawnsAndKings(const Board &board) {
  if (board.get_turn() == kWhite) {
    return _ScorePawnsAndKings<T,kWhite>(board);
  }
  return _ScorePawnsAndKings<T,kBlack>(board);
}

}

namespace net_evaluation {

void SetPHashSize(const size_t bytes) {
  pawn_hash::size = bytes / 72;
  pawn_hash::table.resize(pawn_hash::size);
}

template<typename T, Color our_color>
T ScoreBoard(const Board &board) {
  T score = init<T>();

  // Piece evaluations
  ScorePieces<T, kWhite, our_color>(score, board);
  ScorePieces<T, kBlack, our_color>(score, board);
  
  ScoreCastlingRights<T, our_color>(score, board);

  return score;
}

//template<typename T, Color our_color>
//T ScoreBoard(const Board &board, const EvalConstants &ec) {
  //T score = init<T>();

  //// Initialize general constants
////  const EvalConstants ec(board);

  //// Pawn evaluations
  //AddPawnCounts<T, our_color>(score, board);
////  AddSuperStaticFeatures<T>(score, board, ec);

  //// Common features independent of specific individual piece placement.
  //AddCommonFeatures<T, our_color>(score, board);
  //AddCommonFeatures<T, our_color>(score, board, ec);

  //// Piece evaluations
  //std::array<EvalCounter, 2> check_counter;
  //ScorePieces<T, kWhite, our_color>(score, board, ec, check_counter[kWhite]);
  //ScorePieces<T, kBlack, our_color>(score, board, ec, check_counter[kBlack]);

  //// Features picked up while iterating over pieces
  //AddFeaturePair<T, our_color>(score, kSafeChecks, check_counter[kWhite].safe, check_counter[kBlack].safe);
  //AddFeaturePair<T, our_color>(score, kUnSafeChecks, check_counter[kWhite].unsafe, check_counter[kBlack].unsafe);

  //return score;
//}

//NetLayerType FiltersForward(const CNNLayerType &prev_cnn_layer, const Array3d<FNetLayerType, 3, 3, 16> &filters,
                            //const NetLayerType &bias, Square s) {
  //int h = GetSquareY(s);
  //int w = GetSquareX(s);
  //FNetLayerType result(bias);
  //for (size_t i = 0; i < 3; ++i) {
    //if (h+i == 0 || h+i > 8) {
      //continue;
    //}
    //for (size_t j = 0; j < 3; ++j) {
      //if (w+j == 0 || w+j > 8) {
        //continue;
      //}
      //for (size_t c = 0; c < prev_cnn_layer[0][0].size(); ++c) {
        ////result += prev_cnn_layer[h+i-1][w+j-1][c] * filters[i][j][c];
        //result.FMA(filters[i][j][c], prev_cnn_layer[h+i-1][w+j-1][c]);
      //}
    //}
  //}
  //result.relu();
  //return result.to_simple_vec();
//}

//NetLayerType NetForward(CNNLayerType &cnn_layer_one, const CNNHelper &helper) {
  //// ReLU 1
  //for (size_t h = 0; h < kBoardLength; ++h) {
    //for (size_t w = 0; w < kBoardLength; ++w) {
      //cnn_layer_one[h][w].relu();
    //}
  //}

  //NetLayerType our_p_out(0);
  //for (size_t i = 0; i < helper.our_p.size(); ++i) {
    //our_p_out += FiltersForward(cnn_layer_one, cnn_our_p_filters, cnn_our_p_bias, helper.our_p[i]);
  //}
  //NetLayerType our_k_out = FiltersForward(cnn_layer_one, cnn_our_k_filters, cnn_our_k_bias, helper.our_k);

  //NetLayerType opp_p_out(0);
  //for (size_t i = 0; i < helper.opp_p.size(); ++i) {
    //opp_p_out += FiltersForward(cnn_layer_one, cnn_opp_p_filters, cnn_opp_p_bias, helper.opp_p[i]);
  //}
  //NetLayerType opp_k_out = FiltersForward(cnn_layer_one, cnn_opp_k_filters, cnn_opp_k_bias, helper.opp_k);

  //// Dense
  //NetLayerType out(0);
  //size_t i;
  //for (i = 0; i < our_p_out.size(); ++i) {
    //out += our_p_out[i] * cnn_dense_out[i];
    //out += our_k_out[i] * cnn_dense_out[i + our_p_out.size()];
    //out += opp_p_out[i] * cnn_dense_out[i + 2 * our_p_out.size()];
    //out += opp_k_out[i] * cnn_dense_out[i + 3 * our_p_out.size()];
  //}

  //return out;
//}

//Score NetForward(NetLayerType &layer_one) {
  //layer_one += bias_layer_one;
  //layer_one.relu();

  //NetLayerType layer_two = bias_layer_two;
  //for (size_t i = 0; i < layer_one.size(); ++i) {
    //layer_two.FMA(second_layer_weights[i], layer_one[i]);
  //}
  //layer_two.relu();

  //float win = layer_two.dot(win_weights) + win_bias;
  //float win_draw = layer_two.dot(win_draw_weights) + win_draw_bias;

  ////float wpct = sigmoid(win) * c + sigmoid(win_draw) * (1 - c);

  ////return wpct_to_score(wpct);
  //return WDLScore::from_pct_valid(sigmoid(win), sigmoid(win_draw));
//}

NetLayerType PKNetForward(NetLayerType &pk_layer_one) {
  if (!use_pk_net) {
    return pk_layer_one;
  }
  pk_layer_one += pk_layer_one_bias;
  pk_layer_one.relu();
  
  NetLayerType layer_two = NetLayerType(0);
  for (size_t i = 0; i < pk_layer_one.size(); ++i) {
    layer_two.FMA(pk_layer_two_weights[i], pk_layer_one[i]);
    // layer_two[i] = pk_layer_one.dot(pk_layer_two_weights[i]);
  }
  
  return layer_two;
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
  // std::cout << sum << std::endl;
  
  for (size_t i = 0; i < 3; ++i) {
      outcomes[i] /= sum;
  }

  //NetLayerType layer_two = bias_layer_two;
  //for (size_t i = 0; i < layer_one.size(); ++i) {
  //  layer_two.FMA(second_layer_weights[i], layer_one[i]);
  //}
  //layer_two.relu();
  
  // std::cout << outcomes[0] << " " << outcomes[1] << " " << outcomes[2] << std::endl;

  float win = outcomes[0];
  float win_draw = outcomes[0] + outcomes[1];
  return WDLScore::from_pct_valid(win, win_draw);

  //float wpct = sigmoid(win) * c + sigmoid(win_draw) * (1 - c);

  //return wpct_to_score(wpct);
  //return WDLScore::from_pct_valid(sigmoid(win), sigmoid(win_draw));
}

Score ScoreBoard(const Board &board) {
  //const EvalConstants ec(board);
  HashType p_hash = board.get_pawn_hash();
  pawn_hash::PawnEntry entry = pawn_hash::GetEntry(p_hash);
  NetLayerType cnn_out;
  if (pawn_hash::ValidateHash(entry, p_hash)) {
    cnn_out = entry.output;
  }
  else {
    NetLayerType pk_in = ScorePawnsAndKings<NetLayerType>(board);
    cnn_out = PKNetForward(pk_in);
    pawn_hash::SaveEntry(cnn_out, p_hash);
  }
  //  CNNHelper helper;
  //  CNNLayerType cnn_input = GetSuperStaticRawFeatures<CNNLayerType>(board, ec, helper);
  //  cnn_out = NetForward(cnn_input, helper);
  //  pawn_hash::SaveEntry(cnn_out, p_hash);
  //}

  NetLayerType layer_one = init<NetLayerType>();
  if (board.get_turn() == kWhite) {
    layer_one = ScoreBoard<NetLayerType, kWhite>(board);
  }
  else {
    layer_one = ScoreBoard<NetLayerType, kBlack>(board);
  }
  // std::cout << layer_one[0] << std::endl;
  layer_one += cnn_out;
  if (contempt[board.get_turn()] != 0) {
    return AddContempt(NetForward(layer_one), board.get_turn());
  }
  return NetForward(layer_one);
}

//template<size_t size>
//void init_cnn_weights(Array3d<FNetLayerType, 3, 3, 16> &cnn_filters, const std::array<float, size> &weights,
                      //double multiplier = 1.0) {
  //size_t idx = 0;
  //for (size_t h = 0; h < cnn_filters.size(); ++h) {
    //for (size_t w = 0; w < cnn_filters[0].size(); ++w) {
      //for (size_t i = 0; i < cnn_filters[0][0].size(); ++i) {
        //NetLayerType tmp(0);
        //for (size_t j = 0; j < cnn_filters[0][0][0].size(); ++j) {
          //tmp[j] = weights[idx++] * multiplier;
          //// cnn_filters[h][w][i][j] = weights[idx++] * multiplier;
        //}
        //cnn_filters[h][w][i] = FNetLayerType(tmp);
      //}
    //}
  //}
  //assert(idx == size);
//}

//template<size_t size>
//void init_cnn_bias(NetLayerType &cnn_bias, const std::array<float, size> &bias_weights,
//                      double multiplier = 1.0) {
//  for (size_t i = 0; i < net_hardcode::cnn_l1_bias.size() && i < cnn_bias.size(); ++i) {
//    cnn_bias[i] = bias_weights[i] * multiplier;
//  }
//}

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
  
  if (use_pk_net) {
    std::vector<NetLayerType> l1_weights = load_weights(8*64 + 4, offset);
    
    for (size_t k = 0; k < block_size; ++k) {
      bias_layer_one[k] = gNetWeightsData[offset+k];
    }
    offset += block_size;
    
    // PK l1 weights
    std::vector<NetLayerType> pk_l1_weights = load_weights(4*64, offset);
    net_input_weights = std::vector<NetLayerType>(772);
    for (PieceType pt = kPawn; pt <= kKing; ++pt) {
      for (Color color = kWhite; color <= kBlack; ++color) {
        for (Square square = 0; square < 64; ++square) {
          size_t idx = (pt + 6 * color) * 64 + square;
          if (pt == kKing || pt == kPawn) {
            net_input_weights.at(idx) = pk_l1_weights.at(((pt/kKing)  + 2 * color) * 64 + square);
          }
          else {
            net_input_weights.at(idx) = l1_weights.at(((pt-1) + 4 * color) * 64 + square);
          }
        }
      }
    }
    for (size_t i=0; i < 4; ++i) {
      net_input_weights.at(12*64+i) = l1_weights.at(8*64+i);
    }
    
    // PK l1 bias
    for (size_t k = 0; k < block_size; ++k) {
      pk_layer_one_bias[k] = gNetWeightsData[offset+k];
    }
    offset += block_size;
    
    // PK l2 weights
    pk_layer_two_weights = std::vector<NetLayerType>(block_size);
    for (size_t i = 0; i < block_size; ++i) {
      for (size_t k = 0; k < block_size; ++k) {
        pk_layer_two_weights[i][k] = gNetWeightsData[offset + i + block_size * k];
      }
    }
    //std::cout << pk_layer_two_weights[0][0] << " " << pk_layer_two_weights[0][1] << std::endl;
    offset += block_size * block_size;
  }
  else {
    // L1 Weights
    net_input_weights = load_weights(772, offset);
  
    // L1 Bias
    for (size_t k = 0; k < block_size; ++k) {
      bias_layer_one[k] = gNetWeightsData[offset+k];
    }
    offset += block_size;
  }
  
  
  // Output Weights
  output_weights = std::vector<NetLayerType>(3);
  for (size_t i = 0; i < block_size; ++i) {
    //size_t j = i * block_size;
    for (size_t k = 0; k < 3; ++k) {
      //output_weights[i][k] = gNetWeightsData[offset+j+k];
      output_weights[k][i] = gNetWeightsData[offset+i+block_size*k];
    }
  }
  offset += 3 * block_size;
  
  // Output Bias
  for (size_t k = 0; k < 3; ++k) {
    output_bias[k] = gNetWeightsData[offset+k];
  }
  
  //std::cout << "l1 w0:" << net_input_weights[0][0] << " l1 w1:" << net_input_weights[0][1]
  //          << " l1 b0:" << bias_layer_one[0] << " l1 b1:" << bias_layer_one[1] << std::endl;
  //std::cout << "out w0:" << output_weights[0][0] << " out w1:" << output_weights[0][1]
  //          << " out b0:" << output_bias[0] << " out b1:" << output_bias[1] << std::endl;
}

//std::vector<int32_t> GetCNNInputs(const Board &board) {
  //const EvalConstants ec(board);
  //CNNHelper helper;
  //return GetSuperStaticRawFeatures<std::vector<int32_t> >(board, ec, helper);
//}

//std::vector<int32_t> GetNetInputs(const Board &board) {
  //const EvalConstants ec(board);
  //if (board.get_turn() == kWhite) {
    //return ScoreBoard<std::vector<int32_t>, kWhite>(board, ec);
  //}
  //return ScoreBoard<std::vector<int32_t>, kBlack>(board, ec);
//}

std::vector<int32_t> GetNetInputs(const Board &board) {
  //const EvalConstants ec(board);
  if (board.get_turn() == kWhite) {
    return ScoreBoard<std::vector<int32_t>, kWhite>(board);
  }
  return ScoreBoard<std::vector<int32_t>, kBlack>(board);
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

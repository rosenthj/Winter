/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2016 Jonas Kuratli, Jonathan Maurer, Jonathan Rosenthal
 *  Copyright (C) 2017-2019 Jonathan Rosenthal
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

/*
 * evaluation.cc
 *
 *  Created on: Nov 6, 2016
 *      Author: Jonathan Rosenthal
 */

#include "evaluation.h"
#include "learning/linear_algebra.h"
//#include "learning/statistics.h"
#include "learning/cluster.h"
#include "learning/machine_learning.h"
#include "benchmark.h"
#include "search.h"
#include "general/feature_indexes.h"
#include "general/magic.h"
#include "general/settings.h"
#include "general/bit_operations.h"
#include "general/hardcoded_params.h"
#include "data.h"
#include <random>
#include <algorithm>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include <utility>
#include <cassert>

using namespace positional_features;

namespace {
const double pi = std::acos(-1);
const double kEuler = std::exp(1);
std::mt19937_64 rng;

struct TrainingPosition{
  Board board;
  double target;
};

int kSign[2] = {1,-1};

std::vector<PScore> eval_score_values(kNumFeatures, 0);

cluster::ClusterModel<settings::kNumClusters>* init_cluster_model() {
  return new cluster::NormFuzzyCMeans<settings::kNumClusters, kPhaseVecLength>();
}

cluster::ClusterModel<settings::kNumClusters>* cluster_model = init_cluster_model();

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
    0,  1,  2,  3,  4,  5,  6,  7,  6,  5,  4,  3,  2,  1, 0,
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
    0,  1,  2,  3,  4,  5,  6,  7,  6,  5,  4,  3,  2,  1, 0
};

template<typename T>
std::vector<T> operator-(std::vector<T> v) {
  for (size_t i = 0; i < v.size(); i++) {
    v[i] *= (-1);
  }
  return v;
}

template<typename T>
T init() {
  return T(kNumFeatures);
}

template<> PScore init<PScore>() { return PScore(0); }

template<typename T> inline
void AddFeature(T &s,const Color color, const int index, const int value) {
  s[index] += kSign[color] * value;
}

template<> inline void AddFeature<PScore>(PScore &s, const Color color, const int index,
    const int value) {
  s.FMA(eval_score_values[index], kSign[color] * value);
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

std::array<BitBoard, 2> get_passed(const std::array<BitBoard, 2> &p_fill_forward, const std::array<BitBoard, 2> &pawn_bb) {
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

template<typename T, Color color>
inline void ScoreIsolated(T &score, const EvalConstants &ec) {
  BitBoard not_isolated = bitops::E(ec.pawn_bb[color]) | bitops::W(ec.pawn_bb[color]);
  not_isolated = bitops::FillNorth(not_isolated, ~0) | bitops::FillSouth(not_isolated, ~0);
  AddFeature<T>(score, color, kIsolatedPawnIndex, bitops::PopCount(ec.pawn_bb[color] & ~not_isolated));
}

template<typename T, Color color>
inline void ScorePawns(T &score, const EvalConstants &ec) {
  constexpr Color not_color = color ^ 0x1;

  ScoreIsolated<T, color>(score, ec);

  AddFeature<T>(score, color, kPawnAttackIndex,
                bitops::PopCount(ec.covered_once[color]
                                & (ec.c_pieces[not_color] ^ ec.pawn_bb[not_color])));

  std::array<BitBoard, 4> categorized_pawns {
    ec.pawn_bb[color] & ec.opposed_pawns,
    ec.pawn_bb[color] & ~(ec.opposed_pawns | ec.passed[color]),
    ec.passed[color] & king_pawn_coverage[not_color][ec.king_squares[not_color]],
    ec.passed[color] & ~king_pawn_coverage[not_color][ec.king_squares[not_color]]
  };
  for (int cat = 0; cat < 4; cat++) {
    int cat_offset = 24 * cat;
    for (BitBoard cat_pawns = categorized_pawns[cat]; cat_pawns; bitops::PopLSB(cat_pawns)) {
      Square pawn_square = bitops::NumberOfTrailingZeros(cat_pawns);
      BitBoard pawn = bitops::GetLSB(cat_pawns);
      bool is_neighbored = pawn & ec.neighbored_pawns[color];
      bool is_protected = pawn & ec.covered_once[color];
      int pawn_rank = color * 7 + kSign[color] * GetSquareY(pawn_square) - 1;
      AddFeature<T>(score, color, kPawnEvalIndex + cat_offset + 12 * is_neighbored
                                                 + 6 * is_protected + pawn_rank, 1);
    }
  }
}

template<typename T, Color color>
inline void ScoreKnights(T &score, const Board &board, const EvalConstants &ec,
                        EvalCounter &counter, const BitBoard enemy_king_zone) {
  constexpr Color not_color = color ^ 0x1;

  for (BitBoard pieces = board.get_piece_bitboard(color, kKnight); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    AddFeature<T>(score, color, kKnightPSTIndex + kPSTindex[piece_square], 1);
    AddFeature<T>(score, color, kKingAttackDistance + kKnight - 1,
        magic::GetSquareDistance(piece_square, ec.king_squares[not_color]));
//      int relative_x = GetSquareX(piece_square) - GetSquareX(enemy_king_square) + 7;
//      int relative_y = GetSquareY(piece_square) - GetSquareY(enemy_king_square) + 7;
//      AddFeature<T>(score, color, kKnightVsKingPosition + relative_king_map[relative_x + 15 * relative_y], 1);
    BitBoard attack_map = magic::GetAttackMap<kKnight>(piece_square, ec.all_pieces);
    counter.unsafe += bitops::PopCount(attack_map
                                             & ec.checks.unsafe[color][kKnight-kKnight]);
    counter.safe += bitops::PopCount(attack_map
                                           & ec.checks.safe[color][kKnight-kKnight]);
    AddFeature<T>(score, color, kKnightSquaresIndex +
                  bitops::PopCount(attack_map & ~(ec.c_pieces[color] | ec.covered_once[not_color])), 1);
    AddFeature<T>(score, color, kMinorAttackIndex,
        bitops::PopCount(attack_map & ec.major_pieces[not_color]));
    if (attack_map & enemy_king_zone) {
      counter.king_attack_count++;
      counter.king_zone_attacks[kKnight - kKnight]++;//= bitops::PopCount(attack_map & enemy_king_zone);
    }
  }

  AddFeature<T>(score, color, kKnightOutpost,
      bitops::PopCount(board.get_piece_bitboard(color, kKnight)
          & ec.covered_once[color]
          & (~ec.covered_potentially[not_color])));
}

template<typename T, Color color>
inline void ScoreBishops(T &score, const Board &board, const EvalConstants &ec,
                        EvalCounter &counter, const BitBoard enemy_king_zone) {
  constexpr Color not_color = color ^ 0x1;
  const Square enemy_king_square = ec.king_squares[not_color];
  BitBoard bishop_targets = 0;
  BitBoard abstract_targets = 0;
  for (BitBoard pieces = board.get_piece_bitboard(color, kBishop); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
//      AddFeature<T>(score, color, kKingAttackDistance + kBishop - 1,
//          magic::GetSquareDistance(piece_square, king_squares[not_color]));
    int relative_x = GetSquareX(piece_square) - GetSquareX(enemy_king_square) + 7;
    int relative_y = GetSquareY(piece_square) - GetSquareY(enemy_king_square) + 7;
    AddFeature<T>(score, color, kBishopVsKingPosition + relative_king_map[relative_x + 15 * relative_y], 1);
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
    AddFeature<T>(score, color, kBishopMobility
                  + bitops::PopCount(attack_map & ~ec.c_pieces[color]), 1);
    abstract_targets |= magic::GetAttackMap<kBishop>(piece_square, ec.hard_block[color]);
  }
  bishop_targets &= ~ec.c_pieces[color];
  AddFeature<T>(score, color, kMinorAttackIndex,
      bitops::PopCount(bishop_targets & ec.major_pieces[not_color]));
  abstract_targets &= ~bishop_targets;
  AddFeature<T>(score, color, kBishop + kAbstractActivityIndex,
      bitops::PopCount(abstract_targets));
}

template<typename T, Color color>
inline void ScoreRooks(T &score, const Board &board, const EvalConstants &ec,
                       EvalCounter &counter, const BitBoard enemy_king_zone) {
  constexpr Color not_color = color ^ 0x1;

  for (BitBoard pieces = board.get_piece_bitboard(color, kRook); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);

    BitBoard file = magic::GetSquareFile(piece_square);
    if (!(file & ec.all_pawns_bb)) {
      AddFeature<T>(score, color, kRookOpenFile, 1);
    }

    AddFeature<T>(score, color, kKingAttackDistance + kRook - 1,
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
    AddFeature<T>(score, color, kRookMobility + bitops::PopCount(attack_map), 1);
    AddFeature<T>(score, color, kRook + kAbstractActivityIndex,
        bitops::PopCount(abstract_attack));
  }
}

template<typename T, Color color>
inline void ScoreQueens(T &score, const Board &board, const EvalConstants &ec,
                        EvalCounter &counter, const BitBoard enemy_king_zone) {
  constexpr Color not_color = color ^ 0x1;

  for (BitBoard pieces = board.get_piece_bitboard(color, kQueen); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    AddFeature<T>(score, color, kKingAttackDistance + kQueen - 1,
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
    AddFeature<T>(score, color, kQueenMobility + bitops::PopCount(attack_map), 1);
    if (evaluation_features::kUseQueenActivity) {
      BitBoard abstract_attack = attack_map ^
          magic::GetAttackMap<kQueen>(piece_square, ec.hard_block[color]);
      AddFeature<T>(score, color, kQueen + kActivityBonusIndex,
                    bitops::PopCount(attack_map));
      AddFeature<T>(score, color, kQueen + kAbstractActivityIndex,
                    bitops::PopCount(abstract_attack));
    }
  }
}

template<typename T, Color color>
inline void ScoreKings(T &score, const Board &board,
                       const EvalConstants &ec, const EvalCounter &counter) {
  constexpr Color not_color = color ^ 0x1;

  Square king_square = bitops::NumberOfTrailingZeros(board.get_piece_bitboard(color,kKing));
  AddFeature<T>(score, color, kKingPSTIndex + kPSTindex[king_square], 1);

  if (board.get_piece_bitboard(not_color, kQueen)) {
    AddFeature<T>(score, color, kKingVectorExposure,
                  bitops::PopCount(magic::GetAttackMap<kBishop>(king_square,
                                       board.get_piece_bitboard(color, kPawn))));
    AddFeature<T>(score, color, kKingVectorExposure + 1,
                  bitops::PopCount(magic::GetAttackMap<kRook>(king_square,
                                       board.get_piece_bitboard(color, kPawn))));
  }
  else {
    if (board.get_piece_bitboard(not_color, kBishop)) {
      AddFeature<T>(score, color, kKingVectorExposure,
                    bitops::PopCount(magic::GetAttackMap<kBishop>(king_square,
                                         board.get_piece_bitboard(color, kPawn))));
    }
    if (board.get_piece_bitboard(not_color, kRook)) {
      AddFeature<T>(score, color, kKingVectorExposure + 1,
                    bitops::PopCount(magic::GetAttackMap<kRook>(king_square,
                                         board.get_piece_bitboard(color, kPawn))));
    }
  }

  if (counter.king_attack_count >= 2) {
    int king_attack_scaling = kKingAttackerScaling[std::min(counter.king_attack_count, 6)];
    for (PieceType pt = kKnight - kKnight; pt <= kQueen - kKnight; pt++) {
      AddFeature<T>(score, color, kKingAttack
                                  + pt, king_attack_scaling * counter.king_zone_attacks[pt]);
    }
  }
}

template<typename T, Color color>
inline void ScorePieces(T &score, const Board &board, const EvalConstants &ec,
                        EvalCounter &counter) {
  const BitBoard enemy_king_zone = magic::GetKingArea(ec.king_squares[color ^ 0x1]);

  ScorePawns<T, color>(score, ec);
  ScoreKnights<T, color>(score, board, ec, counter, enemy_king_zone);
  ScoreBishops<T, color>(score, board, ec, counter, enemy_king_zone);
  ScoreRooks<T, color>(score, board, ec, counter, enemy_king_zone);
  ScoreQueens<T, color>(score, board, ec, counter, enemy_king_zone);
  ScoreKings<T, color>(score, board, ec, counter);
}

template<typename T>
inline void AddCommonFeatures(T &score, const Board &board) {
  AddFeature<T>(score, board.get_turn(), kTempoBonusIndex, 1);

  for (PieceType piece_type = kPawn; piece_type <= kQueen; piece_type++) {
    AddFeature<T>(score, kWhite, piece_type + kBaseValueIndex,
        board.get_piece_count(kWhite, piece_type) - board.get_piece_count(kBlack, piece_type));
  }

  AddFeature<T>(score, kWhite, kBishopPairIndex,
      (board.get_piece_count(kWhite, kBishop) / 2)
      - (board.get_piece_count(kBlack, kBishop)) / 2);
}

template<typename T>
inline void AddCommonFeatures(T &score, const Board &board, const EvalConstants &ec) {
  AddFeature<T>(score, kWhite, kPawn + kActivityBonusIndex,
      bitops::PopCount(ec.p_forward[kWhite]) - bitops::PopCount(ec.p_forward[kBlack]));

  AddFeature<T>(score, kWhite, kDoublePawnPenaltyIndex,
      bitops::PopCount(bitops::N(ec.p_fill_forward[kWhite]) & ec.pawn_bb[kWhite])
    - bitops::PopCount(bitops::S(ec.p_fill_forward[kBlack]) & ec.pawn_bb[kBlack]));

  AddFeature<T>(score, kWhite, kUnprotectedPieces,
                bitops::PopCount(ec.nbr_bitboard & ec.c_pieces[kWhite] & (~ec.controlled[kWhite]))
                -bitops::PopCount(ec.nbr_bitboard & ec.c_pieces[kBlack] & (~ec.controlled[kBlack])));
}

}

namespace evaluation {

// The main evaluation function. Templates are used so we can fill a feature list during training
// but an incrementally updated score during actual play.
template<typename T>
T ScoreBoard(const Board &board) {
  T score = init<T>();

  // Initialize general constants
  const EvalConstants ec(board);

  // Common features independent of specific individual piece placement.
  AddCommonFeatures<T>(score, board);
  AddCommonFeatures<T>(score, board, ec);

  // Piece evaluations
  std::array<EvalCounter, 2> check_counter;
  ScorePieces<T, kWhite>(score, board, ec, check_counter[kWhite]);
  ScorePieces<T, kBlack>(score, board, ec, check_counter[kBlack]);

  // Features picked up while iterating over pieces
  AddFeature<T>(score, kWhite, kSafeChecks, check_counter[kWhite].safe - check_counter[kBlack].safe);
  AddFeature<T>(score, kWhite, kUnSafeChecks, check_counter[kWhite].unsafe - check_counter[kBlack].unsafe);

  if (board.get_turn() == kWhite) {
    return score;
  }
  return -score;
}

Score ScoreBoard(const Board &board) {
  PScore score =  ScoreBoard<PScore>(board);
  Vec<double, settings::kNumClusters> weights =
      cluster_model->GetWeightedProbabilities(board);

  assert(std::abs(weights.sum() - 1.0) < 0.0001);
  Score eval_score = std::round(weights.dot(score));
  assert(!is_mate_score(eval_score));
  return eval_score;
}

template std::vector<double> ScoreBoard<std::vector<double> >(const Board &board);

void PrintFeatureValues(const Board &board) {
  std::vector<Score> features(ScoreBoard<std::vector<Score> >(board));
  size_t idx = 0;
  for (size_t i = 0; i < kNumFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    std::cout << features[i]
              << " <-- " << kFeatureInfos[idx].info << std::endl;
  }
}

void SaveGMMHardCode(std::string file_name) {
  cluster_model->SaveHardCode(file_name);
}

void SaveGMMVariables() {
  std::ofstream file(settings::kGMMParamsFile);
  std::ofstream description_file(settings::kParamExplanationFile);
  size_t idx = 0;
  for (size_t i = 0; i < kNumFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    for (size_t j = 0; j < settings::kNumClusters; j++) {
      file << eval_score_values[i][j] << " ";
      description_file  << eval_score_values[i][j] << " ";
    }
    file << std::endl;
    description_file << "<-- " << kFeatureInfos[idx].info << std::endl;
  }
  file.flush();
  file.close();
  description_file.flush();
  description_file.close();
}

void SaveGMMVariablesHardCode(std::string filename) {
  std::ofstream file(filename);
  file << "const std::array<int, (" << kNumFeatures << "*"
      << settings::kNumClusters << ")> eval_weights = {" << std::endl;

  size_t idx = 0;
  for (size_t i = 0; i < kNumFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    file << "    ";
    for (int j = 0; j < settings::kNumClusters; j++) {
      file  << eval_score_values[i][j];
      if (j+1 < settings::kNumClusters || i+1 < kNumFeatures) {
        file << ", ";
      }
      else {
        file << "  ";
      }
      int val = 5 - parse::CountChars(eval_score_values[i][j]);
      while (0 < val--) {
        file << " ";
      }
    }
    file << "// " << kFeatureInfos[idx].info << std::endl;
  }
  file << "};" << std::endl;
  file.flush();
  file.close();
}

void LoadMixturesHardCoded() {
  std::vector<double> params(hardcode::NFCM_params.begin(), hardcode::NFCM_params.end());
  cluster_model->LoadFromParams(params);
}

void LoadVariablesFromFile() {
  const int k = settings::kNumClusters;
  std::ifstream file(settings::kGMMParamsFile);
  for (size_t i = 0; i < kNumFeatures; i++) {
    for (size_t j = 0; j < k; j++) {
      file >> eval_score_values[i][j];
    }
  }
  file.close();
}

void LoadGMMVariablesHardCoded() {
  int c = 0;
  for (size_t i = 0; i < kNumFeatures; i++) {
    for (size_t j = 0; j < settings::kNumClusters; j++) {
      eval_score_values[i][j] = hardcode::eval_weights[c++];
    }
  }

  LoadMixturesHardCoded();
}

enum SGDVariantType {
  SGDNormal, SGDAdaDelta, SGDAdaGrad, SGDAdam, SGDRotating
};

template<typename T>
double Sigmoid(T x) {
  return 1 / ( 1 + std::exp(-x) );
}

enum LearningStyle {
  kSupervised, kTDL, kMixedLearning
};

void CheckVariableInfluence() {
  std::vector<double> variable_influences(kNumFeatures);
  std::vector<double> set_influence_abs_sum(kFeatureInfos.size());
  std::vector<double> set_influence_squared_sum(kFeatureInfos.size());
  Vec<double, settings::kNumClusters> mixture_proportions(0);
  std::vector<Game> games = data::LoadGames();
  if (games.size() == 0) {
    std::cout << "No games found!" << std::endl;
    return;
  }
  long num_positions = 0;
  for (size_t index = 0; index < games.size(); index++) {
    games[index].set_to_position_after(0);

    while (games[index].board.get_num_made_moves() < games[index].moves.size()) {
      games[index].forward();
      std::vector<int> features =
          ScoreBoard<std::vector<int> >(games[index].board);
      Vec<double, settings::kNumClusters> probabilities =
          cluster_model->GetWeightedProbabilities(games[index].board);
      double set_influence = 0;
      size_t idx = 0;
      for (size_t i = 0; i < kNumFeatures; i++) {
        if (i == kFeatureInfos[idx + 1].idx) {
          set_influence_squared_sum[idx] += set_influence * set_influence;
          set_influence_abs_sum[idx] += std::abs(set_influence);
          idx++;
          set_influence = 0;
        }
        double variable_influence = 0;
        for (size_t j = 0; j < settings::kNumClusters; j++) {
          variable_influence += eval_score_values[i][j] * features[i]
                                * probabilities[j];
          set_influence += eval_score_values[i][j] * features[i]
                                                          * probabilities[j];
        }
        variable_influences[i] += (variable_influence * variable_influence);
        if (i == kNumFeatures - 1) {
          set_influence_squared_sum[idx] += set_influence * set_influence;
          set_influence_abs_sum[idx] += std::abs(set_influence);
          set_influence = 0;
        }
      }
      mixture_proportions += probabilities;
      num_positions++;
    }
    if ((index + 1) % 1000 == 0) {
      std::cout << (index + 1) << " games evaluated" << std::endl;
    }
  }
  std::ofstream sfile("params/set_influence");
  for (size_t idx = 0; idx < kFeatureInfos.size(); idx++) {
    double mean = (set_influence_abs_sum[idx] / num_positions);
    sfile  << mean
        << " +- " << std::sqrt((set_influence_squared_sum[idx] / num_positions)- mean * mean)
        << " <-- " << kFeatureInfos[idx].info << std::endl;

  }
  sfile.flush();
  sfile.close();
  std::ofstream file("params/influence");
  size_t idx = 0;
  for (size_t i = 0; i < kNumFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    file  << (variable_influences[i] / num_positions)
        << " <-- " << kFeatureInfos[idx].info << std::endl;

  }
  file.flush();
  file.close();
}

void CheckFeatureMagnitude() {
  std::vector<Vec<double, settings::kNumClusters>> feature_magnitudes(kNumFeatures, 0);
  Vec<double, settings::kNumClusters> mixture_proportions(0);
  std::vector<Game> games = data::LoadGames();
  for (size_t index = 0; index < games.size(); index++) {
    games[index].set_to_position_after(0);

    while (games[index].board.get_num_made_moves() < games[index].moves.size()) {
      games[index].forward();
      if (false){
        games[index].board.Print();
      }
      std::vector<int> features =
          ScoreBoard<std::vector<int> >(games[index].board);
      Vec<double, settings::kNumClusters> probabilities = cluster_model->GetWeightedProbabilities(games[index].board);
      for (size_t i = 0; i < kNumFeatures; i++) {
        feature_magnitudes[i] += probabilities * std::abs(features[i]);
      }
      mixture_proportions += probabilities;
    }
    if ((index + 1) % 5000 == 0) {
      std::cout << (index + 1) << " games evaluated" << std::endl;
    }
  }
  std::ofstream file("params/magnitude");
  size_t idx = 0;
  for (size_t i = 0; i < kNumFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    for (int j = 0; j < settings::kNumClusters; j++) {
      file  << (feature_magnitudes[i][j] / mixture_proportions[j]) << " ";
    }
    file  << "<-- " << kFeatureInfos[idx].info << std::endl;

  }
  file.flush();
  file.close();
}

std::vector<double> GetQuantileBreakpoints() {
  std::vector<double> quantile_breakpoints;
  quantile_breakpoints.emplace_back(0.01);
  quantile_breakpoints.emplace_back(0.02);
  quantile_breakpoints.emplace_back(0.03);
  quantile_breakpoints.emplace_back(0.04);
  for (double d = 0.05; d <=0.501; d += 0.05) {
    quantile_breakpoints.emplace_back(d);
  }
  return quantile_breakpoints;
}

Score EvaluateQuietMoveValue() {
  std::vector<Game> games = data::LoadGames(1200000);
  const size_t max_depth = 10, n_score_bins = 40, score_bin_size = 200,
            n_dif_bins = 120, dif_bin_size = 40;
  std::vector<Vec<double, settings::kNumClusters> > x(max_depth, 0), x_squared(max_depth, 0), count(max_depth, 0);
  Vec<double, max_depth> n(0), xs2(0), xs(0);
  std::vector<Array2d<long, n_score_bins, n_dif_bins> > histogram(max_depth);
  for (size_t i = 0; i < max_depth; i++) {
    for (size_t j = 0; j < n_score_bins; j++) {
      for (size_t k = 0; k < n_dif_bins; k++) {
        histogram[i][j][k] = 0;
      }
    }
  }
  for (size_t i = 0; i < games.size(); i++) {
    Game game = games[i];
    game.set_to_position_after(0);
    std::vector<bool> quiet_move(game.moves.size());
    std::vector<bool> in_check(game.moves.size());
    std::vector<Vec<double, settings::kNumClusters> > mixture_probabilities(game.moves.size(), 0);
    std::vector<Score> scores(game.moves.size() + 1);
    std::vector<Score> qscores(game.moves.size() + 1);
    while (game.board.get_num_made_moves() < game.moves.size()) {
      int move_number = game.board.get_num_made_moves();
      const Move move = game.moves[game.board.get_num_made_moves()];
      if (GetMoveType(move) < kEnPassant
          && !game.board.GivesCheck(move)) {
        quiet_move[move_number] = true;
      }
      in_check[move_number] = game.board.InCheck();
      mixture_probabilities[move_number] = BoardMixtureProbability(game.board);
      if (move_number > 4) {
        scores[move_number] = ScoreBoard(game.board);
        qscores[move_number] = search::QSearch(game.board);
      }
      game.forward();
    }
    //scores[scores.size() - 1] = ScoreBoard(game.board);
    //qscores[scores.size() - 1] = search::QSearch(game.board);

    for (size_t depth = 0; depth < max_depth; depth++) {
      for (size_t i = 6; i < quiet_move.size() - depth - 2; i++) {
        if (!in_check[i]) {
          size_t score_bin_idx = scores[i];
          score_bin_idx += score_bin_size / 2;
          score_bin_idx /= score_bin_size;
          score_bin_idx += n_score_bins / 2;
          score_bin_idx = std::max(0, (int)score_bin_idx);
          score_bin_idx = std::min(n_score_bins - 1, score_bin_idx);

          Score dif = (depth % 2) ? qscores[i+1+depth] - scores[i]
                                  : -qscores[i+1+depth] - scores[i];
          count[depth] += mixture_probabilities[i];
          x[depth] += mixture_probabilities[i] * dif;
          x_squared[depth] += mixture_probabilities[i] * dif * dif;
          n[depth]++;
          xs[depth] += dif;
          xs2[depth] += dif * dif;
          dif /= dif_bin_size;
          dif += n_dif_bins / 2;
          size_t dif_bin = std::max(0, dif);
          dif_bin = std::min(n_dif_bins - 1, dif_bin);
          histogram[depth][score_bin_idx][dif_bin]++;
        }
      }
    }
    if ((i + 1) % 10000 == 0) {
      std::cout << "Evaluated " << (i+1) << " games!" << std::endl;
    }
  }
  std::cout << "Mixture Based Statistics" << std::endl;
  for (size_t depth = 0; depth < max_depth; depth++) {
    Vec<double, settings::kNumClusters> x_mean = x[depth] / count[depth];
    Vec<double, settings::kNumClusters> variance =
        (x_squared[depth] - (x_mean * x_mean * count[depth])) / count[depth];
    std::cout << "Depth: " << (depth + 1) << " Means: ";
    x_mean.print();
    std::cout << " Sigmas: ";
    variance.sqrt().print();
    std::cout << " Counts: ";
    count[depth].print();
    std::cout << std::endl;
  }
  std::cout << "Non Mixture Based Statistics" << std::endl;
  for (size_t depth = 0; depth < max_depth; depth++) {
    double x_mean = xs[depth] / n[depth];
    double variance =
        (xs2[depth] - (x_mean * x_mean * n[depth])) / n[depth];
    std::cout << "Depth: " << (depth + 1) << " Means: " << x_mean
              << " Sigmas: " << std::sqrt(variance) << " Counts: "
              << n[depth] << std::endl;
  }
  std::cout << "Saving d1 Histogram" << std::endl;
  std::vector<std::vector <double> > vhistogram(n_score_bins, std::vector<double>(n_dif_bins + 1));
  std::vector<std::vector <double> > csum(n_score_bins, std::vector<double>(n_dif_bins));
  std::vector<double> quantile_breakpoints;
  for (double d = 0.05; d <=0.96; d += 0.05) {
    quantile_breakpoints.emplace_back(d);
  }
  std::vector<std::vector <int> > quantiles(n_score_bins,
                                            std::vector<int>(quantile_breakpoints.size() + 1));
  for (size_t i = 0; i < n_score_bins; i++) {
    vhistogram[i][0] = (i - (n_score_bins / 2)) * score_bin_size;
    quantiles[i][0] = (i - (n_score_bins / 2)) * score_bin_size;
  }
  for (size_t depth = 0; depth < max_depth; depth++) {
    for (size_t i = 0; i < n_score_bins; i++) {
      std::cout << i << ": ";
      long sum = 0;
      for (size_t j = 0; j < n_dif_bins; j++) {
        vhistogram[i][j + 1] = histogram[depth][i][j];
        sum += histogram[depth][i][j];
      }
      for (size_t j = 0; j < n_dif_bins; j++) {
        vhistogram[i][j + 1] /= sum;
      }
      csum[i][0] = vhistogram[i][0 + 1];
      for (size_t j = 1; j < n_dif_bins; j++) {
        csum[i][j] = csum[i][j-1] + vhistogram[i][j + 1];
      }
      std::cout << i << ",0 = " << csum[i][0] << "|" << vhistogram[i][0 + 1] << " ";
      for (size_t j = 1; j < n_dif_bins; j++) {
        std::cout << i << "," << j << " = " << csum[i][j] << "|" << vhistogram[i][j + 1] << " ";
        for (size_t idx = 0; idx < quantile_breakpoints.size(); idx++) {
          if (csum[i][j-1] < quantile_breakpoints[idx]
              && csum[i][j] >= quantile_breakpoints[idx]) {
            quantiles[i][idx + 1] = ( (j - n_dif_bins/2) * dif_bin_size);
          }
        }
      }
      std::cout << std::endl;
    }
    parse::Save2dVecToCSV<double>(vhistogram, "data/hist" + std::to_string(depth + 1) + ".csv");
    parse::Save2dVecToCSV<int>(quantiles, "data/quant" + std::to_string(depth + 1) + ".csv");
  }
  return 0;
}

Vec<double, settings::kNumClusters> BoardMixtureProbability(const Board &board) {
  return cluster_model->GetWeightedProbabilities(board);
}

Score GetPawnBaseValue(const Board &board) {
  PScore score = eval_score_values[kBaseValueIndex + kPawn];
  Vec<double, settings::kNumClusters> weights =
      cluster_model->GetWeightedProbabilities(board);
  return std::round(weights.dot(score));
}

Score GetTempoValue(const Board &board) {
  PScore score = eval_score_values[kTempoBonusIndex];
  Vec<double, settings::kNumClusters> weights =
      cluster_model->GetWeightedProbabilities(board);
  return std::round(weights.dot(score));
}

int ScoreToCentipawn(const Score score, const Board &board) {
  return (100 * score) / GetPawnBaseValue(board);
}

std::vector<PScore> GetEvaluationWeights() {
  return eval_score_values;
}

Score GetScore(const size_t feature_idx, const size_t cluster_idx) {
  return eval_score_values[feature_idx][cluster_idx];
}

void SetScore(const size_t feature_idx, const size_t cluster_idx, const Score score) {
  eval_score_values[feature_idx][cluster_idx] = score;
}

void SetModel(cluster::ClusterModel<settings::kNumClusters>* model) {
  cluster_model->SetModel(model);
}

}

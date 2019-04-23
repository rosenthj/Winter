/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2016 Jonas Kuratli, Jonathan Maurer, Jonathan Rosenthal
 *  Copyright (C) 2017-2018 Jonathan Rosenthal
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
const double eval_scaling = 1024.0;

const BitBoard fourth_row = parse::StringToBitBoard("a4") | parse::StringToBitBoard("b4")
                      | parse::StringToBitBoard("c4") | parse::StringToBitBoard("d4")
                      | parse::StringToBitBoard("e4") | parse::StringToBitBoard("f4")
                      | parse::StringToBitBoard("g4") | parse::StringToBitBoard("h4");

const BitBoard fifth_row = fourth_row << 8;

struct TrainingPosition{
  Board board;
  double target;
};

int kSign[2] = {1,-1};

std::vector<PScore> eval_score_values(kNumFeatures, 0);

cluster::ClusterModel<settings::kNumClusters>* init_cluster_model() {
  if (settings::kClusterModelType == settings::kClusterNFCM) {
    return new cluster::NormFuzzyCMeans<settings::kNumClusters, kPhaseVecLength>();
  }
  return new cluster::GaussianMixtureModel<settings::kNumClusters, kPhaseVecLength>();
}

cluster::ClusterModel<settings::kNumClusters>* cluster_model = init_cluster_model();

const Array2d<BitBoard, 2, 64> get_king_pawn_coverage() {
  const BitBoard second_row = parse::StringToBitBoard("a2") | parse::StringToBitBoard("b2")
                        | parse::StringToBitBoard("c2") | parse::StringToBitBoard("d2")
                        | parse::StringToBitBoard("e2") | parse::StringToBitBoard("f2")
                        | parse::StringToBitBoard("g2") | parse::StringToBitBoard("h2");

  const BitBoard seventh_row = second_row << (8 * 5);
  Array2d<BitBoard, 2, 64> coverage;
  for (Square square = 0; square < 64; square++) {
    BitBoard bb = GetSquareBitBoard(square);
    int i = GetSquareY(square);
    while (i) {
      bb |= bitops::E(bb) | bitops::W(bb);
      i--;
    }
    bb = bitops::FillNorthEast(bb, ~0) | bitops::FillNorthWest(bb, ~0);
    bb = bitops::FillNorth(bb, ~0);
    bb &= ~seventh_row | bitops::N(bb);
    coverage[kWhite][square] = bb;
    bb = GetSquareBitBoard(square);
    i = 7 - GetSquareY(square);
    while (i) {
      bb |= bitops::E(bb) | bitops::W(bb);
      i--;
    }
    bb = bitops::FillSouthEast(bb, ~0) | bitops::FillSouthWest(bb, ~0);
    bb = bitops::FillSouth(bb, ~0);
    bb &= ~second_row | bitops::S(bb);
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

}

namespace evaluation {

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

// The main evaluation function. Templates are used so we can fill a feature list during training
// but an incrementally updated score during actual play.
template<typename T>
T ScoreBoard(const Board &board) {
  T score = init<T>();

  // Initialize general constants
  const Square king_squares[2] = {
      bitops::NumberOfTrailingZeros(board.get_piece_bitboard(kWhite, kKing)),
      bitops::NumberOfTrailingZeros(board.get_piece_bitboard(kBlack, kKing))
  };

  BitBoard all_major_pieces = 0;
  for (PieceType piece = kRook; piece <= kKing; piece++) {
    all_major_pieces |= board.get_piecetype_bitboard(piece);
  }
  const BitBoard major_pieces[2] = {all_major_pieces & board.get_color_bitboard(kWhite),
                              all_major_pieces & board.get_color_bitboard(kBlack)};
  const BitBoard c_pieces[2] = {board.get_color_bitboard(kWhite), board.get_color_bitboard(kBlack)};

  const BitBoard all_pieces = board.get_color_bitboard(kWhite) | board.get_color_bitboard(kBlack);
  const BitBoard empty = ~all_pieces;
  const BitBoard pawn_bb[2] = {
      board.get_piece_bitboard(kWhite, kPawn),
      board.get_piece_bitboard(kBlack, kPawn)
  };
  const BitBoard all_pawns_bb = pawn_bb[kWhite] | pawn_bb[kBlack];

  // Initialize pawn structure based constants.
  // These are needed later on for piece activity calculations.
  const BitBoard covered_once[2] = {
      bitops::NE(pawn_bb[kWhite]) | bitops::NW(pawn_bb[kWhite]),
      bitops::SE(pawn_bb[kBlack]) | bitops::SW(pawn_bb[kBlack])
  };

  const BitBoard covered_potentially[2] = {
      bitops::FillNorth(covered_once[kWhite], ~0),
      bitops::FillSouth(covered_once[kBlack], ~0)
  };

  const BitBoard covered_twice[2] = {
      bitops::NE(pawn_bb[kWhite]) & bitops::NW(pawn_bb[kWhite]),
      bitops::SE(pawn_bb[kBlack]) & bitops::SW(pawn_bb[kBlack])
  };
  const BitBoard hard_block[2] = {
      (pawn_bb[kWhite] & ~(bitops::S(empty) | bitops::SE(c_pieces[kBlack])
                                           | bitops::SW(c_pieces[kBlack])))
      | (pawn_bb[kBlack] & ((covered_once[kBlack] & ~covered_once[kWhite])
                            |(covered_twice[kBlack] & ~covered_twice[kWhite]))),
      (pawn_bb[kBlack] & ~(bitops::N(empty) | bitops::NE(c_pieces[kWhite])
                                           | bitops::NW(c_pieces[kWhite])))
      | (pawn_bb[kWhite] & ((covered_once[kWhite] & ~covered_once[kBlack])
                            |(covered_twice[kWhite] & ~covered_twice[kBlack]))),
  };

  BitBoard p_forward[2] = {
      bitops::N(pawn_bb[kWhite]) & empty,
      bitops::S(pawn_bb[kBlack]) & empty
  };

  p_forward[kWhite] |= bitops::N(p_forward[kWhite]) & empty & fourth_row;
  p_forward[kBlack] |= bitops::S(p_forward[kBlack]) & empty & fifth_row;

  AddFeature<T>(score, kWhite, kPawn + kActivityBonusIndex,
      bitops::PopCount(p_forward[kWhite]) - bitops::PopCount(p_forward[kBlack]));

  for (PieceType piece_type = kPawn; piece_type <= kQueen; piece_type++) {
    AddFeature<T>(score, kWhite, piece_type + kBaseValueIndex,
        board.get_piece_count(kWhite, piece_type) - board.get_piece_count(kBlack, piece_type));
  }

  AddFeature<T>(score, kWhite, kBishopPairIndex,
      (board.get_piece_count(kWhite, kBishop) / 2)
      - (board.get_piece_count(kBlack, kBishop)) / 2);


  BitBoard s_filled = bitops::FillSouth(board.get_piece_bitboard(kBlack, kPawn), ~0);
  BitBoard n_filled = bitops::FillNorth(board.get_piece_bitboard(kWhite, kPawn), ~0);

  const BitBoard opposed_pawns = (s_filled & pawn_bb[kWhite])|(n_filled & pawn_bb[kBlack]);
  const BitBoard neighbored_pawns[2] = {
      (bitops::E(pawn_bb[kWhite]) | bitops::W(pawn_bb[kWhite])) & pawn_bb[kWhite],
      (bitops::E(pawn_bb[kBlack]) | bitops::W(pawn_bb[kBlack])) & pawn_bb[kBlack]
  };

  AddFeature<T>(score, kWhite, kDoublePawnPenaltyIndex,
      bitops::PopCount(bitops::N(n_filled) & board.get_piece_bitboard(kWhite, kPawn))
    - bitops::PopCount(bitops::S(s_filled) & board.get_piece_bitboard(kBlack, kPawn)));


  s_filled |= bitops::SW(s_filled) | bitops::SE(s_filled);
  n_filled |= bitops::NW(n_filled) | bitops::NE(n_filled);
  BitBoard passed[2] = {
      pawn_bb[kWhite] & (~(s_filled | bitops::S(bitops::FillSouth(pawn_bb[kWhite], ~0)))),
      pawn_bb[kBlack] & (~(n_filled | bitops::N(bitops::FillNorth(pawn_bb[kBlack], ~0))))
  };

//  AddFeature<T>(score, kWhite, kPassedPawnUnblocked,
//                bitops::PopCount(passed[kWhite] & bitops::S(empty))
//              - bitops::PopCount(passed[kBlack] & bitops::N(empty)));

//  BitBoard behind_passed[2] = {bitops::FillSouth(passed[kWhite], ~(pawn_bb[kWhite] | pawn_bb[kBlack])),
//                            bitops::FillNorth(passed[kBlack], ~(pawn_bb[kWhite] | pawn_bb[kBlack]))};
//  behind_passed[kWhite] &= board.get_piecetype_bitboard(kRook);
//  behind_passed[kBlack] &= board.get_piecetype_bitboard(kRook);
//  AddFeature<T>(score, kWhite, kRookBehindPasser,
//                bitops::PopCount(c_pieces[kWhite] & behind_passed[kWhite])
//                -bitops::PopCount(c_pieces[kBlack] & behind_passed[kBlack]));

  const BitBoard controlled[2] = {
      board.PlayerBitBoardControl(kWhite, all_pieces),
      board.PlayerBitBoardControl(kBlack, all_pieces)
  };

  const BitBoard nbr_bitboard = board.get_piecetype_bitboard(kKnight)
                     | board.get_piecetype_bitboard(kBishop)
                     | board.get_piecetype_bitboard(kRook);
  AddFeature<T>(score, kWhite, kUnprotectedPieces,
                bitops::PopCount(nbr_bitboard & c_pieces[kWhite] & (~controlled[kWhite]))
                -bitops::PopCount(nbr_bitboard & c_pieces[kBlack] & (~controlled[kBlack])));

  BitBoard checking_squares[2][4] = { {0, 0, 0, 0}, {0, 0, 0, 0} };
  BitBoard safe_checking_squares[2][4] = { {0, 0, 0, 0}, {0, 0, 0, 0} };
  for (Color color = kWhite; color <= kBlack; color++) {
    Color not_color = color ^ 0x1;
    Square enemy_king = king_squares[not_color];
    checking_squares[color][kKnight - kKnight] = magic::GetAttackMap<kKnight>(enemy_king, all_pieces);
    checking_squares[color][kBishop - kKnight] = magic::GetAttackMap<kBishop>(enemy_king, all_pieces);
    checking_squares[color][kRook - kKnight] = magic::GetAttackMap<kRook>(enemy_king, all_pieces);
    checking_squares[color][kQueen - kKnight] = checking_squares[color][kRook - kKnight]
                                               | checking_squares[color][kBishop - kKnight];
    BitBoard safe = ~(c_pieces[color] | controlled[not_color]);
    for (PieceType pt = kKnight - kKnight; pt <= kQueen - kKnight; pt++) {
      safe_checking_squares[color][pt] = safe & checking_squares[color][pt];
      checking_squares[color][pt] &= ~(c_pieces[color]);
      checking_squares[color][pt] ^= safe_checking_squares[color][pt];
    }
  }
  int unsafe_checks[2] = {0, 0};
  int safe_checks[2] = {0, 0};

  // Piece evaluations
  for (Color color = kWhite; color <= kBlack; color++) {
    // Init color specific constants
    const Color not_color = color ^ 0x1;
    const Square enemy_king_square = king_squares[not_color];
    const BitBoard enemy_king_zone = magic::GetKingArea(enemy_king_square);
    int king_attack_count = 0;

//    // Evaluate passed pawns
//    BitBoard covered_passed_pawns = passed[color] & king_pawn_coverage[not_color][enemy_king_square];
//    while (covered_passed_pawns) {
//      Square pawn_square = bitops::NumberOfTrailingZeros(covered_passed_pawns);
//      int pawn_rank = color * 7 + kSign[color] * GetSquareY(pawn_square) - 1;
//      AddFeature<T>(score, color, kPassedPawnBonusIndex + pawn_rank, 1);
//      bitops::PopLSB(covered_passed_pawns);
//    }
//    BitBoard uncovered_passed = passed[color] & ~king_pawn_coverage[not_color][enemy_king_square];
//    while (uncovered_passed) {
//      Square pawn_square = bitops::NumberOfTrailingZeros(uncovered_passed);
//      int pawn_rank = color * 7 + kSign[color] * GetSquareY(pawn_square) - 1;
//      AddFeature<T>(score, color, kPassedPawnBonusIndex + 6 + pawn_rank, 1);
//      bitops::PopLSB(uncovered_passed);
//    }

    BitBoard not_isolated = bitops::E(pawn_bb[color]) | bitops::W(pawn_bb[color]);
    not_isolated = bitops::FillNorth(not_isolated, ~0);
    not_isolated = bitops::FillSouth(not_isolated, ~0);
    AddFeature<T>(score, color, kIsolatedPawnIndex, bitops::PopCount(pawn_bb[color] & ~not_isolated));

    BitBoard categorized_pawns[4];
    categorized_pawns[0] = pawn_bb[color] & opposed_pawns;
    categorized_pawns[1] = pawn_bb[color] & ~(opposed_pawns | passed[color]);
    categorized_pawns[2] = passed[color] & king_pawn_coverage[not_color][enemy_king_square];
    categorized_pawns[3] = passed[color] & ~king_pawn_coverage[not_color][enemy_king_square];
    for (int cat = 0; cat < 4; cat++) {
      BitBoard cat_pawns = categorized_pawns[cat];
      int cat_offset = 24 * cat;
      while (cat_pawns) {
        Square pawn_square = bitops::NumberOfTrailingZeros(cat_pawns);
        BitBoard pawn = bitops::GetLSB(cat_pawns);
        bool is_neighbored = pawn & neighbored_pawns[color];
        bool is_protected = pawn & covered_once[color];
        int pawn_rank = color * 7 + kSign[color] * GetSquareY(pawn_square) - 1;
        AddFeature<T>(score, color, kPawnEvalIndex + cat_offset + 12 * is_neighbored
                                                   + 6 * is_protected + pawn_rank, 1);
        bitops::PopLSB(cat_pawns);
      }
    }

    // Pawn attacks
    BitBoard targets = covered_once[color] & (c_pieces[not_color] ^ pawn_bb[not_color]);
    AddFeature<T>(score, color, kPawnAttackIndex, bitops::PopCount(targets));

    Vec<int, kQueen - kPawn> king_zone_attacks(0);

    // Evaluate Knights
    BitBoard pieces = board.get_piece_bitboard(color, kKnight);
    while (pieces) {
      Square piece_square = bitops::NumberOfTrailingZeros(pieces);
      AddFeature<T>(score, color, kKnightPSTIndex + kPSTindex[piece_square], 1);
      AddFeature<T>(score, color, kKingAttackDistance + kKnight - 1,
          magic::GetSquareDistance(piece_square, king_squares[not_color]));
//      int relative_x = GetSquareX(piece_square) - GetSquareX(enemy_king_square) + 7;
//      int relative_y = GetSquareY(piece_square) - GetSquareY(enemy_king_square) + 7;
//      AddFeature<T>(score, color, kKnightVsKingPosition + relative_king_map[relative_x + 15 * relative_y], 1);
      BitBoard attack_map = magic::GetAttackMap<kKnight>(piece_square, all_pieces);
      unsafe_checks[color] += bitops::PopCount(attack_map
                                               & checking_squares[color][kKnight-kKnight]);
      safe_checks[color] += bitops::PopCount(attack_map
                                             & safe_checking_squares[color][kKnight-kKnight]);
      AddFeature<T>(score, color, kKnightSquaresIndex +
                    bitops::PopCount(attack_map & ~(c_pieces[color] | covered_once[not_color])), 1);
      AddFeature<T>(score, color, kMinorAttackIndex,
          bitops::PopCount(attack_map & major_pieces[not_color]));
      if (attack_map & enemy_king_zone) {
        king_attack_count++;
        king_zone_attacks[kKnight - kKnight] ++;//= bitops::PopCount(attack_map & enemy_king_zone);
      }
      bitops::PopLSB(pieces);
    }

    // Evaluate Bishops
    pieces = board.get_piece_bitboard(color, kBishop);
    BitBoard bishop_targets = 0;
    BitBoard abstract_targets = 0;
    while (pieces) {
      Square piece_square = bitops::NumberOfTrailingZeros(pieces);
//      AddFeature<T>(score, color, kKingAttackDistance + kBishop - 1,
//          magic::GetSquareDistance(piece_square, king_squares[not_color]));
      int relative_x = GetSquareX(piece_square) - GetSquareX(enemy_king_square) + 7;
      int relative_y = GetSquareY(piece_square) - GetSquareY(enemy_king_square) + 7;
      AddFeature<T>(score, color, kBishopVsKingPosition + relative_king_map[relative_x + 15 * relative_y], 1);
      BitBoard attack_map = magic::GetAttackMap<kBishop>(piece_square, all_pieces)
          & (~covered_once[not_color] | (c_pieces[not_color] ^ pawn_bb[not_color]));
      unsafe_checks[color] += bitops::PopCount(attack_map
                                             & checking_squares[color][kBishop-kKnight]);
      safe_checks[color] += bitops::PopCount(attack_map
                                             & safe_checking_squares[color][kBishop-kKnight]);
      bishop_targets |= attack_map;
      if (attack_map & enemy_king_zone) {
        king_attack_count++;
        king_zone_attacks[kBishop - kKnight]++;
      }
      AddFeature<T>(score, color, kBishopMobility
                    + bitops::PopCount(attack_map & ~c_pieces[color]), 1);
      abstract_targets |= magic::GetAttackMap<kBishop>(piece_square, hard_block[color]);
      bitops::PopLSB(pieces);
    }
    bishop_targets &= ~c_pieces[color];
    AddFeature<T>(score, color, kMinorAttackIndex,
        bitops::PopCount(bishop_targets & major_pieces[not_color]));
    abstract_targets &= ~bishop_targets;
    AddFeature<T>(score, color, kBishop + kAbstractActivityIndex,
        bitops::PopCount(abstract_targets));

    // Evaluate Rooks
    pieces = board.get_piece_bitboard(color, kRook);
    while (pieces) {
      Square piece_square = bitops::NumberOfTrailingZeros(pieces);

      BitBoard file = magic::GetSquareFile(piece_square);
      if (!(file & all_pawns_bb)) {
        AddFeature<T>(score, color, kRookOpenFile, 1);
      }

      AddFeature<T>(score, color, kKingAttackDistance + kRook - 1,
          magic::GetSquareDistance(piece_square, king_squares[not_color]));
      BitBoard attack_map = ~c_pieces[color]
          & (~covered_once[not_color] | (c_pieces[not_color] ^ pawn_bb[not_color]))
          & magic::GetAttackMap<kRook>(piece_square, all_pieces);
      unsafe_checks[color] += bitops::PopCount(attack_map
                                             & checking_squares[color][kRook-kKnight]);
      safe_checks[color] += bitops::PopCount(attack_map
                                             & safe_checking_squares[color][kRook-kKnight]);
      if (attack_map & enemy_king_zone) {
        king_attack_count++;
        king_zone_attacks[kRook - kKnight]++;
      }
      BitBoard abstract_attack = attack_map ^
          magic::GetAttackMap<kRook>(piece_square, hard_block[color]);
      AddFeature<T>(score, color, kRookMobility + bitops::PopCount(attack_map), 1);
      AddFeature<T>(score, color, kRook + kAbstractActivityIndex,
          bitops::PopCount(abstract_attack));
      bitops::PopLSB(pieces);
    }

    // Evaluate Queens
    pieces = board.get_piece_bitboard(color, kQueen);
    while (pieces) {
      Square piece_square = bitops::NumberOfTrailingZeros(pieces);
      AddFeature<T>(score, color, kKingAttackDistance + kQueen - 1,
          magic::GetSquareDistance(piece_square, king_squares[not_color]));
      BitBoard attack_map = ~c_pieces[color]
          & (~covered_once[not_color] | (c_pieces[not_color] ^ pawn_bb[not_color]))
          & magic::GetAttackMap<kQueen>(piece_square, all_pieces);
      unsafe_checks[color] += bitops::PopCount(attack_map
                                             & checking_squares[color][kQueen-kKnight]);
      safe_checks[color] += bitops::PopCount(attack_map
                                             & safe_checking_squares[color][kQueen-kKnight]);
      if (attack_map & enemy_king_zone) {
        king_attack_count++;
        king_zone_attacks[kQueen - kKnight]++;
      }
      AddFeature<T>(score, color, kQueenMobility + bitops::PopCount(attack_map), 1);
      if (evaluation_features::kUseQueenActivity) {
        BitBoard abstract_attack = attack_map ^
            magic::GetAttackMap<kQueen>(piece_square, hard_block[color]);
        AddFeature<T>(score, color, kQueen + kActivityBonusIndex,
                      bitops::PopCount(attack_map));
        AddFeature<T>(score, color, kQueen + kAbstractActivityIndex,
                      bitops::PopCount(abstract_attack));
      }
      bitops::PopLSB(pieces);
    }

    // Evaluate King
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

    if (king_attack_count >= 2) {
      int king_attack_scaling = kKingAttackerScaling[std::min(king_attack_count, 6)];
      for (PieceType pt = kKnight - kKnight; pt <= kQueen - kKnight; pt++) {
        AddFeature<T>(score, color, kKingAttack + pt, king_attack_scaling * king_zone_attacks[pt]);
      }
    }

    AddFeature<T>(score, color, kKnightOutpost,
        bitops::PopCount(board.get_piece_bitboard(color, kKnight)
            & covered_once[color]
            & (~covered_potentially[not_color])));
  }

  AddFeature<T>(score, kWhite, kSafeChecks, safe_checks[kWhite] - safe_checks[kBlack]);
  AddFeature<T>(score, kWhite, kUnSafeChecks, unsafe_checks[kWhite] - unsafe_checks[kBlack]);

  AddFeature<T>(score, board.get_turn(), kTempoBonusIndex, 1);

  if (board.get_turn() == kWhite) {
    return score;
  }
  return -score;
}

Score ScoreBoard(const Board &board) {
  PScore score =  ScoreBoard<PScore>(board);
  Vec<double, settings::kNumClusters> weights =
      cluster_model->GetWeightedProbabilities(board);
//      gmm_main.GetWeightedProbabilities(GetBoardPhaseVec(board));

  assert(std::abs(weights.sum() - 1.0) < 0.0001);
  Score eval_score = std::round(weights.dot(score));
  assert(!is_mate_score(eval_score));
  return eval_score;
}

template std::vector<double> ScoreBoard<std::vector<double> >(const Board &board);

void PrintFeatureValues(const Board &board) {
  std::vector<int> features(ScoreBoard<std::vector<int> >(board));
  size_t idx = 0;
  for (size_t i = 0; i < kNumFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    std::cout << features[i]
              << " <-- " << kFeatureInfos[idx].info << std::endl;
  }
}

template<int k>
void SaveGMM(cluster::GaussianMixtureModel<k,kPhaseVecLength> &gmm, std::string file_name) {
  std::ofstream file(file_name);
  for (size_t m = 0; m < k; m++) {
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      file << gmm.mixtures[m].mu[i] << " ";
    }
    file << gmm.weights[m] << std::endl;
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      for (size_t j = 0; j < kPhaseVecLength; j++) {
        file << gmm.mixtures[m].sigma[i][j] << " ";
      }
      file << std::endl;
    }
  }
  file.flush();
  file.close();
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
  if (settings::kClusterModelType == settings::kClusterNFCM) {
    std::vector<double> params(hardcode::NFCM_params.begin(), hardcode::NFCM_params.end());
    cluster_model->LoadFromParams(params);
  }
  else {
    std::vector<double> params(hardcode::gmm_components.begin(), hardcode::gmm_components.end());
    cluster_model->LoadFromParams(params);
  }
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
  const int k = settings::kNumClusters;
  int c = 0;
  for (size_t i = 0; i < kNumFeatures; i++) {
    for (size_t j = 0; j < k; j++) {
      if (settings::kClusterModelType == settings::kClusterNFCM) {
        eval_score_values[i][j] = hardcode::eval_weights[c++];
      }
      else {
        eval_score_values[i][j] = hardcode::eval_weights_gmm[c++];
      }
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

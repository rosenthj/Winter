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
 *      Author: Jonathan
 */

#include "evaluation.h"
#include "learning/linear_algebra.h"
#include "learning/statistics.h"
#include "benchmark.h"
#include "general/feature_indexes.h"
#include "general/magic.h"
#include "general/settings.h"
#include "general/bit_operations.h"
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

typedef Vec<Score,settings::kGMMk> PScore;

const BitBoard fourth_row = parse::StringToBitBoard("a4") | parse::StringToBitBoard("b4")
                      | parse::StringToBitBoard("c4") | parse::StringToBitBoard("d4")
                      | parse::StringToBitBoard("e4") | parse::StringToBitBoard("f4")
                      | parse::StringToBitBoard("g4") | parse::StringToBitBoard("h4");

const BitBoard fifth_row = fourth_row << 8;

Vec<double,kPhaseVecLength> GetBoardPhaseVec(const Board &board) {
  Vec<double,kPhaseVecLength> vec;
  for (PieceType pt = kPawn; pt <= kQueen; pt++) {
    vec[0] += std::abs(board.get_piece_count(kWhite, pt) - board.get_piece_count(kBlack, pt));
  }
  for (PieceType pt = kKnight; pt <= kQueen; pt++) {
    vec[pt] += board.get_piecetype_count(pt);
  }
  Square wKing = bitops::NumberOfTrailingZeros(board.get_piece_bitboard(kWhite, kKing));
  Square bKing = bitops::NumberOfTrailingZeros(board.get_piece_bitboard(kBlack, kKing));
  vec[5] = std::abs(GetSquareX(wKing) - GetSquareX(bKing)) +
      std::abs(GetSquareY(wKing) - GetSquareY(bKing));
  return vec;
}

struct TrainingPosition{
  Board board;
  double target;
};

int kSign[2] = {1,-1};

int sign(double x) {
  return (x > 0) - (x < 0);
}

std::vector<PScore> feature_GMM_score_values(kNumFeatures);
GMM<settings::kGMMk, kPhaseVecLength> gmm_main;
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

template<> PScore init<PScore>() { return PScore(); }

template<typename T> inline
void AddFeature(T &s,const Color color, const int index, const int value) {
  s[index] += kSign[color] * value;
}

template<> inline void AddFeature<PScore>(PScore &s, const Color color, const int index,
    const int value) {
  s.FMA(feature_GMM_score_values[index], kSign[color] * value);
}

template<typename T>
T ScoreBoard(const Board &board) {
  T score = init<T>();

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


  AddFeature<T>(score, kWhite, kDoublePawnPenaltyIndex,
      bitops::PopCount(bitops::N(n_filled) & board.get_piece_bitboard(kWhite, kPawn))
    - bitops::PopCount(bitops::S(s_filled) & board.get_piece_bitboard(kBlack, kPawn)));


  s_filled |= bitops::SW(s_filled) | bitops::SE(s_filled);
  n_filled |= bitops::NW(n_filled) | bitops::NE(n_filled);
  BitBoard passed[2] = {
      board.get_piece_bitboard(kWhite, kPawn) & (~s_filled),
      board.get_piece_bitboard(kBlack, kPawn) & (~n_filled)
  };

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
    Square enemy_king = bitops::NumberOfTrailingZeros(
        board.get_piece_bitboard(not_color,kKing));
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

  for (Color color = kWhite; color <= kBlack; color++) {
    Color not_color = color ^ 0x1;
    Square enemy_king_square = bitops::NumberOfTrailingZeros(
        board.get_piece_bitboard(not_color,kKing));
    BitBoard enemy_king_zone = magic::GetKingArea(enemy_king_square);
    int king_attack_count = 0;

    BitBoard passed_pawns = passed[color];
    while (passed_pawns) {
      Square pawn_square = bitops::NumberOfTrailingZeros(passed_pawns);
      int pawn_rank = color * 7 + kSign[color] * GetSquareY(pawn_square) - 1;
      AddFeature<T>(score, color, kPassedPawnBonusIndex + pawn_rank, 1);
      bitops::PopLSB(passed_pawns);
    }

    BitBoard targets = covered_once[color] & (c_pieces[not_color] ^ pawn_bb[not_color]);
    AddFeature<T>(score, color, kPawnAttackIndex, bitops::PopCount(targets));

    Vec<int, kQueen - kPawn> king_zone_attacks;

    BitBoard pieces = board.get_piece_bitboard(color, kKnight);
    while (pieces) {
      Square piece_square = bitops::NumberOfTrailingZeros(pieces);
      AddFeature<T>(score, color, kKnightPSTIndex + kPSTindex[piece_square], 1);
      AddFeature<T>(score, color, kKingAttackDistance + kKnight - 1,
          magic::GetSquareDistance(piece_square, king_squares[not_color]));
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

    pieces = board.get_piece_bitboard(color, kBishop);
    BitBoard bishop_targets = 0;
    BitBoard abstract_targets = 0;
    while (pieces) {
      Square piece_square = bitops::NumberOfTrailingZeros(pieces);
      AddFeature<T>(score, color, kKingAttackDistance + kBishop - 1,
          magic::GetSquareDistance(piece_square, king_squares[not_color]));
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

    pieces = board.get_piece_bitboard(color, kRook);
    while (pieces) {
      Square piece_square = bitops::NumberOfTrailingZeros(pieces);
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
  Vec<double, settings::kGMMk> weights =
      gmm_main.GetWeightedProbabilities(GetBoardPhaseVec(board));
  assert(std::abs(weights.sum() - 1.0) < 0.0001);
  return std::round(weights.dot(score));
}

void PrintFeatureValues(const Board &board) {
  std::vector<int> features(ScoreBoard<std::vector<int> >(board));
  int idx = 0;
  for (int i = 0; i < kNumFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    std::cout << features[i]
              << " <-- " << kFeatureInfos[idx].info << std::endl;
  }
}

void SetGameToRandomQuiescent(Game &game) {
  while (true) {
    int index = rng() % (game.moves.size()-2
        -(game.moves.size()%2));
    if (GetMoveType(game.moves[index]) < kCapture
        && GetMoveType(game.moves[index+1]) < kCapture
        && GetMoveType(game.moves[index+2]) < kCapture
        && GetMoveType(game.moves[index+3]) < kCapture) {
      game.set_to_position_after(index);
      break;
    }
  }
}



void SetGamesToRandomQuiescent(std::vector<Game> &games) {
  std::cout << "Sampling Games!" << std::endl;
  for (size_t i = 0; i < games.size(); i++) {
    while (true) {
      int index = rng() % (games[i].moves.size()-2
          -(games[i].moves.size()%2));
      if (GetMoveType(games[i].moves[index]) < kCapture
          && GetMoveType(games[i].moves[index+1]) < kCapture) {
          //&& GetMoveType(games[i].moves[index+2]) < kCapture
          //&& GetMoveType(games[i].moves[index+3]) < kCapture) {
        games[i].set_to_position_after(index);
        break;
      }
    }
  }
  std::cout << "Finished sampling!" << std::endl;
}

void SetGamesToRandom(std::vector<Game> &games) {
  std::cout << "Sampling Games!" << std::endl;
  for (size_t i = 0; i < games.size(); i++) {
    int index = rng() % (games[i].moves.size()-2
        -(games[i].moves.size()%2));
    games[i].set_to_position_after(index);
  }
  std::cout << "Finished sampling!" << std::endl;
}



template<int k>
GMM<k, kPhaseVecLength> EMForGMM(std::vector<Game> &games, int iterations = 50) {
  SetGamesToRandomQuiescent(games);
  std::shuffle(games.begin(), games.end(), rng);
  std::vector<Game> validation;
  for (int i = 0; i < 10000; i++) {
    validation.emplace_back(games.back());
    games.pop_back();
  }
  GMM<k, kPhaseVecLength> best_gmm;
  double log_prob = 0, best_log_prob = -100000000;
  int initializations = 100;
  while (initializations--) {
    int it = iterations;
    GMM<k, kPhaseVecLength> gmm;
    std::cout << "setting random means" << std::endl;
    for (size_t i = 0; i < k; i++) {
      bool unique = false;
      while (!unique) {
        gmm.mixtures[i].mu = GetBoardPhaseVec(games[rng() % games.size()].board);
        unique = true;
        for (size_t j = 0; j < i; j++) {
          if (gmm.mixtures[i].mu == gmm.mixtures[j].mu) {
            unique = false;
          }
        }
      }
      gmm.mixtures[i].mu.print();
      std::cout << std::endl;
    }
    std::cout << "initializing centroid assignments" << std::endl;
    std::vector<Vec<double, k> > assignments(games.size());
    for (size_t i = 0; i < games.size(); i++) {
      if (i < games.size() / 25) {
        assignments[i] = i % k;
      }
      else {
        assignments[i] = gmm.GetD2MixtureAssignment(GetBoardPhaseVec(games[i].board));
      }
    }
    while (it--) {
      std::cout << "Remaining iterations: " << it << std::endl;
      //Calculate Mu
      std::cout << "Calculating Means" << std::endl;
      std::vector<Vec<double,kPhaseVecLength> > sums(k);
      Vec<double,k> counts;
      std::cout << "Counts: ";
      counts.print(); std::cout << std::endl;
      for (size_t i = 0; i < games.size(); i++) {
        for (size_t mix = 0; mix < k; mix++) {
          sums.at(mix) += assignments[i][mix] * GetBoardPhaseVec(games[i].board);
        }
        counts += assignments[i];
      }
      for (size_t i = 0; i < k; i++) {
        gmm.mixtures[i].mu = sums[i] / counts[i];
        gmm.weights[i] = counts[i];
        std::cout << "Mixture Count[" << i <<"]: "<< counts[i] << std::endl;
        std::cout << "Mixture Means: ";
        gmm.mixtures[i].mu.print();
        std::cout << std::endl;
      }
      std::cout << "Calculating Standard Deviations" << std::endl;
      //Calculate Sigma
      for (size_t j = 0; j < k; j++) {
        for (size_t m = 0; m < kPhaseVecLength; m++) {
          for (size_t n = 0; n < kPhaseVecLength; n++) {
            gmm.mixtures[j].sigma[m][n] = 0;
          }
        }
      }
      for (size_t i = 0; i < games.size(); i++) {
        Vec<double, kPhaseVecLength> game_vec = GetBoardPhaseVec(games[i].board);
        for (size_t j = 0; j < k; j++) {
          for (size_t m = 0; m < kPhaseVecLength; m++) {
            double d1 = gmm.mixtures[j].mu[m] - game_vec[m];
            for (size_t n = 0; n < kPhaseVecLength; n++) {
              double d2 = gmm.mixtures[j].mu[n] - game_vec[n];
              gmm.mixtures[j].sigma[m][n] += assignments[i][j] * (d1 * d2);
            }
          }
        }
      }
      for (size_t j = 0; j < k; j++) {
        if (counts[j] <= 1) {
          std::cout << "Mixture " << j << " has no count!" << std::endl;
          continue;
        }
        std::cout << "Mixture " << j << " Covariance Matrix:" << std::endl;
        for (size_t m = 0; m < kPhaseVecLength; m++) {
          for (size_t n = 0; n < kPhaseVecLength; n++) {
            gmm.mixtures[j].sigma[m][n] /= (counts[j]-1);
            if (m == n) {
              gmm.mixtures[j].sigma[m][n] = std::max(gmm.mixtures[j].sigma[m][n], std::pow(0.004,2));
            }
            std::cout << gmm.mixtures[j].sigma[m][n] << " ";
          }
          std::cout << std::endl;
        }
        gmm.mixtures[j].set_sigma_inv();
        std::cout << "Divisor " << j<< ":" << gmm.mixtures[j].sqrt_det_sigma_times_divisor << std::endl;
      }
      //Calculate Indexes
      if (it > 0) {
        std::cout << "Calculating Sample assignments" << std::endl;
        for (size_t i = 0; i < games.size(); i++) {
          assignments[i] =
              gmm.GetWeightedProbabilities(GetBoardPhaseVec(games[i].board));
        }
      }
      //Calculate mixture probability
      log_prob = 0;
      for (size_t i = 0; i < validation.size(); i++) {
        log_prob += std::log(gmm.GetSampleProbability(GetBoardPhaseVec(validation[i].board)));
      }
      std::cout << "Log Mixture probability: " << log_prob << std::endl;
    }
    if (log_prob > best_log_prob) {
      best_log_prob = log_prob;
      best_gmm = gmm;
    }
  }
  while(validation.size() > 0) {
    games.emplace_back(validation.back());
    validation.pop_back();
  }
  std::cout << "Best log probability validation sum: " << best_log_prob << std::endl;
  return best_gmm;
}

void SampledEMForGMM(int iterations) {
  search::set_print_info(false);
  const int k = settings::kGMMk;
  std::vector<Game> games = data::LoadGames();
  SetGamesToRandom(games);
  std::shuffle(games.begin(), games.end(), rng);
  std::vector<Board> positions;
  positions.reserve(games.size());
  for (int i = 0; i < games.size(); i++) {
    positions.emplace_back(search::SampleEval(games[i].board));
    if (positions.size() % 1000 == 0) {
      std::cout << "sampled " << positions.size() << " positions!" << std::endl;
    }
  }
  games.clear();
  std::vector<Board> validation;
  for (int i = 0; i < 10000; i++) {
    validation.emplace_back(positions.back());
    positions.pop_back();
  }
  GMM<k, kPhaseVecLength> best_gmm;
  double log_prob = 0, best_log_prob = -100000000;
  int initializations = 1;
  while (initializations--) {
    int it = iterations;
    GMM<k, kPhaseVecLength> gmm;
    std::cout << "setting random means" << std::endl;
    for (size_t i = 0; i < k; i++) {
      bool unique = false;
      while (!unique) {
        gmm.mixtures[i].mu = GetBoardPhaseVec(positions[rng() % positions.size()]);
        unique = true;
        for (size_t j = 0; j < i; j++) {
          if (gmm.mixtures[i].mu == gmm.mixtures[j].mu) {
            unique = false;
          }
        }
      }
      gmm.mixtures[i].mu.print();
      std::cout << std::endl;
    }
    std::cout << "initializing centroid assignments" << std::endl;
    std::vector<Vec<double, k> > assignments(positions.size());
    for (size_t i = 0; i < positions.size(); i++) {
      assignments[i] = gmm.GetD2MixtureAssignment(GetBoardPhaseVec(positions[i]));
    }
    while (it--) {
      std::cout << "Remaining iterations: " << it << std::endl;
      //Calculate Mu
      std::cout << "Calculating Means" << std::endl;
      std::vector<Vec<double,kPhaseVecLength> > sums(k);
      Vec<double,k> counts;
      std::cout << "Counts: ";
      counts.print(); std::cout << std::endl;
      for (size_t i = 0; i < positions.size(); i++) {
        for (size_t mix = 0; mix < k; mix++) {
          sums.at(mix) += assignments[i][mix] * GetBoardPhaseVec(positions[i]);
        }
        counts += assignments[i];
      }
      for (size_t i = 0; i < k; i++) {
        gmm.mixtures[i].mu = sums[i] / counts[i];
        gmm.weights[i] = counts[i];
        std::cout << "Mixture Count[" << i <<"]: "<< counts[i] << std::endl;
      }
      std::cout << "Calculating Standard Deviations" << std::endl;
      //Calculate Sigma
      if (true) {
        for (size_t j = 0; j < k; j++) {
          for (size_t m = 0; m < kPhaseVecLength; m++) {
            for (size_t n = 0; n < kPhaseVecLength; n++) {
              gmm.mixtures[j].sigma[m][n] = 0;
            }
          }
        }
        for (size_t i = 0; i < positions.size(); i++) {
          Vec<double, kPhaseVecLength> game_vec = GetBoardPhaseVec(positions[i]);
          for (size_t j = 0; j < k; j++) {
            for (size_t m = 0; m < kPhaseVecLength; m++) {
              double d1 = gmm.mixtures[j].mu[m] - game_vec[m];
              for (size_t n = 0; n < kPhaseVecLength; n++) {
                double d2 = gmm.mixtures[j].mu[n] - game_vec[n];
                gmm.mixtures[j].sigma[m][n] += assignments[i][j] * (d1 * d2);
              }
            }
          }
        }
        for (size_t j = 0; j < k; j++) {
          if (counts[j] <= 1) {
            std::cout << "Mixture " << j << " has no count!" << std::endl;
            continue;
          }
          std::cout << "Mixture " << j << " Covariance Matrix:" << std::endl;
          for (size_t m = 0; m < kPhaseVecLength; m++) {
            for (size_t n = 0; n < kPhaseVecLength; n++) {
              gmm.mixtures[j].sigma[m][n] /= (counts[j]-1);
              if (m == n) {
                gmm.mixtures[j].sigma[m][n] = std::max(gmm.mixtures[j].sigma[m][n], std::pow(0.004,2));
              }
              else {
                //gmm.mixtures[j].sigma[m][n] = 0;
              }
              std::cout << gmm.mixtures[j].sigma[m][n] << " ";
            }
            std::cout << std::endl;
          }
          gmm.mixtures[j].set_sigma_inv();
          std::cout << "Divisor " << j<< ":" << gmm.mixtures[j].sqrt_det_sigma_times_divisor << std::endl;
        }
      }
      //Calculate Indexes
      if (it > 0) {
        std::cout << "Calculating Sample assignments" << std::endl;
        for (size_t i = 0; i < positions.size(); i++) {
          assignments[i] =
              gmm.GetWeightedProbabilities(GetBoardPhaseVec(positions[i]));
        }
      }
      //Calculate mixture probability
      log_prob = 0;
      for (size_t i = 0; i < validation.size(); i++) {
        log_prob += std::log(gmm.GetSampleProbability(GetBoardPhaseVec(validation[i])));
      }
      std::cout << "Log Mixture probability: " << log_prob << std::endl;
    }
    if (log_prob > best_log_prob) {
      best_log_prob = log_prob;
      best_gmm = gmm;
    }
  }
  std::cout << "Best log probability validation sum: " << best_log_prob << std::endl;
  std::ofstream file(settings::kSampledMixtureFile);
  for (size_t m = 0; m < k; m++) {
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      file << best_gmm.mixtures[m].mu[i] << " ";
    }
    file << best_gmm.weights[m] << std::endl;
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      for (size_t j = 0; j < kPhaseVecLength; j++) {
        file << best_gmm.mixtures[m].sigma[i][j] << " ";
      }
      file << std::endl;
    }
  }
  file.flush();
  file.close();
  search::set_print_info(true);
}


void RunEMForGMM() {
  const int k = settings::kGMMk;
  std::vector<Game> games = data::LoadGames(1200000);
  GMM<k,kPhaseVecLength> gmm = EMForGMM<k>(games);
  std::ofstream file(settings::kMixtureFile);
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

template<int k>
void SaveGMM(GMM<k,kPhaseVecLength> &gmm, std::string file_name) {
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

void SaveGMMVariables() {
  std::ofstream file(settings::kGMMParamsFile);
  std::ofstream description_file(settings::kParamExplanationFile);
  int idx = 0;
  for (int i = 0; i < kNumFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    for (int j = 0; j < settings::kGMMk; j++) {
      file << feature_GMM_score_values[i][j] << " ";
      description_file  << feature_GMM_score_values[i][j] << " ";
    }
    file << std::endl;
    description_file << "<-- " << kFeatureInfos[idx].info << std::endl;
  }
  file.flush();
  file.close();
  description_file.flush();
  description_file.close();
}

void LoadMixtures() {
  const int k = settings::kGMMk;
  std::ifstream fileg(settings::kMixtureFile);
  for (size_t m = 0; m < k; m++) {
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      fileg >> gmm_main.mixtures[m].mu[i];
    }
    fileg >> gmm_main.weights[m];
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      for (size_t j = 0; j < kPhaseVecLength; j++) {
        fileg >> gmm_main.mixtures[m].sigma[i][j];
      }
    }
    gmm_main.mixtures[m].set_sigma_inv();
  }
  fileg.close();
}

void LoadGMMVariables() {
  const int k = settings::kGMMk;
  std::ifstream file(settings::kGMMParamsFile);
  for (size_t i = 0; i < kNumFeatures; i++) {
    for (size_t j = 0; j < k; j++) {
      file >> feature_GMM_score_values[i][j];
    }
  }
  file.close();

  LoadMixtures();
}

enum SGDVariantType {
  SGDNormal, SGDAdaDelta, SGDAdaGrad, SGDAdam, SGDRotating
};

template<typename T>
double Sigmoid(T x) {
  return 1 / ( 1 + std::exp(-x) );
}

void EnforceConstraints(std::vector<double> &variables) {
  const int k = settings::kGMMk;
  int idx = 0;
  for (size_t i = 0; i < kNumFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    for (size_t j = 0; j < k; j++) {
      variables.at(i * k + j) = std::max((double) kFeatureInfos[idx].min_value, variables[i * k + j]);
      variables.at(i * k + j) = std::min((double) kFeatureInfos[idx].max_value, variables[i * k + j]);
      if (kFeatureInfos[idx].encoded_info == 1 && i < (kFeatureInfos[idx + 1].idx - 1)
          && variables.at(i * k + j) > variables.at((i + 1) * k + j)) {
        double dif = variables.at(i * k + j) - variables.at((i + 1) * k + j);
        variables.at(i * k + j) -= dif / 1000;
        variables.at((i + 1) * k + j) += dif / 1000;
        if (variables[i * k + j] > 0) {
          variables[i * k + j] -= dif / 50000;
        }
        else if (variables[(i+1) * k + j] < 0) {
          variables[(i+1) * k + j] += dif / 50000;
        }
      }
    }
  }
  for (size_t j = 0; j < k; j++) {
    variables[(kKingPSTIndex + 0) * k + j] = 0;
    variables[(kBaseValueIndex + kKnight) * k + j] = 0;
    variables[(kKnightSquaresIndex + 4) * k + j] = 0;
    variables[(kBishopMobility + 6) * k + j] = 0;
    variables[(kRookMobility + 6) * k + j] = 0;
    variables[(kQueenMobility + 10) * k + j] = 0;
  }
}

void SGDDrawMarginTrain() {
  const double scaling = 1024;
  double draw_margin = 1.0;
  long completed_iterations = 0;
  std::vector<Game> games = data::LoadGames();

  double nu = 0.000005;
  double kMinNu = (1.0 / kMillion);

  std::vector<int8_t> position_tries(games.size(), 0);
  while (nu > kMinNu) {
    size_t index = rng() % games.size();
    if (position_tries[index] == 5) {
      SetGameToRandomQuiescent(games[index]);
      position_tries[index] = 0;
    }
    position_tries[index]++;
    double target = games[index].result;
    if (games[index].board.get_turn() == kBlack) {
      target = 1 - target;
    }
    if (target != 0 && target != 0.5 && target != 1) {
      std::cout << "Target error!" << std::endl;
    }
    Score game_score = ScoreBoard(games[index].board);
    double gradient = 0;//sigmoid - target;
    if (target == 1) {
      gradient = Sigmoid<double>(draw_margin - game_score / scaling);
    }
    else if (target == 0) {
      gradient = 1.0 - Sigmoid<double>(-draw_margin - game_score / scaling);
    }
    else {
      gradient = Sigmoid<double>(game_score / scaling - draw_margin)
                  - Sigmoid<double>(-game_score / scaling - draw_margin)
                  - (std::cosh(draw_margin) / std::sinh(draw_margin));

    }
    draw_margin -= nu * gradient;
    completed_iterations++;
    if (completed_iterations % 100000 == 0) {
      std::cout << "completed " << completed_iterations << " iterations! Current margin: "
          << std::round(draw_margin * scaling) << std::endl;
    }
    if (completed_iterations % 200000000 == 0) {
      nu /= 2;
      std::cout << "New Nu: " << nu << std::endl;
    }
  }
  std::cout << "Training completed!" << std::endl;
}

template<int k, int sgd_variant>
void SGDMarginVariantTrain(bool from_scratch) {
  const int scaling = 1024;
  const double draw_margin = 1.0;
  const double lambda1 = 1.0 / kThousand;
  const double lambda2 = 0.0 / kMillion;
  const bool regularize = false;
  std::vector<double> variables(kNumFeatures * k);
  if (!from_scratch) {
    for (size_t i = 0; i < kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        variables[i * k + j] = feature_GMM_score_values[i][j];
      }
    }
  }
  long completed_iterations = 0;
  std::vector<Game> games = data::LoadGames();
  GMM<k, kPhaseVecLength> gmm;
  if (from_scratch && settings::kTrainGMMFromScratch) {
    gmm = EMForGMM<k>(games);
    SaveGMM<k>(gmm, settings::kMixtureFile);
  }
  else {
    if (from_scratch && !settings::kTrainGMMFromScratch) {
      LoadMixtures();
    }
    for (size_t m = 0; m < k; m++) {
      for (size_t i = 0; i < kPhaseVecLength; i++) {
        gmm.mixtures[m].mu[i]=  gmm_main.mixtures[m].mu[i];
        for (size_t j = 0; j < kPhaseVecLength; j++) {
          gmm.mixtures[m].sigma[i][j] = gmm_main.mixtures[m].sigma[i][j];
        }
      }
      gmm.mixtures[m].set_sigma_inv();
      gmm.weights[m] = gmm_main.weights[m];
    }
  }

  std::vector< std::vector<double> > feature_std_dev(kNumFeatures, std::vector<double>(k));
  Vec<double, k> mixture_counts;
  for (int index = 0; index < games.size(); index++) {
    SetGameToRandomQuiescent(games[index]);
    Vec<double, k> probabilities = gmm.GetWeightedProbabilities(
        GetBoardPhaseVec(games[index].board));
    mixture_counts += probabilities;
    std::vector<int> features = ScoreBoard<std::vector<int> >(games[index].board);
    for (int i = 0; i < kNumFeatures; i++) {
      for (int j = 0; j < k; j++) {
        feature_std_dev[i][j] += features[i] * features[i] * probabilities[j];
      }
    }
  }
  for (int i = 0; i < kNumFeatures; i++) {
    for (int j = 0; j < k; j++) {
      feature_std_dev[i][j] /= (mixture_counts[j] - 1);
      //Now we have the variance, the standard deviation is the square root.
      if (feature_std_dev[i][j] > 0) {
        feature_std_dev[i][j] = std::sqrt(feature_std_dev[i][j]);
      }
      feature_std_dev[i][j] = 1.0;
      //We want to avoid having to handle removed features specially
      feature_std_dev[i][j] = std::max(0.001,feature_std_dev[i][j]);
      variables[i * k + j] *= feature_std_dev[i][j];
    }
  }

  const double kGamma = 0.9;//AdaDelta
  std::vector< std::vector <double> > expectSqUpdate(kNumFeatures,
                                                     std::vector<double>(k));
  const double kBeta1 = 0.99;
  const double kBeta2 = 0.999;
  const double kEpsilon = 0.00000001;
  double rotating_base_nu = 0.25;
  double nu = 0.5;
  double kMinNu = (1.0 / kMillion);
  int focus = 0;
  if (sgd_variant == SGDAdam) {
    nu = 0.01 * (1 - kBeta1);
    kMinNu = (1.0 / kMillion) * (1 - kBeta1);
  }

  std::vector< std::vector <double> > matG(kNumFeatures,//AdaGrad
                                                     std::vector<double>(k, kEpsilon));
  std::vector< std::vector <double> > expectGrad(kNumFeatures,
                                                     std::vector<double>(k));
  std::vector< std::vector <double> > expectSqGrad(kNumFeatures,
                                                     std::vector<double>(k));

  std::vector<int8_t> position_tries(games.size(), 0);
  while (sgd_variant == SGDAdaDelta || sgd_variant == SGDRotating || nu > kMinNu) {
    size_t index = rng() % games.size();
    if (position_tries[index] == 5) {
      SetGameToRandomQuiescent(games[index]);
      position_tries[index] = 0;
    }
    position_tries[index]++;
    double target = games[index].result;
    if (games[index].board.get_turn() == kBlack) {
      target = 1 - target;
    }
    if (target != 0 && target != 0.5 && target != 1) {
      std::cout << "Target error!" << std::endl;
    }
    std::vector<int> features =
        ScoreBoard<std::vector<int> >(games[index].board);
    features[kBaseValueIndex + kKnight] = 0;
    if (!regularize) {
      features[kKingPSTIndex + 0] = 0;
      //features[kActivityBonusIndex + kQueen] = 0;
      //features[kAbstractActivityIndex + kQueen] = 0;
      features[kKnightSquaresIndex + 4] = 0;
      features[kBishopMobility + 6] = 0;
      features[kRookMobility + 6] = 0;
      features[kQueenMobility + 10] = 0;
    }
    Vec<double, k> score;
    for (size_t i = 0; i < kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        score[j] += features[i] * variables[k * i + j]
                                / feature_std_dev[i][j];
      }
    }
    Vec<double, k> probabilities = gmm.GetWeightedProbabilities(
        GetBoardPhaseVec(games[index].board));
    double probability_of_position = gmm.GetSampleProbability(
        GetBoardPhaseVec(games[index].board));
    if (probability_of_position == 0) {
      std::cout << "Error! Found impossible position!" << std::endl;
    }
    double final_score = 0;
    for (size_t i = 0; i < k; i++) {
      final_score += score[i] * probabilities[i];
    }
    final_score /= scaling;
    //double sigmoid = 1 / ( 1 + std::exp(-final_score) );
    double PWin = 0, PLoss = 0, PDraw = 0;
    if (target == 1) {
      PWin = 1;
    }
    else if (target == 0) {
      PLoss = 1;
    }
    else {
      PDraw = 1;
    }
    double gradient = PWin  * (Sigmoid<double>(final_score - draw_margin) - 1)
                    + PLoss * (1 - Sigmoid<double>(-final_score - draw_margin))
                    + PDraw * (1 - Sigmoid<double>(-final_score - draw_margin)
                                 - Sigmoid<double>(-final_score + draw_margin));
    for (size_t i = 0; i < kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        if (sgd_variant == SGDAdam) {
          double grad = (gradient * probabilities[j] * features[i])
              / feature_std_dev[i][j];
          expectGrad[i][j] = kBeta1 * expectGrad[i][j] + (1 - kBeta1) * grad;
          expectSqGrad[i][j] = kBeta2 * expectSqGrad[i][j] + (1 - kBeta2) * grad * grad;
          double m_hat = expectGrad[i][j] / (1 - kBeta1);
          double v_hat = expectSqGrad[i][j] / (1 - kBeta2);
          variables[i * k + j] -= (nu / (std::sqrt(v_hat) + kEpsilon)) * m_hat;
        }
        else if (sgd_variant == SGDAdaDelta) {
          double grad = (gradient * probabilities[j] * features[i])
              / (32 * feature_std_dev[i][j]);
          double update = -(std::sqrt(expectSqUpdate[i][j] + kEpsilon)
                            / std::sqrt(expectSqGrad[i][j] + kEpsilon)) * grad;
          variables[i * k + j] += update;

          expectSqGrad[i][j] = kGamma * expectSqGrad[i][j]
                               + (1 - kGamma) * grad * grad;
          expectSqUpdate[i][j] = kGamma * expectSqUpdate[i][j]
                               + (1 - kGamma) * update * update;
        }
        else if (sgd_variant == SGDAdaGrad) {
          double grad = (gradient * probabilities[j] * features[i])
              / feature_std_dev[i][j];
          variables[i * k + j] -= (nu / std::sqrt(matG[i][j])) * grad;
          matG[i][j] += grad * grad;
        }
        else if (sgd_variant == SGDRotating) {
          if (i == focus) {
            variables[i * k + j] -= nu * gradient * probabilities[j] * features[i]
                                                                   / feature_std_dev[i][j];
          }
        }
        else {//Regular SGD
          variables[i * k + j] -= nu * gradient * probabilities[j] * features[i]
                                                                   / feature_std_dev[i][j];
        }
        if (regularize && features[i] != 0) {
          variables[i * k + j] -= nu * lambda1 * probabilities[j] * sign(variables[i * k + j]);
          variables[i * k + j] -= nu * lambda2 * probabilities[j] * variables[i * k + j];
        }
      }
    }
    completed_iterations++;
    if (completed_iterations % 5 == 0) {
      EnforceConstraints(variables);
    }
    if (completed_iterations % 1000000 == 0) {
      std::cout << "completed " << completed_iterations/1000000 << " million iterations!"
          << std::endl;
      for (size_t i = 0; i < kNumFeatures; i++) {
        for (size_t j = 0; j < k; j++) {
          feature_GMM_score_values[i][j] = std::round(variables[i * k + j]
                                                      / feature_std_dev[i][j]);
        }
      }
      if (completed_iterations % 2000000 == 0 || sgd_variant != SGDRotating) {
        SaveGMMVariables();
      }
      if (completed_iterations % 500000000 == 0) {
        benchmark::EntropyLossTimedSuite(Milliseconds(10));
      }
    }
    if (completed_iterations % 200000000 == 0 && sgd_variant != SGDAdaGrad
                                              && sgd_variant != SGDRotating) {
      nu /= 2;
      std::cout << "New Nu: " << nu << std::endl;
    }
    else if (completed_iterations % 100000 == 0 && sgd_variant == SGDRotating) {
      nu /= 2;
      if (completed_iterations % 2000000 == 0) {
        nu = rotating_base_nu;
        focus = rng() % kNumFeatures;
      }
    }
  }
  std::cout << "Training completed!" << std::endl;
}


template<int k, int sgd_variant>
void SGDVariantTrain(bool from_scratch) {
  const int scaling = 1024;
  const double lambda1 = 1.0 / kThousand;
  const double lambda2 = 0.0 / kMillion;
  const bool regularize = false;
  std::vector<double> variables(kNumFeatures * k);
  if (!from_scratch) {
    for (size_t i = 0; i < kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        variables[i * k + j] = feature_GMM_score_values[i][j];
      }
    }
  }
  long completed_iterations = 0;
  std::vector<Game> games = data::LoadGames();
  GMM<k, kPhaseVecLength> gmm;
  if (from_scratch && settings::kTrainGMMFromScratch) {
    gmm = EMForGMM<k>(games);
    SaveGMM<k>(gmm, settings::kMixtureFile);
  }
  else {
    if (from_scratch && !settings::kTrainGMMFromScratch) {
      LoadMixtures();
    }
    for (size_t m = 0; m < k; m++) {
      for (size_t i = 0; i < kPhaseVecLength; i++) {
        gmm.mixtures[m].mu[i]=  gmm_main.mixtures[m].mu[i];
        for (size_t j = 0; j < kPhaseVecLength; j++) {
          gmm.mixtures[m].sigma[i][j] = gmm_main.mixtures[m].sigma[i][j];
        }
      }
      gmm.mixtures[m].set_sigma_inv();
      gmm.weights[m] = gmm_main.weights[m];
    }
  }

  std::vector< std::vector<double> > feature_std_dev(kNumFeatures, std::vector<double>(k));
  Vec<double, k> mixture_counts;
  for (int index = 0; index < games.size(); index++) {
    SetGameToRandomQuiescent(games[index]);
    Vec<double, k> probabilities = gmm.GetWeightedProbabilities(
        GetBoardPhaseVec(games[index].board));
    mixture_counts += probabilities;
    std::vector<int> features = ScoreBoard<std::vector<int> >(games[index].board);
    for (int i = 0; i < kNumFeatures; i++) {
      for (int j = 0; j < k; j++) {
        feature_std_dev[i][j] += features[i] * features[i] * probabilities[j];
      }
    }
  }
  for (int i = 0; i < kNumFeatures; i++) {
    for (int j = 0; j < k; j++) {
      feature_std_dev[i][j] /= (mixture_counts[j] - 1);
      //Now we have the variance, the standard deviation is the square root.
      if (feature_std_dev[i][j] > 0) {
        feature_std_dev[i][j] = std::sqrt(feature_std_dev[i][j]);
      }
      feature_std_dev[i][j] = 1.0;
      //We want to avoid having to handle removed features specially
      feature_std_dev[i][j] = std::max(0.001,feature_std_dev[i][j]);
      variables[i * k + j] *= feature_std_dev[i][j];
    }
  }

  const double kGamma = 0.9;//AdaDelta
  std::vector< std::vector <double> > expectSqUpdate(kNumFeatures,
                                                     std::vector<double>(k));
  const double kBeta1 = 0.99;
  const double kBeta2 = 0.999;
  const double kEpsilon = 0.00000001;
  double rotating_base_nu = 0.25;
  double nu = 0.5;
  double kMinNu = (1.0 / kMillion);
  int focus = 0;
  if (sgd_variant == SGDAdam) {
    nu = 0.01 * (1 - kBeta1);
    kMinNu = (1.0 / kMillion) * (1 - kBeta1);
  }

  std::vector< std::vector <double> > matG(kNumFeatures,//AdaGrad
                                                     std::vector<double>(k, kEpsilon));
  std::vector< std::vector <double> > expectGrad(kNumFeatures,
                                                     std::vector<double>(k));
  std::vector< std::vector <double> > expectSqGrad(kNumFeatures,
                                                     std::vector<double>(k));

  std::vector<int8_t> position_tries(games.size(), 0);
  while (sgd_variant == SGDAdaDelta || sgd_variant == SGDRotating || nu > kMinNu) {
    size_t index = rng() % games.size();
    if (position_tries[index] == 5) {
      SetGameToRandomQuiescent(games[index]);
      position_tries[index] = 0;
    }
    position_tries[index]++;
    double target = games[index].result;
    if (games[index].board.get_turn() == kBlack) {
      target = 1 - target;
    }
    if (target != 0 && target != 0.5 && target != 1) {
      std::cout << "Target error!" << std::endl;
    }
    std::vector<int> features =
        ScoreBoard<std::vector<int> >(games[index].board);
    features[kBaseValueIndex + kKnight] = 0;
    Vec<double, k> score;
    for (size_t i = 0; i < kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        score[j] += features[i] * variables[k * i + j]
                                / feature_std_dev[i][j];
      }
    }
    Vec<double, k> probabilities = gmm.GetWeightedProbabilities(
        GetBoardPhaseVec(games[index].board));
    double probability_of_position = gmm.GetSampleProbability(
        GetBoardPhaseVec(games[index].board));
    if (probability_of_position == 0) {
      std::cout << "Error! Found impossible position!" << std::endl;
    }
    double final_score = 0;
    for (size_t i = 0; i < k; i++) {
      final_score += score[i] * probabilities[i];
    }
    final_score /= scaling;
    double sigmoid = 1 / ( 1 + std::exp(-final_score) );
    double gradient = sigmoid - target;
    for (size_t i = 0; i < kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        if (sgd_variant == SGDAdam) {
          double grad = (gradient * probabilities[j] * features[i])
              / feature_std_dev[i][j];
          expectGrad[i][j] = kBeta1 * expectGrad[i][j] + (1 - kBeta1) * grad;
          expectSqGrad[i][j] = kBeta2 * expectSqGrad[i][j] + (1 - kBeta2) * grad * grad;
          double m_hat = expectGrad[i][j] / (1 - kBeta1);
          double v_hat = expectSqGrad[i][j] / (1 - kBeta2);
          variables[i * k + j] -= (nu / (std::sqrt(v_hat) + kEpsilon)) * m_hat;
        }
        else if (sgd_variant == SGDAdaDelta) {
          double grad = (gradient * probabilities[j] * features[i])
              / (32 * feature_std_dev[i][j]);
          double update = -(std::sqrt(expectSqUpdate[i][j] + kEpsilon)
                            / std::sqrt(expectSqGrad[i][j] + kEpsilon)) * grad;
          variables[i * k + j] += update;

          expectSqGrad[i][j] = kGamma * expectSqGrad[i][j]
                               + (1 - kGamma) * grad * grad;
          expectSqUpdate[i][j] = kGamma * expectSqUpdate[i][j]
                               + (1 - kGamma) * update * update;
        }
        else if (sgd_variant == SGDAdaGrad) {
          double grad = (gradient * probabilities[j] * features[i])
              / feature_std_dev[i][j];
          variables[i * k + j] -= (nu / std::sqrt(matG[i][j])) * grad;
          matG[i][j] += grad * grad;
        }
        else if (sgd_variant == SGDRotating) {
          if (i == focus) {
            variables[i * k + j] -= nu * gradient * probabilities[j] * features[i]
                                                                   / feature_std_dev[i][j];
          }
        }
        else {//Regular SGD
          variables[i * k + j] -= nu * gradient * probabilities[j] * features[i]
                                                                   / feature_std_dev[i][j];
        }
        if (regularize && features[i] != 0) {
          variables[i * k + j] -= nu * lambda1 * probabilities[j] * sign(variables[i * k + j]);
          variables[i * k + j] -= nu * lambda2 * probabilities[j] * variables[i * k + j];
        }
      }
    }
    completed_iterations++;
    if (completed_iterations % 5 == 0) {
      EnforceConstraints(variables);
    }
    if (completed_iterations % 1000000 == 0) {
      std::cout << "completed " << completed_iterations/1000000 << " million iterations!"
          << std::endl;
      for (size_t i = 0; i < kNumFeatures; i++) {
        for (size_t j = 0; j < k; j++) {
          feature_GMM_score_values[i][j] = std::round(variables[i * k + j]
                                                      / feature_std_dev[i][j]);
        }
      }
      if (completed_iterations % 2000000 == 0 || sgd_variant != SGDRotating) {
        SaveGMMVariables();
      }
      if (completed_iterations % 500000000 == 0) {
        benchmark::EntropyLossTimedSuite(Milliseconds(10));
      }
    }
    if (completed_iterations % 200000000 == 0 && sgd_variant != SGDAdaGrad
                                              && sgd_variant != SGDRotating) {
      nu /= 2;
      std::cout << "New Nu: " << nu << std::endl;
    }
    else if (completed_iterations % 100000 == 0 && sgd_variant == SGDRotating) {
      nu /= 2;
      if (completed_iterations % 2000000 == 0) {
        nu = rotating_base_nu;
        focus = rng() % kNumFeatures;
      }
    }
  }
  std::cout << "Training completed!" << std::endl;
}

template<int sgd_variant, bool use_draw_margin>
void SGDVariantRL() {
  const int k = settings::kGMMk;
  const double draw_margin = 1.0;
  const double scaling = 1024;
  std::vector<double> variables(kNumFeatures * k);
  for (size_t i = 0; i < kNumFeatures; i++) {
    for (size_t j = 0; j < k; j++) {
      variables[i * k + j] = feature_GMM_score_values[i][j];
    }
  }
  long completed_iterations = 0;
  std::vector<Game> games = data::LoadGames();
  GMM<k, kPhaseVecLength> gmm;
  LoadMixtures();
  for (size_t m = 0; m < k; m++) {
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      gmm.mixtures[m].mu[i]=  gmm_main.mixtures[m].mu[i];
      for (size_t j = 0; j < kPhaseVecLength; j++) {
        gmm.mixtures[m].sigma[i][j] = gmm_main.mixtures[m].sigma[i][j];
      }
    }
    gmm.mixtures[m].set_sigma_inv();
    gmm.weights[m] = gmm_main.weights[m];
  }

  std::vector< std::vector<double> > feature_std_dev(kNumFeatures, std::vector<double>(k));
  Vec<double, k> mixture_counts;
  for (int index = 0; index < games.size(); index++) {
    SetGameToRandomQuiescent(games[index]);
    Vec<double, k> probabilities = gmm.GetWeightedProbabilities(
        GetBoardPhaseVec(games[index].board));
    mixture_counts += probabilities;
    std::vector<int> features = ScoreBoard<std::vector<int> >(games[index].board);
    for (int i = 0; i < kNumFeatures; i++) {
      for (int j = 0; j < k; j++) {
        feature_std_dev[i][j] += features[i] * features[i] * probabilities[j];
      }
    }
  }
  for (int i = 0; i < kNumFeatures; i++) {
    for (int j = 0; j < k; j++) {
      feature_std_dev[i][j] /= (mixture_counts[j] - 1);
      //Now we have the variance, the standard deviation is the square root.
      if (feature_std_dev[i][j] > 0) {
        feature_std_dev[i][j] = std::sqrt(feature_std_dev[i][j]);
      }
      feature_std_dev[i][j] = 1.0;
      //We want to avoid having to handle removed features specially
      feature_std_dev[i][j] = std::max(0.001,feature_std_dev[i][j]);
      variables[i * k + j] *= feature_std_dev[i][j];
    }
  }

  const double kGamma = 0.9;//AdaDelta
  std::vector< std::vector <double> > expectSqUpdate(kNumFeatures,
                                                     std::vector<double>(k));
  const double kBeta1 = 0.99;
  const double kBeta2 = 0.999;
  const double kEpsilon = 0.00000001;
  double rotating_base_nu = 0.25;
  double nu = 0.2;
  double kMinNu = (1.0 / kMillion);
  int focus = 0;
  if (sgd_variant == SGDAdam) {
    nu = 1.0 * (1 - kBeta1);
    kMinNu = (1.0 / kMillion) * (1 - kBeta1);
  }

  std::vector< std::vector <double> > matG(kNumFeatures,//AdaGrad
                                                     std::vector<double>(k, kEpsilon));
  std::vector< std::vector <double> > expectGrad(kNumFeatures,
                                                     std::vector<double>(k));
  std::vector< std::vector <double> > expectSqGrad(kNumFeatures,
                                                     std::vector<double>(k));

  std::vector<int8_t> position_tries(games.size(), 5);
  std::vector<double> position_targets(games.size());
  //double running_mean = 0.5;
  Vec<double, k> r_mean, prior;
  for (int i = 0; i < k; i++) {
    r_mean = 0.5;
    prior = 0.5;
  }
  while (nu > kMinNu || sgd_variant == SGDAdaDelta || sgd_variant == SGDRotating) {
    size_t index = rng() % games.size();
    if (position_tries[index] >= 5) {
      Score score = 0, qscore = -1;
      //std::cout << "-> " << index << std::flush;
      int qsearch_attempts = 0;
      while (score != qscore) {
        data::SetGameToRandom(games[index]);
        if (games[index].board.InCheck())
          continue;
        qsearch_attempts++;
        if (qsearch_attempts == 20)
          break;
        score = ScoreBoard(games[index].board);
        qscore = search::QSearch(games[index].board);
      }
      if (qsearch_attempts == 20)
        continue;
      position_tries[index] = 0;
      //std::cout << " -> " << games[index].board.get_num_made_moves() << std::flush;
      search::set_print_info(false);
      search::DepthSearch(games[index].board, 4);
      search::set_print_info(true);
      //std::cout << "<- " << std::endl;
      position_targets[index] = search::get_last_search_score() / scaling;
      Vec<double, k> probabilities = gmm.GetWeightedProbabilities(
          GetBoardPhaseVec(games[index].board));
      for (int i = 0; i < k; i++) {
        r_mean[i] = 0.99995 * r_mean[i]
                  + 0.00005 * (position_targets[index] * probabilities[i]
                             + r_mean[i] * (1 - probabilities[i]));
      }
      double game_res = games[index].result;
      if (games[index].board.get_turn() == kBlack) {
        game_res = 1 - game_res;
      }
      for (int i = 0; i < k; i++) {
        prior[i] = 0.99995 * prior[i]
                  + 0.00005 * (game_res * probabilities[i]
                             + prior[i] * (1 - probabilities[i]));
      }
      //position_targets[index] += (prior.dot(probabilities) - r_mean.dot(probabilities)) / 2;
      //position_targets[index] = 0.7 * position_targets[index] + 0.3 * game_res;
      //position_targets[index] = std::min(1.0, position_targets[index]);
      //position_targets[index] = std::max(0.0, position_targets[index]);
    }
    position_tries[index]++;
    std::vector<int> features =
        ScoreBoard<std::vector<int> >(games[index].board);
    Vec<double, k> score;
    for (size_t i = 0; i < kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        score[j] += features[i] * variables[k * i + j]
                                / feature_std_dev[i][j];
      }
    }
    Vec<double, k> probabilities = gmm.GetWeightedProbabilities(
        GetBoardPhaseVec(games[index].board));
    double probability_of_position = gmm.GetSampleProbability(
        GetBoardPhaseVec(games[index].board));
    if (probability_of_position == 0) {
      std::cout << "Error! Found impossible position!" << std::endl;
    }
    double final_score = 0;
    for (size_t i = 0; i < k; i++) {
      final_score += score[i] * probabilities[i];
    }
    final_score /= scaling;
    double sigmoid = 1 / ( 1 + std::exp(-final_score) );
    double game_res = games[index].result;
    if (games[index].board.get_turn() == kBlack) {
      game_res = 1 - game_res;
    }
    double target = 0.7 * Sigmoid<double>(position_targets[index])
                  + 0.3 * game_res;
    double gradient = sigmoid - target;
    if (use_draw_margin) {
      double PWin = 0.7 * Sigmoid<double>(position_targets[index] - draw_margin);
      double PLoss = 0.7 * Sigmoid<double>(-position_targets[index] - draw_margin);
      double PDraw = 0.7 - PWin - PLoss;
      if (game_res == 1) {
        PWin += 0.3;
      }
      else if (game_res == 0) {
        PLoss += 0.3;
      }
      else {
        PDraw += 0.3;
      }
      gradient = PWin  * (Sigmoid<double>(final_score - draw_margin) - 1)
                      + PLoss * (1 - Sigmoid<double>(-final_score - draw_margin))
                      + PDraw * (1 - Sigmoid<double>(-final_score - draw_margin)
                                   - Sigmoid<double>(-final_score + draw_margin));
    }
    for (size_t i = 0; i < kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        if (sgd_variant == SGDAdam) {
          double grad = (gradient * probabilities[j] * features[i])
              / feature_std_dev[i][j];
          expectGrad[i][j] = kBeta1 * expectGrad[i][j] + (1 - kBeta1) * grad;
          expectSqGrad[i][j] = kBeta2 * expectSqGrad[i][j] + (1 - kBeta2) * grad * grad;
          double m_hat = expectGrad[i][j] / (1 - kBeta1);
          double v_hat = expectSqGrad[i][j] / (1 - kBeta2);
          variables[i * k + j] -= (nu / (std::sqrt(v_hat) + kEpsilon)) * m_hat;
        }
        else if (sgd_variant == SGDAdaDelta) {
          double grad = (gradient * probabilities[j] * features[i])
              / (32 * feature_std_dev[i][j]);
          double update = -(std::sqrt(expectSqUpdate[i][j] + kEpsilon)
                            / std::sqrt(expectSqGrad[i][j] + kEpsilon)) * grad;
          variables[i * k + j] += update;

          expectSqGrad[i][j] = kGamma * expectSqGrad[i][j]
                               + (1 - kGamma) * grad * grad;
          expectSqUpdate[i][j] = kGamma * expectSqUpdate[i][j]
                               + (1 - kGamma) * update * update;
        }
        else if (sgd_variant == SGDAdaGrad) {
          double grad = (gradient * probabilities[j] * features[i])
              / feature_std_dev[i][j];
          variables[i * k + j] -= (nu / std::sqrt(matG[i][j])) * grad;
          matG[i][j] += grad * grad;
        }
        else if (sgd_variant == SGDRotating) {
          if (i == focus) {
            variables[i * k + j] -= nu * gradient * probabilities[j] * features[i]
                                                                   / feature_std_dev[i][j];
          }
        }
        else {//Regular SGD
          variables[i * k + j] -= nu * gradient * probabilities[j] * features[i]
                                                                   / feature_std_dev[i][j];
        }
      }
    }
    completed_iterations++;
    if (completed_iterations % 5 == 0) {
      EnforceConstraints(variables);
    }
    if (completed_iterations % 10000 == 0) {
      std::cout << "completed " << completed_iterations/1000 << "000 iterations!"
          << " running means: ";
      r_mean.print();
      prior.print();
      std::cout << std::endl;
      for (size_t i = 0; i < kNumFeatures; i++) {
        for (size_t j = 0; j < k; j++) {
          feature_GMM_score_values[i][j] = std::round(variables[i * k + j]
                                                      / feature_std_dev[i][j]);
        }
      }
      if (completed_iterations % 20000 == 0 || sgd_variant != SGDRotating) {
        SaveGMMVariables();
      }
      if (completed_iterations % 5000000 == 0) {
        benchmark::EntropyLossTimedSuite(Milliseconds(10));
      }
    }
    if (completed_iterations % 5000000 == 0 && sgd_variant != SGDAdaGrad
                                              && sgd_variant != SGDRotating) {
      nu /= 2;
      std::cout << "New Nu: " << nu << std::endl;
    }
    else if (completed_iterations % 100000 == 0 && sgd_variant == SGDRotating) {
      nu /= 2;
      if (completed_iterations % 2000000 == 0) {
        nu = rotating_base_nu;
        focus = rng() % kNumFeatures;
      }
    }
  }
  std::cout << "Training completed!" << std::endl;
}


void Train(bool from_scratch) {
  if (settings::kTrainFromScratch) {
    SGDVariantTrain<settings::kGMMk, SGDNormal>(from_scratch);
  }
  else {
    //SGDMarginVariantTrain<settings::kGMMk, SGDNormal>(false);
    SGDVariantRL<SGDAdam, false>();
  }
}


void CheckVariableInfluence() {
  std::vector<double> variable_influences(kNumFeatures);
  std::vector<double> set_influence_abs_sum(kFeatureInfos.size());
  std::vector<double> set_influence_squared_sum(kFeatureInfos.size());
  Vec<double, settings::kGMMk> mixture_proportions;
  std::vector<Game> games = data::LoadGames();
  long num_positions = 0;
  for (long index = 0; index < games.size(); index++) {
    if (false) {
      std::cout << "game length:" << games[index + 1].moves.size() << std::endl;
      std::cout << "game length:" << games[index].moves.size() << std::endl;
      std::cout << "moves: " << std::flush;
      for (Move move : games[index].moves) {
        std::cout << parse::MoveToString(move) << " " << std::flush;
      }
      std::cout << std::endl;
    }
    games[index].set_to_position_after(0);

    while (games[index].board.get_num_made_moves() < games[index].moves.size()) {
      games[index].forward();
      if (false){
        games[index].board.Print();
      }
      std::vector<int> features =
          ScoreBoard<std::vector<int> >(games[index].board);
      Vec<double, settings::kGMMk> probabilities = gmm_main.GetWeightedProbabilities(
          GetBoardPhaseVec(games[index].board));
      double set_influence = 0;
      int idx = 0;
      for (size_t i = 0; i < kNumFeatures; i++) {
        if (i == kFeatureInfos[idx + 1].idx) {
          set_influence_squared_sum[idx] += set_influence * set_influence;
          set_influence_abs_sum[idx] += std::abs(set_influence);
          idx++;
          set_influence = 0;
        }
        double variable_influence = 0;
        for (size_t j = 0; j < settings::kGMMk; j++) {
          variable_influence += feature_GMM_score_values[i][j] * features[i]
                                * probabilities[j];
          set_influence += feature_GMM_score_values[i][j] * features[i]
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
  for (int idx = 0; idx < kFeatureInfos.size(); idx++) {
    double mean = (set_influence_abs_sum[idx] / num_positions);
    sfile  << mean
        << " +- " << std::sqrt((set_influence_squared_sum[idx] / num_positions)- mean * mean)
        << " <-- " << kFeatureInfos[idx].info << std::endl;

  }
  sfile.flush();
  sfile.close();
  std::ofstream file("params/influence");
  int idx = 0;
  for (int i = 0; i < kNumFeatures; i++) {
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
  std::vector<Vec<double, settings::kGMMk>> feature_magnitudes(kNumFeatures);
  Vec<double, settings::kGMMk> mixture_proportions;
  std::vector<Game> games = data::LoadGames();
  for (long index = 0; index < games.size(); index++) {
    if (false) {
      std::cout << "game length:" << games[index + 1].moves.size() << std::endl;
      std::cout << "game length:" << games[index].moves.size() << std::endl;
      std::cout << "moves: " << std::flush;
      for (Move move : games[index].moves) {
        std::cout << parse::MoveToString(move) << " " << std::flush;
      }
      std::cout << std::endl;
    }
    games[index].set_to_position_after(0);

    while (games[index].board.get_num_made_moves() < games[index].moves.size()) {
      games[index].forward();
      if (false){
        games[index].board.Print();
      }
      std::vector<int> features =
          ScoreBoard<std::vector<int> >(games[index].board);
      Vec<double, settings::kGMMk> probabilities = gmm_main.GetWeightedProbabilities(
          GetBoardPhaseVec(games[index].board));
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
  int idx = 0;
  for (int i = 0; i < kNumFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    for (int j = 0; j < settings::kGMMk; j++) {
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
  const int max_depth = 10, n_score_bins = 40, score_bin_size = 200,
            n_dif_bins = 120, dif_bin_size = 40;
  std::vector<Vec<double, settings::kGMMk> > x(max_depth), x_squared(max_depth), count(max_depth);
  Vec<double, max_depth> n, xs2, xs;
  std::vector<Array2d<long, n_score_bins, n_dif_bins> > histogram(max_depth);
  for (int i = 0; i < max_depth; i++) {
    for (int j = 0; j < n_score_bins; j++) {
      for (int k = 0; k < n_dif_bins; k++) {
        histogram[i][j][k] = 0;
      }
    }
  }
  for (int i = 0; i < games.size(); i++) {
    Game game = games[i];
    game.set_to_position_after(0);
    std::vector<bool> quiet_move(game.moves.size());
    std::vector<bool> in_check(game.moves.size());
    std::vector<Vec<double, settings::kGMMk> > mixture_probabilities(game.moves.size());
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

    for (int depth = 0; depth < max_depth; depth++) {
      for (int i = 6; i < quiet_move.size() - depth - 2; i++) {
        if (!in_check[i]) {
          Score score_bin_idx = scores[i];
          score_bin_idx += score_bin_size / 2;
          score_bin_idx /= score_bin_size;
          score_bin_idx += n_score_bins / 2;
          score_bin_idx = std::max(0, (int)score_bin_idx);
          score_bin_idx = std::min(n_score_bins - 1, (int)score_bin_idx);

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
          dif = std::max(0, (int)dif);
          dif = std::min(n_dif_bins - 1, (int)dif);
          histogram[depth][score_bin_idx][dif]++;
        }
      }
    }
    if ((i + 1) % 10000 == 0) {
      std::cout << "Evaluated " << (i+1) << " games!" << std::endl;
    }
  }
  std::cout << "Mixture Based Statistics" << std::endl;
  for (int depth = 0; depth < max_depth; depth++) {
    Vec<double, settings::kGMMk> x_mean = x[depth] / count[depth];
    Vec<double, settings::kGMMk> variance =
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
  for (int depth = 0; depth < max_depth; depth++) {
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
  for (int i = 0; i < n_score_bins; i++) {
    vhistogram[i][0] = (i - (n_score_bins / 2)) * score_bin_size;
    quantiles[i][0] = (i - (n_score_bins / 2)) * score_bin_size;
  }
  for (int depth = 0; depth < max_depth; depth++) {
    for (int i = 0; i < n_score_bins; i++) {
      std::cout << i << ": ";
      long sum = 0;
      for (int j = 0; j < n_dif_bins; j++) {
        vhistogram[i][j + 1] = histogram[depth][i][j];
        sum += histogram[depth][i][j];
      }
      for (int j = 0; j < n_dif_bins; j++) {
        vhistogram[i][j + 1] /= sum;
      }
      csum[i][0] = vhistogram[i][0 + 1];
      for (int j = 1; j < n_dif_bins; j++) {
        csum[i][j] = csum[i][j-1] + vhistogram[i][j + 1];
      }
      std::cout << i << ",0 = " << csum[i][0] << "|" << vhistogram[i][0 + 1] << " ";
      for (int j = 1; j < n_dif_bins; j++) {
        std::cout << i << "," << j << " = " << csum[i][j] << "|" << vhistogram[i][j + 1] << " ";
        for (int idx = 0; idx < quantile_breakpoints.size(); idx++) {
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

Vec<double, settings::kGMMk> BoardMixtureProbability(const Board &board) {
  return gmm_main.GetWeightedProbabilities(GetBoardPhaseVec(board));
}

Score GetPawnBaseValue(const Board &board) {
  PScore score = feature_GMM_score_values[kBaseValueIndex + kPawn];
  Vec<double, settings::kGMMk> weights =
      gmm_main.GetWeightedProbabilities(GetBoardPhaseVec(board));
  return std::round(weights.dot(score));
}

Score GetTempoValue(const Board &board) {
  PScore score = feature_GMM_score_values[kTempoBonusIndex];
  Vec<double, settings::kGMMk> weights =
      gmm_main.GetWeightedProbabilities(GetBoardPhaseVec(board));
  return std::round(weights.dot(score));
}

int ScoreToCentipawn(const Score score, const Board &board) {
  return (100 * score) / GetPawnBaseValue(board);
}

double BoardProbability(const Board &board) {
  return gmm_main.GetSampleProbability(GetBoardPhaseVec(board));
}

}

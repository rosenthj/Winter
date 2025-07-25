/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2016 Jonas Kuratli, Jonathan Maurer, Jonathan Rosenthal
 *  Copyright (C) 2017-2024 Jonathan Rosenthal
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
 * move_order.cc
 *
 *  Created on: May 27, 2018
 *      Author: Jonathan Rosenthal
 */

#include "board.h"
#include "general/feature_indexes.h"
#include "general/hardcoded_params.h"
#include "general/magic.h"
#include "general/types.h"
#include "move_order.h"
#include "search_thread.h"

#include <algorithm>
#ifdef TUNE_ORDER
#include <iomanip>
#include <iostream>
#include <fstream>
#endif

using namespace move_features;

namespace {
  std::vector<MoveScore> search_params(kNumMoveProbabilityFeatures);
  std::vector<MoveScore> search_params_in_check(kNumMoveProbabilityFeatures);
}

namespace move_order {
  
void Init() {
  for (size_t idx = 0; idx < kNumMoveProbabilityFeatures; ++idx) {
    search_params.at(idx) = hardcode::search_params[idx];
    search_params_in_check.at(idx) = hardcode::search_params_in_check[idx];
  }
}

struct Sorter {
  bool operator() (Move i, Move j) {
    return (i >> 16) > (j >> 16);
  };
};

void insertion_sort(std::vector<Move> &moves, int start_idx = 0) {
  const size_t size = moves.size();
  for (int i = start_idx + 1; i < size; ++i) {
    Move key = moves[i];
    int j = i - 1;
    while (j >= start_idx && (moves[j] >> 16) < (key >> 16)) {
      moves[j + 1] = moves[j];
      --j;
    }
    moves[j + 1] = key;
  }
}

MoveScore get_move_priority(const Move move, search::Thread &t, const Move best) {
  if (move == best)
    return 20000;
  else if (GetMoveType(move) > kCapture) {
    return 10000 + GetMoveType(move) - (GetPieceType(t.board.get_piece(GetMoveDestination(move))) / kNoPiece);
  }
  else if (GetMoveType(move) == kCapture) {
    return 1000 + 10 * GetPieceType(t.board.get_piece(GetMoveDestination(move)))
                - GetPieceType(t.board.get_piece(GetMoveSource(move)));
  }
  return t.get_history_score(t.board.get_turn(), GetMoveSource(move), GetMoveDestination(move)) / 1000;
}

void Sort(std::vector<Move> &moves, search::Thread &t, const Move best_move) {
  for (size_t i = 0; i < moves.size(); ++i) {
    moves[i] |= (get_move_priority(moves[i], t, best_move) << 16);
  }
  insertion_sort(moves);
  for (size_t i = 0; i < moves.size(); ++i) {
    moves[i] &= 0xFFFFL;
  }
}

template<bool in_check> inline MoveScore GetFeatureValue(const size_t index) {
  if (in_check)
    return search_params_in_check[index];
  return search_params[index];
}

template<bool in_check> inline void AddFeature(MoveScore &s,  const size_t index) {
  s += GetFeatureValue<in_check>(index);
}

template<bool in_check> inline void AddFeature(MoveScore &s,  const size_t index, int val) {
  s += GetFeatureValue<in_check>(index) * val;
}

inline Move get_last_move(const Board &board) {
  if (board.get_num_made_moves() > 0) {
    return board.get_last_move();
  }
  return kNullMove;
}

inline BitBoard get_passed_pawn_squares(const Board &board) {
  if (board.get_turn() == kWhite) {
    BitBoard covered = board.get_piece_bitboard(kBlack, kPawn);
    covered |= bitops::SW(covered) | bitops::SE(covered);
    return ~bitops::FillSouth(covered, ~0);
  }
  BitBoard covered = board.get_piece_bitboard(kWhite, kPawn);
  covered |= bitops::NW(covered) | bitops::NE(covered);
  return ~bitops::FillNorth(covered, ~0);
}

inline BitBoard get_pawn_attack_squares(const Board &board) {
  if (board.get_turn() == kWhite) {
    BitBoard targets = board.get_color_bitboard(kBlack) & (~board.get_piecetype_bitboard(kPawn));
    return bitops::SE(targets) | bitops::SW(targets);
  }
  BitBoard targets = board.get_color_bitboard(kWhite) & (~board.get_piecetype_bitboard(kPawn));
  return bitops::NE(targets) | bitops::NW(targets);
}

inline BitBoard get_under_threat_squares(const Board &board) {
  if (board.get_num_made_moves() == 0) {
    return 0;
  }
  BitBoard pot_targets;
  const Move last_move = board.get_last_move();
  const PieceType moved_piece = GetPieceType(board.get_piece(GetMoveDestination(last_move)));
  if (moved_piece == kPawn) {
    BitBoard move_des = GetSquareBitBoard(GetMoveDestination(last_move));
    if (board.get_turn() == kWhite) {
      pot_targets = bitops::SE(move_des) | bitops::SW(move_des);
    }
    else {
      pot_targets = bitops::NE(move_des) | bitops::NW(move_des);
    }
  }
  else {
    pot_targets = magic::GetAttackMap(moved_piece, GetMoveDestination(last_move), board.get_all_pieces());
  }
  pot_targets &= board.get_color_bitboard(board.get_turn());
  BitBoard targets = 0;
  while (pot_targets) {
    BitBoard lsb = bitops::GetLSB(pot_targets);
    if (!board.NonNegativeSEESquare(bitops::NumberOfTrailingZeros(lsb))) {
      targets |= lsb;
    }
    bitops::PopLSB(pot_targets);
  }
  return targets;
}

struct MoveOrderInfo {
  const Move tt_entry;
  const Move last_move;
  const std::array<BitBoard, 6> direct_checks;
  const std::array<BitBoard, 6> taboo_squares;
  const BitBoard passed_pawn_squares;
  const BitBoard pawn_attack_squares;
  const BitBoard under_threat;
  MoveOrderInfo(const Board &board, Move tt_entry_ = kNullMove) :
    tt_entry(tt_entry_),
    last_move(get_last_move(board)),
    direct_checks(board.GetDirectCheckingSquares()),
    taboo_squares(board.GetTabooSquares()),
    passed_pawn_squares(get_passed_pawn_squares(board)),
    pawn_attack_squares(get_pawn_attack_squares(board)),
    under_threat(get_under_threat_squares(board))
      { }
};

enum SlidingCheckType {
  kNoSlideCheck = 0, kBishopCheck, kRookCheck, kQueenDiagCheck, kQueenRooklikeCheck
};

template<bool in_check>
MoveScore GetMoveWeight(const Move move, search::Thread &t, const MoveOrderInfo &info) {
  MoveScore move_weight = 160000000;
  if (move == info.tt_entry) {
    return (1 << 15) - 1;
  }
  int num_made_moves = t.board.get_num_made_moves();
  if (move == t.killers[num_made_moves][0]) {
    AddFeature<in_check>(move_weight, kPWIKiller);
  }
  else if (move == t.killers[num_made_moves][1]) {
    AddFeature<in_check>(move_weight, kPWIKiller + 1);
  }
  if (t.board.get_num_made_moves() > 0 && t.board.get_last_move() != kNullMove) {
    const Square last_destination = GetMoveDestination(t.board.get_last_move());
    PieceType last_moved_piece = GetPieceType(t.board.get_piece(last_destination));
    if (move == t.counter_moves[t.board.get_turn()][last_moved_piece][last_destination]) {
      AddFeature<in_check>(move_weight, kPWICounterMove);
    }
    if (GetMoveType(move) < kCapture) {
      const Color color = t.board.get_turn();
      const PieceType moving_piece = GetPieceType(t.board.get_piece(GetMoveSource(move)));
      const Square source = GetMoveSource(move);
      const Square destination = GetMoveDestination(move);
      const int32_t score = t.get_continuation_score<1>(last_moved_piece, last_destination,
                                         moving_piece, destination);
      AddFeature<in_check>(move_weight, kPWICMH, score);
      AddFeature<in_check>(move_weight, kPWICMH + 1, t.get_continuation_score<2>(move));
      AddFeature<in_check>(move_weight, kPWIHistory, t.get_history_score(color, source, destination));
    }
  }
  const PieceType moving_piece = GetPieceType(t.board.get_piece(GetMoveSource(move)));
  PieceType target = GetPieceType(t.board.get_piece(GetMoveDestination(move)));
  const MoveType move_type = GetMoveType(move);
  if (move_type >= kCapture && (target < moving_piece || target == kNoPiece)) {
    if (!t.board.NonNegativeSEE(move)) {
      AddFeature<in_check>(move_weight, kPWISEE);
    }
  }
  target -= target / kKing;//The target cannot be a king, so we ignore that case.
  AddFeature<in_check>(move_weight, kPWIPieceTypeXTargetPieceType
                            + (moving_piece * 6) + target);
  AddFeature<in_check>(move_weight, kPWIMoveType + GetMoveType(move));
  if (move_type == kNormalMove || move_type == kDoublePawnMove) {
    if (GetSquareBitBoard(GetMoveSource(move)) & info.under_threat) {
      AddFeature<in_check>(move_weight, kPWIPieceUnderAttack + 1);
    }
    if (moving_piece == kPawn) {
      if (true || !in_check) {
        const BitBoard des = GetSquareBitBoard(GetMoveDestination(move));
        if (des & info.passed_pawn_squares) {
          int rank = GetSquareY(GetMoveDestination(move));
          rank = rank + (t.board.get_turn() * (7 - 2 * rank)) - 2;
          AddFeature<in_check>(move_weight, kPWIPassedRankDestination + rank);
        }
        else {
          int rank = GetSquareY(GetMoveDestination(move));
          rank = rank + (t.board.get_turn() * (7 - 2 * rank)) - 2;
          AddFeature<in_check>(move_weight, kPWIPawnRankDestination + rank);
        }
        if (des & info.pawn_attack_squares) {
          AddFeature<in_check>(move_weight, kPWIPawnAttack);
        }
      }
    }
    else if (moving_piece == kKnight) {
      AddFeature<in_check>(move_weight, kPWIKnightMoveSource + kPSTindex[GetMoveSource(move)]);
      AddFeature<in_check>(move_weight, kPWIKnightMoveDestination + kPSTindex[GetMoveDestination(move)]);
    }
    else {
      AddFeature<in_check>(move_weight, kPWIMoveSource + kPSTindex[GetMoveSource(move)]);
      AddFeature<in_check>(move_weight, kPWIMoveDestination + kPSTindex[GetMoveDestination(move)]);
// Bishop relative to king position tested slightly negative. Presumably there is no major benefit
// and the Elo cost is purely the slowdown.
//      if (moving_piece == kBishop) {
//        Square enemy_king_square = bitops::NumberOfTrailingZeros(board.get_piece_bitboard(board.get_not_turn(), kKing));
//        int relative_x = GetSquareX(GetMoveDestination(move)) - GetSquareX(enemy_king_square) + 7;
//        int relative_y = GetSquareY(GetMoveDestination(move)) - GetSquareY(enemy_king_square) + 7;
//        AddFeature<T>(move_weight, kPWIBishopRelativeToKing + relative_king_map[relative_x + 15 * relative_y]);
//      }
    }
//    if (in_check && check_type != kNoSlideCheck) {
//      AddFeature<T, in_check>(move_weight, kPWICheckInterjection + moving_piece * 4 + check_type - 1);
//    }
  }
  else {
    if (GetSquareBitBoard(GetMoveSource(move)) & info.under_threat) {
      AddFeature<in_check>(move_weight, kPWIPieceUnderAttack);
    }
  }
  if (info.last_move != kNullMove && GetMoveDestination(info.last_move) == GetMoveDestination(move)) {
    AddFeature<in_check>(move_weight, kPWICaptureLastMoved);
  }
  if (GetSquareBitBoard(GetMoveDestination(move)) & info.direct_checks[moving_piece]) {
    if (GetMoveType(move) >= kEnPassant) {
      AddFeature<in_check>(move_weight, kPWIGivesCheck);
    }
    else {
      AddFeature<in_check>(move_weight, kPWIGivesCheck + 1);
    }
  }
  if ((GetMoveType(move) == kNormalMove || GetMoveType(move) == kDoublePawnMove)
      && (GetSquareBitBoard(GetMoveDestination(move)) & info.taboo_squares[kKing])) {
    if (GetSquareBitBoard(GetMoveDestination(move)) & info.taboo_squares[moving_piece]) {
      AddFeature<in_check>(move_weight, kPWITabooDestination);
    }
    else if (!t.board.NonNegativeSEE(move)) {
      AddFeature<in_check>(move_weight, kPWISEE + 1);
    }
  }
  AddFeature<in_check>(move_weight, kPWIForcingChanges + IsMoveForcing(move) + 2 * IsMoveForcing(info.last_move));
  return move_weight / 16000;
}

// Sorten moves according to weights given by some classifier
void SortML(std::vector<Move> &moves, search::Thread &t,
                 const Move best_move, const size_t start_idx) {
  MoveOrderInfo info(t.board, best_move);

  //Move ordering is very different if we are in check. Eg a queen move not capturing anything is less likely.
  if (t.board.InCheck()) {
    for (size_t i = start_idx; i < moves.size(); ++i) {
      moves[i] |= (GetMoveWeight<true>(moves[i], t, info) << 16);
    }
  }
  else {
    for (size_t i = start_idx; i < moves.size(); ++i) {
      moves[i] |= (GetMoveWeight<false>(moves[i], t, info) << 16);
    }
  }

  insertion_sort(moves, start_idx);
  for (size_t i = start_idx; i < moves.size(); ++i) {
    moves[i] &= 0xFFFFL;
  }
}

#ifdef TUNE_ORDER

void SetWeight(size_t idx, MoveScore value) {
  if (idx < kNumMoveProbabilityFeatures) {
    search_params[idx] = value;
  }
  else if (idx < (2 * kNumMoveProbabilityFeatures)) {
    search_params_in_check[idx - kNumMoveProbabilityFeatures] = value;
  }
}

MoveScore GetWeight(size_t idx) {
  if (idx < kNumMoveProbabilityFeatures) {
    return search_params.at(idx);
  }
  else if (idx < (2 * kNumMoveProbabilityFeatures)) {
    return search_params_in_check.at(idx - kNumMoveProbabilityFeatures);
  }
  std::cout << "Error detected in move_order::GetWeight(" << idx << ")"
            << std::endl;
  return 0;
}

void PrintOptions() {
  for (size_t idx = 0; idx < 2*kNumMoveProbabilityFeatures; ++idx) {
    std::cout << "option name order_" << idx << " type spin default "
              << GetWeight(idx) << " min -1000 max 1000" << std::endl;
  }
}

void OptionsToFile() {
  std::ofstream f;
  f.open("./order_params.csv");
  for (size_t idx = 0; idx < 2*kNumMoveProbabilityFeatures; ++idx) {
    f << "order_" << idx << ", int, " << GetWeight(idx);
    if (kNumMoveProbabilityFeatures - (idx % kNumMoveProbabilityFeatures) > 3) {
      f << ", -16000000, 16000000, 160000, 0.002" << std::endl;
    }
    else {
      f << ", -1000, 1000, 10, 0.002" << std::endl;
    }
  }
  f.close();
}

void GenerateCPPHelper(const std::vector<MoveScore>& params,
                       const std::string& name,
                       std::ostream& out)               // <-- add param
{
  size_t max_len = 0;
  for (MoveScore p : params)
    max_len = std::max(max_len, std::to_string(p).length());

  out << "constexpr std::array<int, 117> " << name << " = {\n";
  size_t idx = 0;
  std::string feature_name;
  for (FeatureInfo info : kFeatureInfos) {
    while (idx < info.idx)
        out << std::setw(max_len + 2) << params[idx++]
            << ", // " << feature_name << '\n';
    feature_name = info.info;
  }
  out << "};\n\n";
}

void GenerateCPP() {
  const std::string file = "hardcoded_params.h";
  std::ofstream out(file, std::ios::trunc);
  if (!out) { std::cout << "could not open " << file << '\n'; return; }
  GenerateCPPHelper(search_params, "search_params", out);
  GenerateCPPHelper(search_params_in_check, "search_params_in_check", out);
  std::cout << "saved parameters to " << file << '\n';
}

#endif /* TUNE_ORDER */

}

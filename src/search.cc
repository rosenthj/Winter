/*
 *  Winter is a UCI chess engine.

 *
 *
 *
 *£
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
 * search.cc
 *
 *  Created on: Nov 6, 2016
 *      Author: Jonathan Rosenthal
 */

#include "search.h"
#include "evaluation.h"
#include "transposition.h"
#include "data.h"
#include "general/debug.h"
#include "general/bookkeeping.h"
#include "general/feature_indexes.h"
#include "general/hardcoded_params.h"
#include "general/magic.h"
#include "benchmark.h"
#include "search_thread.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cassert>
#include <mutex>

using namespace move_features;

namespace {

const int kPV = 0;
const int kNW = 1;

const int kNormalSearchMode = 0;
const int kSamplingSearchMode = 1;
const int kSamplingEvalMode = 2;
int kNodeCountSampleAt = 1000;
//int kNodeCountSampleEvalAt = 5000;
const int kMaxDepthSampled = 32;

int rsearch_mode;
Milliseconds rsearch_duration;
Depth rsearch_depth;

Board sampled_board;
Score sampled_alpha;
int sampled_node_type;
Depth sampled_depth;
int skip_time_check = 0;

const Array2d<Depth, 64, 64> init_lmr_reductions() {
  Array2d<Depth, 64, 64> lmr_reductions;
  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 64; j++) {
      lmr_reductions[i][j] = std::round(std::log(i+1) * std::log(j+1) / 1.6);
      lmr_reductions[i][j] = std::min(lmr_reductions[i][j], i);
    }
  }
  return lmr_reductions;
}

const Array2d<Depth, 64, 64> lmr_reductions = init_lmr_reductions();

template<int NodeType>
const Depth get_lmr_reduction(const Depth depth, const int move_number) {
  assert(depth > 0);
  if (NodeType == kPV) {
    return lmr_reductions[std::min(depth - 1, 63)][std::min(move_number, 63)] / 2;
  }
  return lmr_reductions[std::min(depth - 1, 63)][std::min(move_number, 63)];
}

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

const Vec<Score, 4> init_futility_margins() {
  Vec<Score, 4> kFutilityMargins(0);
  if (false) {
    kFutilityMargins[0] = 0;
    kFutilityMargins[1] = 413;
    kFutilityMargins[2] = 722;
    kFutilityMargins[3] = 1071;
  }
  else {
    kFutilityMargins[0] = 0;
    kFutilityMargins[1] = 500;
    kFutilityMargins[2] = 1000;
    kFutilityMargins[3] = 1500;
  }
  return kFutilityMargins;
}

const Vec<Score, 4> kFutileMargin = init_futility_margins();
const std::array<int, 5> kLMP = {0, 6, 9, 13, 18};

std::vector<int> search_weights(kNumMoveProbabilityFeatures);
std::vector<int> search_weights_in_check(kNumMoveProbabilityFeatures);

Score last_search_score = 0;

bool print_info = true;

struct Sorter {
  bool operator() (Move i, Move j) {
    return (i >> 16) > (j >> 16);
  };
};

bool SwapToFront(std::vector<Move> &moves, const Move move) {
  for (int i = 0; i < moves.size(); i++) {
    if (moves[i] == move) {
      std::swap(moves[i], moves[0]);
      return true;
    }
  }
  return false;
}

std::mt19937_64 rng;
size_t max_ply = 0, min_ply = 0;
const size_t kInfiniteNodes = 1000000000000;
size_t max_nodes = kInfiniteNodes;
size_t sample_nodes = 0;
size_t evaluation_nodes = 0;
bool fixed_search_time;

Time end_time = now();

inline bool finished(search::Thread &thread) {
  if (thread.id == 0) {
    if (skip_time_check <= 0) {
      skip_time_check = 256;
      return end_time <= now() || max_nodes < search::Threads.node_count() || search::Threads.end_search.load(std::memory_order_relaxed);
    }
    skip_time_check--;
    return search::Threads.end_search;
  }
  return search::Threads.end_search.load(std::memory_order_relaxed);
}

inline void end_search_time() {
  end_time = now();
}

inline Time get_infinite_time() {
  return now()+std::chrono::hours(24);
}

template<int Quiescent>
int32_t get_move_priority(const Move move, const Board &board, const Move best) {
  if (move == best)
    return 20000;
  else if (GetMoveType(move) > kCapture) {
    return 11000;
  }
  else if (GetMoveType(move) == kCapture) {
    return 1000 + 10 * GetPieceType(board.get_piece(GetMoveDestination(move)))
                - GetPieceType(board.get_piece(GetMoveSource(move)));
  }
  else if (Quiescent == kQuiescent) {
    return 10;
  }
//  else if (move == killers[board.get_num_made_moves()][0]) {
//    return 1001;
//  }
//  else if (move == killers[board.get_num_made_moves()][1]) {
//    return 1000;
//  }
  return 10;
}

template<int Quiescent>
void SortMoves(std::vector<Move> &moves, Board &board, const Move best_move) {
  for (unsigned int i = 0; i < moves.size(); i++) {
    moves[i] |= (get_move_priority<Quiescent>(moves[i], board, best_move) << 16);
  }
  std::sort(moves.begin(), moves.end(), Sorter());
  for (unsigned int i = 0; i < moves.size(); i++) {
    moves[i] &= 0xFFFFL;
  }
}

template<typename T>
T init() {
  return T(kNumMoveProbabilityFeatures);
}

template<> int init<int>() { return 0; }

template<typename T, bool in_check> inline
void AddFeature(T &s, const int index) {
  s[index] = 1;
}

template<> inline void AddFeature<int, false>(int &s,  const int index) {
  s += search_weights[index];
}

template<> inline void AddFeature<int, true>(int &s,  const int index) {
  s += search_weights_in_check[index];
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
  const Vec<BitBoard, 6> direct_checks;
  const Vec<BitBoard, 6> taboo_squares;
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

template<typename T, bool in_check>
T GetMoveWeight(const Move move, search::Thread &t, const MoveOrderInfo &info) {
  T move_weight = init<T>();
  if (move == info.tt_entry) {
    AddFeature<T, in_check>(move_weight, kPWIHashMove);
    return move_weight;
  }
  int num_made_moves = t.board.get_num_made_moves();
  if (move == t.killers[num_made_moves][0]) {
    AddFeature<T, in_check>(move_weight, kPWIKiller);
    //return move_weight;
  }
  else if (move == t.killers[num_made_moves][1]) {
    AddFeature<T, in_check>(move_weight, kPWIKiller + 1);
    //return move_weight;
  }
  if (t.board.get_num_made_moves() > 0 && t.board.get_last_move() != kNullMove) {
    const Square last_destination = GetMoveDestination(t.board.get_last_move());
    PieceType last_moved_piece = GetPieceType(t.board.get_piece(last_destination));
    if (move == t.counter_moves[t.board.get_turn()][last_moved_piece][last_destination]) {
      AddFeature<T, in_check>(move_weight, kPWICounterMove);
    }
  }
  const PieceType moving_piece = GetPieceType(t.board.get_piece(GetMoveSource(move)));
  PieceType target = GetPieceType(t.board.get_piece(GetMoveDestination(move)));
  const MoveType move_type = GetMoveType(move);
  if (move_type >= kCapture && (target < moving_piece || target == kNoPiece)) {
    if (!t.board.NonNegativeSEE(move)) {
      AddFeature<T, in_check>(move_weight, kPWISEE);
    }
  }
  target -= target / kKing;//The target cannot be a king, so we ignore that case.
  AddFeature<T, in_check>(move_weight, kPWIPieceTypeXTargetPieceType
                            + (moving_piece * 6) + target);
  AddFeature<T, in_check>(move_weight, kPWIMoveType + GetMoveType(move));
  if (move_type == kNormalMove || move_type == kDoublePawnMove) {
    if (GetSquareBitBoard(GetMoveSource(move)) & info.under_threat) {
      AddFeature<T, in_check>(move_weight, kPWIPieceUnderAttack + 1);
    }
    if (moving_piece == kPawn) {
      if (true || !in_check) {
        const BitBoard des = GetSquareBitBoard(GetMoveDestination(move));
        if (des & info.passed_pawn_squares) {
          int rank = GetSquareY(GetMoveDestination(move));
          rank = rank + (t.board.get_turn() * (7 - 2 * rank)) - 2;
          AddFeature<T, in_check>(move_weight, kPWIPassedRankDestination + rank);
        }
        else {
          int rank = GetSquareY(GetMoveDestination(move));
          rank = rank + (t.board.get_turn() * (7 - 2 * rank)) - 2;
          AddFeature<T, in_check>(move_weight, kPWIPawnRankDestination + rank);
        }
        if (des & info.pawn_attack_squares) {
          AddFeature<T, in_check>(move_weight, kPWIPawnAttack);
        }
      }
    }
    else if (moving_piece == kKnight) {
      AddFeature<T, in_check>(move_weight, kPWIKnightMoveSource + kPSTindex[GetMoveSource(move)]);
      AddFeature<T, in_check>(move_weight, kPWIKnightMoveDestination + kPSTindex[GetMoveDestination(move)]);
    }
    else {
      AddFeature<T, in_check>(move_weight, kPWIMoveSource + kPSTindex[GetMoveSource(move)]);
      AddFeature<T, in_check>(move_weight, kPWIMoveDestination + kPSTindex[GetMoveDestination(move)]);
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
      AddFeature<T, in_check>(move_weight, kPWIPieceUnderAttack);
    }
  }
  if (info.last_move != kNullMove && GetMoveDestination(info.last_move) == GetMoveDestination(move)) {
    AddFeature<T, in_check>(move_weight, kPWICaptureLastMoved);
  }
  if (GetSquareBitBoard(GetMoveDestination(move)) & info.direct_checks[moving_piece]) {
    if (GetMoveType(move) >= kEnPassant) {
      AddFeature<T, in_check>(move_weight, kPWIGivesCheck);
    }
    else {
      AddFeature<T, in_check>(move_weight, kPWIGivesCheck + 1);
    }
  }
  if ((GetMoveType(move) == kNormalMove || GetMoveType(move) == kDoublePawnMove)
      && (GetSquareBitBoard(GetMoveDestination(move)) & info.taboo_squares[kKing])) {
    if (GetSquareBitBoard(GetMoveDestination(move)) & info.taboo_squares[moving_piece]) {
      AddFeature<T, in_check>(move_weight, kPWITabooDestination);
    }
    else if (!t.board.NonNegativeSEE(move)) {
      AddFeature<T, in_check>(move_weight, kPWISEE + 1);
    }
  }
  AddFeature<T, in_check>(move_weight, kPWIForcingChanges + IsMoveForcing(move) + 2 * IsMoveForcing(info.last_move));
  return move_weight;
}

//SlidingCheckType GetSlidingCheckType(const Board &board) {
//  const Square king_square = bitops::NumberOfTrailingZeros(board.get_piece_bitboard(board.get_turn(), kKing));
//  const BitBoard all_pieces = board.get_all_pieces();
//  const BitBoard diag_access = magic::GetAttackMap<kBishop>(king_square, all_pieces);
//  if (diag_access & board.get_piece_bitboard(board.get_not_turn(), kBishop)) {
//    return kBishopCheck;
//  }
//  else if (diag_access & board.get_piece_bitboard(board.get_not_turn(), kQueen)) {
//    return kQueenDiagCheck;
//  }
//  else {
//    const BitBoard vertical_or_horizontal_access = magic::GetAttackMap<kRook>(king_square, all_pieces);
//    if (vertical_or_horizontal_access & board.get_piece_bitboard(board.get_not_turn(), kRook)) {
//      return kRookCheck;
//    }
//    else if (vertical_or_horizontal_access & board.get_piece_bitboard(board.get_not_turn(), kQueen)) {
//      return kQueenRooklikeCheck;
//    }
//  }
//  return kNoSlideCheck;
//}

// Sorten moves according to weights given by some classifier
void SortMovesML(std::vector<Move> &moves, search::Thread &t, const Move best_move = kNullMove) {
  MoveOrderInfo info(t.board, best_move);

  //Move ordering is very different if we are in check. Eg a queen move not capturing anything is less likely.
  if (t.board.InCheck()) {
    for (unsigned int i = 0; i < moves.size(); i++) {
      moves[i] |= ((10000 + GetMoveWeight<int, true>(moves[i], t, info)) << 16);
    }
  }
  else {
    for (unsigned int i = 0; i < moves.size(); i++) {
      moves[i] |= ((10000 + GetMoveWeight<int, false>(moves[i], t, info)) << 16);
    }
  }

  std::sort(moves.begin(), moves.end(), Sorter());
  for (unsigned int i = 0; i < moves.size(); i++) {
    moves[i] &= 0xFFFFL;
  }
}

// Recursively build PV from TT up to param depth
void build_pv(Board &board, std::vector<Move> &pv, const Depth depth) {
  if (depth == 0) {
    return;
  }
  table::Entry entry = table::GetEntry(board.get_hash());
  bool entry_verified = table::ValidateHash(entry, board.get_hash());

  if (entry_verified) {
    std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
    for (Move move : moves) {
      if ((move == entry.get_best_move() && entry_verified)) {
        pv.emplace_back(move);
        board.Make(move);
        build_pv(board, pv, depth-1);
        board.UnMake();
        return;
      }
    }
  }
}

inline bool sufficient_bounds(const Board &board, const table::Entry &entry,
                              const Score alpha, const Score beta,
                              const Depth depth) {
  Score score = entry.get_score(board);
  return entry.depth >= depth
      && ((entry.bound == kExactBound)
          || (entry.bound == kLowerBound && score >= beta)
          || (entry.bound == kUpperBound && score <= alpha));
}

inline bool is_mate_score(const Score score) {
  return (score < kMinScore + 2000) || (score > kMaxScore - 2000);
}

inline bool is_null_move_allowed(const Board &board, const Depth depth) {
  return settings::kUseNullMoves && depth > 1
      && board.has_non_pawn_material(board.get_turn());
      //&& board.get_phase() > 1 * piece_phases[kQueen];// && !board.InCheck();
}

//This tested negative, may revisit in the future.
inline bool cutoff_is_prefetchable(Board &board, const Score alpha, const Score beta,
                                const Depth depth, const std::vector<Move> &moves) {
  for (Move move : moves) {
    board.Make(move);
    if (alpha >= 0 && board.IsDraw()) {
      board.UnMake();
      return true;
    }
    table::Entry entry = table::GetEntry(board.get_hash());
    if (table::ValidateHash(entry,board.get_hash())
        && entry.get_score(board) <= alpha
        && sufficient_bounds(board, entry, alpha, beta, depth)) {
      board.UnMake();
      return true;
    }
    board.UnMake();
  }
  return false;
}

}

namespace search {

std::vector<Move> GetSortedMovesML(Board &board) {
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  Threads.main_thread->board.SetToSamePosition(board);
  SortMovesML(moves, *Threads.main_thread);
  return moves;
}

uint64_t Perft(Board &board, Depth depth) {
  if (depth <= 0) {
    return 1;
  }
  if (depth == 1) {
    return board.GetMoves<kNonQuiescent>().size();
  }
  uint64_t perft_sum = 0;
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  for (Move move : moves) {
    board.Make(move);
    perft_sum += Perft(board, depth-1);
    board.UnMake();
  }
  return perft_sum;
}

Board get_sampled_board() {
  return sampled_board;
}

Score SQSearch(Board &board, Score alpha, Score beta) {
  if (!board.InCheck()) {
    Score static_eval = evaluation::ScoreBoard(board);
    if (static_eval >= beta) {
      return beta;
    }
    alpha = std::max(static_eval, alpha);
  }
  std::vector<Move> moves = board.GetMoves<kQuiescent>();
  if (board.InCheck() && moves.size() == 0) {
    return kMinScore+board.get_num_made_moves();
  }
  SortMoves<kQuiescent>(moves, board, 0);

  for (Move move : moves) {
    board.Make(move);
    Score score = -SQSearch(board, -beta, -alpha);
    board.UnMake();
    if (score >= beta) {
      return beta;
    }
    alpha = std::max(score, alpha);
  }
  return alpha;
}

//Simplified Q-Search with no pruning features or TT-access
Score SQSearch(Board &board) {
  return SQSearch(board, kMinScore, kMaxScore);
}


template<int Mode>
Score QuiescentSearch(Thread &t, Score alpha, Score beta) {
  t.nodes++;
  //Update max ply reached in search
  if (t.max_depth < t.board.get_num_made_moves()) {
    t.max_depth = t.board.get_num_made_moves();
  }

  //End search immediately if trivial draw is reached
  if (t.board.IsTriviallyDrawnEnding()) {
    return 0;
  }

  //TT probe
  table::Entry entry = table::GetEntry(t.board.get_hash());
  bool valid_hash = table::ValidateHash(entry,t.board.get_hash());
  if (valid_hash && sufficient_bounds(t.board, entry, alpha, beta, 0)) {
    return entry.get_score(t.board);
  }

  //Static evaluation
  bool in_check = t.board.InCheck();
  Score static_eval = kMinScore;
  if (!in_check) {
//    if (Mode == kSamplingEvalMode) {
//      evaluation_nodes++;
//      if (evaluation_nodes == kNodeCountSampleEvalAt) {
//        sampled_board.SetToSamePosition(board);
//        end_search_time();
//      }
//    }

    static_eval = evaluation::ScoreBoard(t.board);
    if (valid_hash && entry.bound == kLowerBound && static_eval < entry.get_score(t.board)) {
      static_eval = entry.get_score(t.board);
    }

    //Stand pat
    if (static_eval >= beta) {
      return static_eval;
    }

    //Update alpha in PV nodes if possible
    if (static_eval > alpha) {
      alpha = static_eval;
    }
  }

  //Get moves
  std::vector<Move> moves = t.board.GetMoves<kQuiescent>();

  if (moves.size() == 0) {
    if (in_check) {
      //In check all moves are generated, so if we have no moves in check it is mate.
      return kMinScore+t.board.get_num_made_moves();
    }
    return alpha;
  }

  //Sort move list
  if (table::ValidateHash(entry,t.board.get_hash())) {
    SortMoves<kQuiescent>(moves, t.board, entry.get_best_move());
    //SortMovesML(moves, board, entry.best_move);
  }
  else {
    SortMoves<kQuiescent>(moves, t.board, 0);
    //SortMovesML(moves, board, 0);
  }

  //Move loop
  for (Move move : moves) {
    //SEE pruning
    //Exception for checking moves: -16.03 +/- 10.81
    if (!in_check && GetMoveType(move) != kEnPassant && !t.board.NonNegativeSEE(move)) {
        continue;
    }

    //Make move, search and unmake
    t.board.Make(move);
    Score score = -QuiescentSearch<Mode>(t, -beta, -alpha);
    t.board.UnMake();

    //Return beta if we fail high
    if (score >= beta) {
      //table::SaveEntry(board, move, score, 0);
      return beta;
    }

    //Update alpha in PV nodes if possible
    if (score > alpha) {
      alpha = score;
    }
  }

  //Return the lower bound after all has been said and done.
  return alpha;
}

inline Score get_futility_margin(Depth depth, Score score) {
  return kFutileMargin[depth];
}

Score sample_node_and_return_alpha(const Board &board, const Depth depth,
                                   const int node_type, const Score alpha) {
  sampled_board.SetToSamePosition(board);
  sampled_depth = depth;
  sampled_node_type = node_type;
  sampled_alpha = alpha;
  end_search_time();
  return alpha;
}

inline void bookkeeping_log(int NodeType, const Board &board, Move tt_entry,
                            Depth depth, int move_number, int expected_node,
                            bookkeeping::Trigger trigger) {
  if (settings::bookkeeping_active) {
    bookkeeping::InfoContainer info;
    info.NodeType = NodeType;
    info.depth = depth;
    info.expected_node = expected_node;
    info.min_ply = min_ply;
    info.move_number = move_number;
    info.tt_entry = tt_entry;
    info.trigger = trigger;
    bookkeeping::log_info(board, info);
  }
}

template<int NodeType, int Mode>
Score AlphaBeta(Thread &t, Score alpha, Score beta, Depth depth, int expected_node = 1) {
  assert(beta > alpha);
  assert(beta == alpha + 1 || NodeType != kNW);

  const Score original_alpha = alpha;

  //Immediately return 0 if we detect a draw.
  if (t.board.IsDraw() || (settings::kRepsForDraw == 3 && t.board.CountRepetitions(min_ply) >= 2)) {
    t.nodes++;
    return 0;
  }

  //We drop to QSearch if we run out of depth.
  if (depth <= 0) {
    if (!settings::kUseQS) {
      t.nodes++;
      return evaluation::ScoreBoard(t.board);
    }
    return QuiescentSearch<Mode>(t, alpha, beta);
  }

  // To avoid counting nodes twice if all we do is fall through to QSearch,
  // we wait until here to count this node.
  t.nodes++;

  //Transposition Table Probe
  table::Entry entry = table::GetEntry(t.board.get_hash());
  bool valid_entry = table::ValidateHash(entry,t.board.get_hash());
  if (NodeType != kPV && valid_entry
      && sufficient_bounds(t.board, entry, alpha, beta, depth) ) {
    return entry.get_score(t.board);
  }

  const bool in_check = t.board.InCheck();
  Score static_eval = alpha;
  t.set_static_score(kNoScore);
  bool strict_worsening = false;

  //Speculative pruning methods
  if (NodeType == kNW && beta > kMinScore + 2000 && alpha < kMaxScore - 2000 && !in_check) {

    //Set static eval from board and TT entry.
    if (valid_entry) {
      if (entry.bound == kExactBound) {
        static_eval = entry.get_score(t.board);
      }
      else {
        static_eval = evaluation::ScoreBoard(t.board);
        if (entry.bound == kLowerBound && static_eval < entry.get_score(t.board)) {
          static_eval = entry.get_score(t.board);
        }
      }
    }
    else {
      static_eval = evaluation::ScoreBoard(t.board);
    }
    t.set_static_score(static_eval);
    strict_worsening = t.strict_worsening();

    //Static Null Move Pruning
    if (NodeType == kNW && depth <= 5) {
      Score margin = 330 * depth;
      if (settings::kUseScoreBasedPruning && static_eval > beta + margin
          && t.board.get_phase() > 1 * piece_phases[kQueen]) {
        return beta;
      }
    }

    //Null Move Pruning
    if (static_eval >= beta && is_null_move_allowed(t.board, depth)) {
      t.board.Make(kNullMove);
      const Depth R = 3 + depth / 5;
      Score score = -AlphaBeta<kNW, Mode>(t, -beta, -alpha,
                                    depth - R, (expected_node & (~0x1)) + 2);
      t.board.UnMake();
      if (score >= beta) {
        return beta;
      }
    }
  }

  //Get move list and return result if there are no legal moves
  std::vector<Move> moves = t.board.GetMoves<kNonQuiescent>();
  if (moves.size() == 0) {
    if (in_check) {
      return kMinScore+t.board.get_num_made_moves();
    }
    return 0;
  }

//  if (Mode == kSamplingSearchMode && NodeType == kNW && depth <= kMaxDepthSampled) {
//    sample_nodes++;
//    if (sample_nodes >= kNodeCountSampleAt && (in_check || sample_nodes >= kNodeCountSampleAt + 10)) {
//      return sample_node_and_return_alpha(t.board, depth, NodeType, alpha);
//    }
//  }

  Move tt_entry = kNullMove;
  if (valid_entry) {
    tt_entry = entry.get_best_move();
  }

  bool moves_sorted = false, swapped = false;
  if (tt_entry != kNullMove) {
    swapped = SwapToFront(moves, tt_entry);
    if (swapped && tt_entry != moves[0]) {
      debug::Error("SwapToFront failed");
    }
  }
  if (!swapped) {
    SortMovesML(moves, t, tt_entry);
    moves_sorted = true;
  }

  Move best_local_move = moves[0];
  if (NodeType == kPV && moves.size() == 1) {
    depth++;
  }

  //Init checking squares for efficient detection of checking moves.
  //Moves detected always give check, but only direct checks are detected.
  //Ie no checks from special moves or discovered checks.
  Vec<BitBoard, 6> checking_squares = t.board.GetDirectCheckingSquares();

  //Move loop
  for (unsigned int i = 0; i < moves.size(); i++) {
    if (i == 1 && !moves_sorted) {
      SortMovesML(moves, t, tt_entry);
      moves_sorted = true;
    }
    const Move move = moves[i];
    Depth reduction = 0;

    if (!in_check && !(checking_squares[GetPieceType(t.board.get_piece(GetMoveSource(move)))]
                              & GetSquareBitBoard(GetMoveDestination(move)))) {
      //Late Move Pruning
      assert(depth > 0);
      if (NodeType == kNW && depth < kLMP.size() && (i >= kLMP[depth])
          && GetMoveType(move) < kEnPassant) {
        continue;
      }

      //Late Move Reduction factor
      reduction = get_lmr_reduction<NodeType>(depth, i);
      if (GetMoveType(move) > kDoublePawnMove) {
        reduction = (2 * reduction) / 3;
      }
      if (!strict_worsening && reduction > 2) {
        reduction = (4 * reduction) / 5;
      }
      assert(reduction < depth);

      //Futility Pruning
      if (NodeType == kNW && settings::kUseScoreBasedPruning
          && depth - reduction <= 3 && static_eval < (alpha - get_futility_margin(depth - reduction, static_eval))
          && GetMoveType(move) < kEnPassant) {
        continue;
      }
    }

    //Make moves, search and unmake
    t.board.Make(move);
    Score score;
    if (i == 0) {
      //First move gets searched at full depth and window
      score = -AlphaBeta<NodeType, Mode>(t, -beta, -alpha, depth - 1, expected_node ^ 0x1);
    }
    else {
      //Assume we have searched the best move already and search with closed window and possibly reduction
      score = -AlphaBeta<kNW, Mode>(t, -(alpha+1), -alpha, depth - 1 - reduction, 1);
      //Research with full depth if our initial search indicates an improvement over Alpha
      if (score > alpha && (NodeType == kPV || reduction > 0)) {
        score = -AlphaBeta<NodeType, Mode>(t, -beta, -alpha, depth - 1, 0);
      }
    }
    assert(score >= kMinScore && score <= kMaxScore);
    t.board.UnMake();

    //Ensure we still have time and our score was not prematurely terminated
    if (finished(t)) {
      Threads.end_search = true;
      return alpha;
    }

    //Check if search failed high
    if (score >= beta) {
      //Save TT entry
      table::SaveEntry(t.board, move, score, depth);

      //Update Killers
      if (GetMoveType(move) < kCapture) {
        int num_made_moves = t.board.get_num_made_moves();
        if (t.killers[num_made_moves][0] != move) {
          t.killers[num_made_moves][1] = t.killers[num_made_moves][0];
          t.killers[num_made_moves][0] = move;
        }
      }

      //Update Counter Move
      if (t.board.get_num_made_moves() > 0 && t.board.get_last_move() != kNullMove) {
        const Square last_destination = GetMoveDestination(t.board.get_last_move());
        PieceType last_moved_piece = GetPieceType(t.board.get_piece(last_destination));
        assert(last_moved_piece != kNoPiece);
        t.counter_moves[t.board.get_turn()][last_moved_piece][last_destination] = move;
      }
      return beta;
    }

    //In PV nodes we might be improving Alpha without breaking Beta
    if (score > alpha) {
      //Update score and expected best move
      alpha = score;
      best_local_move = move;
    }
  }
  if (alpha > original_alpha) {
    // We should save any best move which has improved alpha.
    table::SavePVEntry(t.board, best_local_move, alpha, depth);
    //table::SavePVEntry(t.board, best_local_move);
  }
  return alpha;
}

template<int Mode>
Score RootSearchLoop(Thread &t, Score original_alpha, Score beta, Depth current_depth,
                     std::vector<Move> &moves) {
  Score alpha = original_alpha;
  //const bool in_check = board.InCheck();
  if (settings::kRepsForDraw == 3 && alpha < -1 && t.board.MoveInListCanRepeat(moves)) {
    if (beta <= 0) {
      return 0;
    }
    alpha = -1;
  }
  for (int i = 0; i < moves.size(); i++) {
    t.board.Make(moves[i]);
    if (i == 0) {
      Score score = -AlphaBeta<kPV, Mode>(t, -beta, -alpha, current_depth - 1);
      if (settings::kRepsForDraw == 3 && score < 0 && t.board.CountRepetitions() >= 2) {
        score = 0;
      }
      t.board.UnMake();
      if (score >= beta) {
        return score;
      }
      alpha = std::max(score, alpha);
    }
    else {
//      Depth reduction = 0;
//      if (!in_check && !board.InCheck() && GetMoveType(moves[i]) <= kDoublePawnMove) {
//        reduction = get_lmr_reduction<kPV>(current_depth, i) / 2;
//      }
      Score score = -AlphaBeta<kNW, Mode>(t, -(alpha + 1), -alpha, current_depth - 1);
      if (score > alpha) {
        score = -AlphaBeta<kPV, Mode>(t, -beta, -alpha, current_depth - 1);
      }
      if (settings::kRepsForDraw == 3 && score < 0 && t.board.CountRepetitions() >= 2) {
        score = 0;
      }
      t.board.UnMake();
      if (finished(t)) {
        Threads.end_search = true;
        return alpha;
      }
      if (score >= beta) {
        auto it = moves.rbegin() + moves.size() - i - 1;
        std::rotate(it, it + 1, moves.rend());
        return beta;
      }
      else if (score > alpha) {
        alpha = score;
        auto it = moves.rbegin() + moves.size() - i - 1;
        std::rotate(it, it + 1, moves.rend());
      }
    }
  }
  if (t.id == 0) {
    table::SavePVEntry(t.board, moves[0], alpha, current_depth);
    //table::SavePVEntry(t.board, moves[0]);
  }
  return alpha;
}

// Estimates the standard deviation of an assumed distribution of scores.
// The true mean is assumed to be the previous search score.
inline Score estimate_Delta(const std::vector<Score> &previous_scores) {
  double weight = 1, discount = 0.9, weighted_sum = 0, sum_weights = 0;
  const Score belief = previous_scores.back();
  if (is_mate_score(belief)) {
    // If we believe we have mate, then +-10 should suffice.
    return 10;
  }
  for (int i = previous_scores.size() - 2; i >= 0; i--) {
    if (is_mate_score(previous_scores[i])) {
      // Minor hack to deal with overflow/underflow issues.
      return 250;
    }
    Score diff = belief - previous_scores[i];
    weighted_sum += diff * diff * weight;
    sum_weights += weight;
    weight *= discount;
  }
  weighted_sum += 400 * 400 * weight;
  sum_weights += weight;
  return 10 + std::round(std::sqrt(weighted_sum / sum_weights));
}

template<int Mode>
inline Score PVS(Thread &t, Depth current_depth, const std::vector<Score> &previous_scores, std::vector<Move> &moves) {
  if (current_depth <= 4) {
    return RootSearchLoop<Mode>(t, kMinScore, kMaxScore, current_depth, moves);
  }
  else {
    Score score = previous_scores.back();
    Score std_dev_estimate = estimate_Delta(previous_scores);
    Score delta = 2 * std_dev_estimate;
    Score alpha = std::max(score-delta, kMinScore);
    Score beta = std::min(score+delta, kMaxScore);
    SortMovesML(moves, t, moves[0]);
    score = RootSearchLoop<Mode>(t, alpha, beta, current_depth, moves);
    while (!finished(t) && (score <= alpha || score >= beta)) {
      if (score <= alpha) {
        if (!is_mate_score(alpha) && !is_mate_score(beta)) {
          beta = (alpha + 3 * beta) / 4;
        }
        alpha = std::max(alpha-delta, kMinScore);
        if (delta > std_dev_estimate * 8) {// We failed thrice, all bets are off.
          if (alpha > 0) {
            alpha = 0;
          }
          else {
            alpha = kMinScore;
          }
        }
      }
      else if (score >= beta) {
        if (!is_mate_score(alpha) && !is_mate_score(beta)) {
          alpha = (3 * alpha + beta) / 4;
        }
        beta = std::min(beta+delta, kMaxScore);
        if (delta > std_dev_estimate * 8) {// We failed thrice, all bets are off.
          if (beta < 0) {
            beta = 0;
          }
          else {
            beta = kMaxScore;
          }
        }
      }
      score = RootSearchLoop<Mode>(t, alpha, beta, current_depth, moves);
      delta *= 2;
    }
    return score;
  }
}

std::mutex mutex;

void Thread::search() {
  const Time begin = now();
  double time_factor = 1.0;
  current_depth = 1;
  root_height = board.get_num_made_moves();
  if (id == 0) {
    end_time = begin+rsearch_duration;
    for (Thread* t : Threads.helpers) {
      t->board.SetToSamePosition(board);
      t->moves = moves;
      t->perturb_root_moves();
      while(rng() % 2) {
        t->perturb_root_moves();
      }
      t->max_depth = t->board.get_num_made_moves();
      t->current_depth = 1;
    }
    for (Thread* t : Threads.helpers) {
      t->start_searching();
    }
  }

  Score score = evaluation::ScoreBoard(board);
  set_static_score(score);
  Move last_best = kNullMove;
  std::vector<Score> previous_scores;

  for (current_depth = 1; current_depth <= rsearch_depth; current_depth++) {
    if(finished(*this)) {
      Threads.end_search = true;
      break;
    }

    if (id != 0 && current_depth > 4) {
      std::unique_lock<std::mutex> lock(mutex);
      int count = 0;
      for (Thread* t : Threads.helpers) {
        if (current_depth <= t->current_depth) {
          count++;
        }
      }
      if (current_depth <= Threads.main_thread->current_depth) {
        count++;
      }
      if ((count - 1) >= (Threads.count()+1) / 2) {
        lock.unlock();
        continue;
      }
      lock.unlock();
      if ((rng() % 4) == 0) {
        perturb_root_moves();
      }
    }

    if (rsearch_mode == kNormalSearchMode) {
      //std::cout << "T" << id << " searching depth " << current_depth << " nodes: " << nodes << std::endl;
      score = PVS<kNormalSearchMode>(*this, current_depth, previous_scores, moves);
    }
    else if (rsearch_mode == kSamplingSearchMode) {
      score = PVS<kSamplingSearchMode>(*this, current_depth, previous_scores, moves);
    }
    else {
      assert(rsearch_mode == kSamplingEvalMode);
      score = PVS<kSamplingEvalMode>(*this, current_depth, previous_scores, moves);
    }
    //score = MTDf<Mode>(board, current_depth, score, moves);
    //score = sMTDf<Mode>(board, current_depth, score, moves);


    if(!finished(*this)) {
      last_search_score = score;
      previous_scores.emplace_back(score);
      if (id != 0) {
        continue;
      }
      std::vector<Move> pv;
      build_pv(board, pv, current_depth);
      size_t node_count = Threads.node_count();
      Time end = now();
      auto time_used = std::chrono::duration_cast<Milliseconds>(end-begin);
      if (print_info) {
        std::cout << "info "  << " depth " << current_depth
            << " seldepth " << (Threads.max_depth() - board.get_num_made_moves())
            << " time " << time_used.count()
            << " nodes " << node_count << " nps " << ((1000*node_count) / (time_used.count()+1));
        if (!is_mate_score(score)) {
          std::cout << " score cp "
              << (score / 8);
        }
        else {
          Score m_score = board.get_num_made_moves();
          if (score < 0) {
            m_score = -(score - kMinScore - m_score) / 2;
            std::cout << " score mate " << m_score;
          }
          else {
            m_score = (kMaxScore - score - m_score + 2) / 2;
            std::cout << " score mate " << m_score;
          }
        }
        std::cout << " pv";
        for (Move move : pv) {
          std::cout << " " << parse::MoveToString(move);
        }
        std::cout << std::endl;
      }
      if (!fixed_search_time) {
        if (last_best == moves[0]) {
          time_factor = std::max(time_factor * 0.9, 0.5);
          if (time_used.count() > (rsearch_duration.count() * time_factor)) {
            end_time = now();
            return;
          }
        }
        else {
          last_best = moves[0];
          time_factor = 1.0;
        }
      }
    }
  }
}

template<int Mode>
Move RootSearch(Board &board, Depth depth, Milliseconds duration = Milliseconds(24 * 60 * 60 * 1000)){
  // Measure complete search time

  max_ply = board.get_num_made_moves();
  min_ply = board.get_num_made_moves();
  Threads.reset_node_count();
  rsearch_depth = std::min(depth, settings::kMaxDepth);
  rsearch_duration = duration;
  rsearch_mode = Mode;
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  table::Entry entry = table::GetEntry(board.get_hash());
  Move tt_move = kNullMove;
  if (table::ValidateHash(entry,board.get_hash())) {
    tt_move = entry.get_best_move();
  }
  Threads.main_thread->board.SetToSamePosition(board);
  SortMovesML(moves, *Threads.main_thread, tt_move);
  Threads.main_thread->moves = moves;
  Threads.end_search = false;
  Threads.main_thread->start_searching();
  Threads.main_thread->wait_actions_over();
  Threads.end_search = true;
  for (Thread* t : Threads.helpers) {
    t->wait_actions_over();
  }
  return Threads.main_thread->moves[0];
}

void set_print_info(bool print_info_) {
  print_info = print_info_;
}

Score get_last_search_score() {
  return last_search_score;
}

Move DepthSearch(Board board, Depth depth) {
  fixed_search_time = true;
  max_nodes = kInfiniteNodes;
  return RootSearch<kNormalSearchMode>(board, depth);
}

Move FixedTimeSearch(Board board, Milliseconds duration) {
  fixed_search_time = true;
  max_nodes = kInfiniteNodes;
  return RootSearch<kNormalSearchMode>(board, 1000, duration);
}

Move TimeSearch(Board board, Milliseconds duration) {
  fixed_search_time = false;
  max_nodes = kInfiniteNodes;
  return RootSearch<kNormalSearchMode>(board, 1000, duration);
}

Move NodeSearch(Board board, size_t num_nodes) {
  fixed_search_time = true;
  max_nodes = num_nodes;
  return RootSearch<kNormalSearchMode>(board, 1000);
}

void end_search() {
  end_time = now();
}

void clear_killers_and_counter_moves() {
//  for (size_t i = 0; i < killers.size(); i++) {
//    killers[i][0] = 0;
//    killers[i][1] = 0;
//  }
//
//  for (size_t c = 0; c < 2; c++) {
//    for (PieceType pt = kPawn; pt <= kKing; pt++) {
//      for (Square sq = 0; sq < 64; sq++) {
//        counter_moves[c][pt][sq] = kNullMove;
//      }
//    }
//  }
  Threads.clear_killers_and_countermoves();
}

void CreateSearchParamDataset2(bool from_scratch) {
  if (from_scratch) {
    debug::Error("Dataset creation from scratch not supported at the moment.");
  }
  set_print_info(false);
  std::vector< std::vector <int> > samples;
  std::vector<Game> games = data::LoadGames();
  int sampled_positions = 0;
  int all_above = 0, all_below = 0, too_easy = 0;
  while (samples.size() < 1 * kMillion) {
    clear_killers_and_counter_moves();
    table::ClearTable();
    kNodeCountSampleAt = 1000 + rng() % 500;
    Game game = games[rng() % games.size()];
    if (game.moves.size() < 25) {
      continue;
    }
    game.set_to_position_after((1 * game.moves.size() / 3)
                               + (rng() % (2 * game.moves.size() / 3)) - 2);
    Board board = game.board;
    sample_nodes = 0;
    sampled_alpha = kMinScore;
    RootSearch<kSamplingSearchMode>(board, 128, Milliseconds(200));
    if (sampled_alpha == kMinScore) {
      continue;
    }
    end_time = get_infinite_time();
    std::vector<Move> moves = sampled_board.GetMoves<kNonQuiescent>();
    std::shuffle(moves.begin(), moves.end(), rng);
    Threads.main_thread->board.SetToSamePosition(sampled_board);
    SortMovesML(moves, *Threads.main_thread, kNullMove);
    std::vector<std::vector<int> > features;
    MoveOrderInfo info(sampled_board);
    if (sampled_board.InCheck()) {
      for (int i = 0; i < moves.size(); i++) {
        features.emplace_back(GetMoveWeight<std::vector<int>, true>(moves[i], *Threads.main_thread, info));
      }
    }
    else {
      for (int i = 0; i < moves.size(); i++) {
        features.emplace_back(GetMoveWeight<std::vector<int>, false>(moves[i], *Threads.main_thread, info));
      }
    }
    std::vector<Score> scores(features.size());
    int low = 0, high = 0;
    for (int i = 0; i < moves.size(); i++) {
      sampled_board.Make(moves[i]);
      Threads.main_thread->board.SetToSamePosition(sampled_board);
      Score score = -AlphaBeta<kNW, kNormalSearchMode>(*Threads.main_thread,
                                                       -(sampled_alpha+1),
                                                       -sampled_alpha,
                                                       sampled_depth - 1);
      sampled_board.UnMake();
      scores[i] = score;
      if (score > sampled_alpha) {
        high++;
        if (high > moves.size() / 2) {
          break;
        }
      }
      else {
        low++;
      }
    }
    if (high == 0) {
      all_below++;
      continue;
    }
    else if (low == 0) {
      all_above++;
      continue;
    }
    else if (high > low) {
      too_easy++;
      continue;
    }
    int i = rng() % moves.size();
    Score score;
    sampled_board.Make(moves[i]);
    if (sampled_node_type == kNW) {
      score = scores[i];
    }
    else {
      Threads.main_thread->board.SetToSamePosition(sampled_board);
      score = -AlphaBeta<kPV, kNormalSearchMode>(*Threads.main_thread,
                                                 -(sampled_alpha+1),
                                                 -sampled_alpha,
                                                 sampled_depth - 1);
    }
    sampled_board.UnMake();
    int target = 0;
    if (score > sampled_alpha) {
      target = 1;
    }
    features[i][0] = target;
    samples.emplace_back(features[i]);
    sampled_positions++;
    if (sampled_positions % 1000 == 0) {
      std::cout << "Sampled " << sampled_positions << " positions!" << std::endl;
      std::cout << "Further " << all_above << " all cut nodes, "
                              << all_below << " all nodes and "
                              << too_easy << " too easy nodes!" << std::endl;
      parse::Save2dVecToCSV<int>(samples, "data/search_param_dataset.csv");
    }
  }
  parse::Save2dVecToCSV<int>(samples, "data/search_param_dataset.csv");
  std::cout << "Finished creating dataset!" << std::endl;
}

// For move ordering we train a logistic regression classifier to predict whether a move will break beta.
// Alternatively predicting relative move orderings such as the probability of a move being greater or equal
// to all previous moves in the list or better than the previous move in the move ordering performed worse.

void TrainSearchParams(bool from_scratch) {
  const int scaling = 128;
  set_print_info(false);
  std::vector<double> weights(kNumMoveProbabilityFeatures);
  std::vector<double> weights_in_check(kNumMoveProbabilityFeatures);
  if (!from_scratch) {
    LoadSearchVariables();
    for (int i = 0; i < kNumMoveProbabilityFeatures; i++) {
      weights[i] = search_weights[i];
      weights_in_check[i] = search_weights_in_check[i];
    }
  }
  weights[kPWIHashMove] = 2000;
  weights[kPWIMoveType + kRookPromotion] = -2000;
  weights[kPWIMoveType + kBishopPromotion] = -2000;
  weights_in_check[kPWIHashMove] = 2000;
  weights_in_check[kPWIMoveType + kRookPromotion] = -2000;
  weights_in_check[kPWIMoveType + kBishopPromotion] = -2000;
  std::vector<Game> games = data::LoadGames();
  double nu = 4.;
  if (!from_scratch) {
    nu /= 8;
  }
  int sampled_positions = 0;
  int all_above = 0, all_below = 0, too_easy = 0;
  while (true) {
    clear_killers_and_counter_moves();
    table::ClearTable();
    kNodeCountSampleAt = 800 + rng() % 400;
    Game game = games[rng() % games.size()];
    if (game.moves.size() < 25) {
      continue;
    }
    game.set_to_position_after((2 * game.moves.size() / 3)
                               + (rng() % (game.moves.size() / 3)) - 2);
    Board board = game.board;
    sample_nodes = 0;
    sampled_alpha = kMinScore;
    RootSearch<kSamplingSearchMode>(board, 128, Milliseconds(150));
    if (sampled_alpha == kMinScore) {
      continue;
    }
    end_time = get_infinite_time();
    std::vector<Move> moves = sampled_board.GetMoves<kNonQuiescent>();
    std::shuffle(moves.begin(), moves.end(), rng);
    Threads.main_thread->board.SetToSamePosition(sampled_board);
    SortMovesML(moves, *Threads.main_thread, kNullMove);
    std::vector<std::vector<int> > features;
    MoveOrderInfo info(sampled_board);
    if (sampled_board.InCheck()) {
      for (int i = 0; i < moves.size(); i++) {
        features.emplace_back(GetMoveWeight<std::vector<int>, true>(moves[i], *Threads.main_thread, info));
      }
    }
    else {
      for (int i = 0; i < moves.size(); i++) {
        features.emplace_back(GetMoveWeight<std::vector<int>, false>(moves[i], *Threads.main_thread, info));
      }
    }
    std::vector<Score> scores(features.size());
    int low = 0, high = 0;
    for (int i = 0; i < moves.size(); i++) {
      Threads.main_thread->board.Make(moves[i]);
      Score score = -AlphaBeta<kNW, kNormalSearchMode>(*Threads.main_thread,
                                                       -(sampled_alpha+1),
                                                       -sampled_alpha,
                                                       sampled_depth - 1);
      Threads.main_thread->board.UnMake();
      scores[i] = score;
      if (score > sampled_alpha) {
        high++;
        if (high > moves.size() / 2) {
          break;
        }
      }
      else {
        low++;
      }
    }
    if (high == 0) {
      all_below++;
      continue;
    }
    else if (low == 0) {
      all_above++;
      continue;
    }
    else if (high > (low / 2)) {
      too_easy++;
      continue;
    }
    for (int i = 0; i < moves.size(); i++) {
      if (GetMoveType(moves[i]) == kRookPromotion
          || GetMoveType(moves[i]) == kBishopPromotion) {
        continue;
      }
      Score score;
      sampled_board.Make(moves[i]);
      if (sampled_node_type == kNW) {
        score = scores[i];
      }
      else {
        Threads.main_thread->board.SetToSamePosition(sampled_board);
        score = -AlphaBeta<kPV, kNormalSearchMode>(*Threads.main_thread,
                                                  -(sampled_alpha+1),
                                                  -sampled_alpha,
                                                  sampled_depth - 1);
      }
      sampled_board.UnMake();
      double target = 0;
      if (score > sampled_alpha) {
        target = 1;
      }
      double final_score = 0;
      if (sampled_board.InCheck()) {
        for (int idx = 0; idx < kNumMoveProbabilityFeatures; idx++) {
          final_score += features[i][idx] * weights_in_check[idx];
        }
        final_score /= scaling;
        double sigmoid = 1 / ( 1 + std::exp(-final_score) );
        double gradient = sigmoid - target;
        for (int idx = 0; idx < kNumMoveProbabilityFeatures; idx++) {
          weights_in_check[idx] -= (2 * nu * gradient * features[i][idx]) / (1);
        }
      }
      else {
        for (int idx = 0; idx < kNumMoveProbabilityFeatures; idx++) {
          final_score += features[i][idx] * weights[idx];
        }
        final_score /= scaling;
        double sigmoid = 1 / ( 1 + std::exp(-final_score) );
        double gradient = sigmoid - target;
        for (int idx = 0; idx < kNumMoveProbabilityFeatures; idx++) {
          weights[idx] -= (nu * gradient * features[i][idx]) / (1);
        }
      }
    }
    sampled_positions++;
    if (sampled_positions % 10 == 0) {
      //Our reference is a king moving into the corner with nothing else special.
      //Everything else is set relative to this situation.
      weights[kPWIMoveType + kEnPassant] = 0;
      weights[kPWIPieceTypeXTargetPieceType + kKing * 6 + kNoPiece - 1] = 0;
      weights[kPWIMoveSource] = 0;
      weights[kPWIKnightMoveSource + 1] = 0;
      weights_in_check[kPWIMoveType + kEnPassant] = 0;
      weights_in_check[kPWIPieceTypeXTargetPieceType + kKing * 6 + kNoPiece - 1] = 0;
      weights_in_check[kPWIMoveSource] = 0;
      weights_in_check[kPWIKnightMoveSource + 1] = 0;
    }
    if (sampled_positions % 1000 == 0) {
      std::cout << "Sampled " << sampled_positions << " positions!" << std::endl;
      std::cout << "Further " << all_above << " all cut nodes, "
                              << all_below << " all nodes and "
                              << too_easy << " too easy nodes!" << std::endl;
      for (int idx = 0; idx < kNumMoveProbabilityFeatures; idx++) {
        search_weights[idx] = std::round(weights[idx]);
        search_weights_in_check[idx] = std::round(weights_in_check[idx]);
      }
      SaveSearchVariables();
    }
    if (sampled_positions % 10000 == 0) {
      benchmark::MoveOrderTest();
    }
    if (sampled_positions % 100000 == 0) {
      nu /= 2;
      std::cout << "New nu: " << nu << std::endl;
    }
  }
}

struct SearchTrainHelper {
  Move move;
  Score score;
  std::vector<int> features;
};

double get_score_target(const Score score, const std::vector<SearchTrainHelper> &moves_and_scores) {
  double equal = 0, greater = 0;
  for (int i = 0; i < moves_and_scores.size(); i++) {
    if (moves_and_scores[i].score == score) {
      equal++;
    }
    else if (moves_and_scores[i].score > score) {
      greater++;
    }
  }
  assert(equal >= 1);//score is expected to be taken from amongst moves in list.
  equal--;
  return 1 - ( (greater + equal/2) / (moves_and_scores.size() - 1.0) );
}

struct SearchParamPositionSample {
  Move move;
  int num_moves;
  int better_moves;
  int equal_moves;
  Score score;
  int break_beta;
  std::vector<int> features;
};

int CountGreater(const Score score, const std::vector<SearchTrainHelper> &moves_and_scores) {
  int res = 0;
  for (int i = 0; i < moves_and_scores.size(); i++) {
    if (moves_and_scores[i].score > score)
      res++;
  }
  return res;
}

int CountEqual(const Score score, const std::vector<SearchTrainHelper> &moves_and_scores) {
  int res = 0;
  for (int i = 0; i < moves_and_scores.size(); i++) {
    if (moves_and_scores[i].score == score)
      res++;
  }
  return res-1;
}

void PrintDataset(const std::vector<SearchParamPositionSample> &samples, const std::string filename) {
  std::ofstream file(filename);
  //print header first
  file << "move, num_moves, better_moves, equal_moves, score, break_beta";
  for (int i = 0; i < samples[i].features.size(); i++) {
    file << ", fe" << i;
  }
  file << std::endl;
  for (int i = 0; i < samples.size(); i++) {
    file << samples[i].move << ", " << samples[i].num_moves << ", "
        << samples[i].better_moves << ", " << samples[i].equal_moves << ", "
        << samples[i].score << ", " << samples[i].break_beta;
    for (int j = 0; j < samples[i].features.size(); j++) {
      file << ", " << samples[i].features[j];
    }
    file << std::endl;
  }
  file.flush();
  file.close();
}

void CreateSearchParamDataset() {
  set_print_info(false);
  std::vector<Game> games = data::LoadGames();
  int sampled_positions = 0;
  std::vector<SearchParamPositionSample> samples, samples_in_check;
  while (true) {
    clear_killers_and_counter_moves();
    table::ClearTable();
    kNodeCountSampleAt = 40000 + rng() % 400;
    Game game = games[rng() % games.size()];
    if (game.moves.size() < 25) {
      continue;
    }
    game.set_to_position_after((game.moves.size() / 3)
                               + (rng() % (2 * game.moves.size() / 3)) - 2);
    Board board = game.board;
    sample_nodes = 0;
    sampled_alpha = kMinScore;
    RootSearch<kSamplingSearchMode>(board, 128, Milliseconds(7500));
    if (sampled_alpha == kMinScore) {
      continue;
    }
    end_time = get_infinite_time();
    std::vector<Move> moves = sampled_board.GetMoves<kNonQuiescent>();
    if (moves.size() <= 1) {
      continue;
    }
    std::shuffle(moves.begin(), moves.end(), rng);
    Threads.main_thread->board.SetToSamePosition(sampled_board);
    SortMovesML(moves, *Threads.main_thread, kNullMove);
    std::vector<std::vector<int> > features;
    MoveOrderInfo info(sampled_board);
    if (sampled_board.InCheck()) {
      for (int i = 0; i < moves.size(); i++) {
        features.emplace_back(GetMoveWeight<std::vector<int>, true>(moves[i], *Threads.main_thread, info));
      }
    }
    else {
      for (int i = 0; i < moves.size(); i++) {
        features.emplace_back(GetMoveWeight<std::vector<int>, false>(moves[i], *Threads.main_thread, info));
      }
    }
    std::vector<SearchTrainHelper> moves_and_scores;
    for (int i = 0; i < moves.size(); i++) {
      if (GetMoveType(moves[i]) == kRookPromotion
          || GetMoveType(moves[i]) == kBishopPromotion) {
        continue;
      }
      sampled_board.Make(moves[i]);
      Threads.main_thread->board.SetToSamePosition(sampled_board);
      Score score = -AlphaBeta<kPV, kNormalSearchMode>(*Threads.main_thread,
                                                       kMinScore,
                                                       kMaxScore,
                                                       6);//sampled_depth - 1);
      sampled_board.UnMake();
      SearchTrainHelper sth;
      sth.move = moves[i];
      sth.score = score;
      sth.features = features[i];
      moves_and_scores.emplace_back(sth);
    }
    if (CountGreater(sampled_alpha, moves_and_scores) > moves_and_scores.size() / 2
        || CountGreater(sampled_alpha, moves_and_scores) == 0) {
      continue;
    }
    for (int i = 0; i < moves_and_scores.size(); i++) {
      SearchParamPositionSample sample;
      sample.move = moves_and_scores[i].move;
      sample.score = moves_and_scores[i].score;
      sample.break_beta = sample.score > sampled_alpha ? 1 : 0;
      sample.features = moves_and_scores[i].features;
      sample.better_moves = CountGreater(moves_and_scores[i].score, moves_and_scores);
      sample.equal_moves = CountEqual(moves_and_scores[i].score, moves_and_scores);
      sample.num_moves = moves_and_scores.size();
      if (sampled_board.InCheck()) {
        samples_in_check.emplace_back(sample);
      }
      else {
        samples.emplace_back(sample);
      }
    }
    sampled_positions++;
    if (sampled_positions % 100 == 0) {
      std::cout << "Sampled " << sampled_positions << " positions!" << std::endl;
    }
    if (sampled_positions % 1000 == 0) {
      if (!samples.empty()) {
        PrintDataset(samples, "search_params/DSet3.csv");
      }
      if (!samples_in_check.empty()) {
        PrintDataset(samples_in_check, "search_params/DSetInCheck3.csv");
      }
    }
  }
}

Score QSearch(Board &board) {
  return QuiescentSearch<kNormalSearchMode>((*Threads.main_thread), kMinScore, kMaxScore);
}

Board SampleEval(Board board) {
  evaluation_nodes = 0;
  RootSearch<kSamplingEvalMode>(board, 128, Milliseconds(5000));
  return sampled_board;
}

void SaveSearchVariables() {
  std::ofstream file(settings::kSearchParamFile);
  std::ofstream description_file(settings::kSearchParamExplanationFile);
  int idx = 0;
  for (int i = 0; i < kNumMoveProbabilityFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    file << search_weights[i] << " " << std::endl;
    description_file  << search_weights[i]
                      << " <-- " << kFeatureInfos[idx].info << std::endl;
  }
  file.flush();
  file.close();
  description_file.flush();
  description_file.close();

  std::ofstream fileic(settings::kSearchParamInCheckFile);
  std::ofstream description_fileic(settings::kSearchParamInCheckExplanationFile);
  idx = 0;
  for (int i = 0; i < kNumMoveProbabilityFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    fileic << search_weights_in_check[i] << " " << std::endl;
    description_fileic  << search_weights_in_check[i]
                      << " <-- " << kFeatureInfos[idx].info << std::endl;
  }
  fileic.flush();
  fileic.close();
  description_fileic.flush();
  description_fileic.close();
}

void SaveHardcodeSearchVariables() {
  std::ofstream file("hardcoded_search_params.txt");
  int idx = 0;
  file << "const std::array<int, " << kNumMoveProbabilityFeatures << "> search_params = {" << std::endl;
  for (int i = 0; i < kNumMoveProbabilityFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    file  << "    " << search_weights[i];
    if (i+1 < kNumMoveProbabilityFeatures) {
      file << ",";
    }
    else {
      file << " ";
    }
    int val = 5 - parse::CountChars(search_weights[i]);
    while (0 < val--) {
      file << " ";
    }
    file << "// " << kFeatureInfos[idx].info << std::endl;
  }
  file << "};" << std::endl << std::endl;

  file << "const std::array<int, " << kNumMoveProbabilityFeatures << "> search_params_in_check = {" << std::endl;
  idx = 0;
  for (int i = 0; i < kNumMoveProbabilityFeatures; i++) {
    if (i == kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    file  << "    " << search_weights_in_check[i];
    if (i+1 < kNumMoveProbabilityFeatures) {
      file << ",";
    }
    else {
      file << " ";
    }
    int val = 5 - parse::CountChars(search_weights_in_check[i]);
    while (0 < val--) {
      file << " ";
    }
    file << "// " << kFeatureInfos[idx].info << std::endl;
  }
  file << "};" << std::endl;

  file.flush();
  file.close();
}

void LoadSearchVariables() {
  std::ifstream file(settings::kSearchParamFile);
  for (size_t i = 0; i < kNumMoveProbabilityFeatures; i++) {
    file >> search_weights[i];
  }
  file.close();

  std::ifstream fileic(settings::kSearchParamInCheckFile);
  for (size_t i = 0; i < kNumMoveProbabilityFeatures; i++) {
    fileic >> search_weights_in_check[i];
  }
  fileic.close();
}

void LoadSearchVariablesHardCoded() {
  int c = 0;
  for (size_t i = 0; i < kNumMoveProbabilityFeatures; i++) {
    search_weights[i] = hardcode::search_params[c];
    search_weights_in_check[i] = hardcode::search_params_in_check[c++];
  }
}

void EvaluateScoreDistributions(const int focus) {
  set_print_info(false);
  long count1 = 0, count2 = 0;
  std::vector<Game> games = data::LoadGames(1200000);
  const int max_depth = 3, n_score_bins = 160, score_bin_size = 100,
            n_dif_bins = 800, dif_bin_size = 16;
  std::vector<Array2d<long, n_score_bins, n_dif_bins> > histogram(max_depth);
  for (int i = 0; i < max_depth; i++) {
    for (int j = 0; j < n_score_bins; j++) {
      for (int k = 0; k < n_dif_bins; k++) {
        histogram[i][j][k] = 0;
      }
    }
  }
  int id = 0;
  while (true) {
    id++;
    Game game = games[rng() % games.size()];
    int index = (rng() % (2 * game.moves.size() / 3)) + (game.moves.size() / 3) - 2;
    game.set_to_position_after(index);
    clear_killers_and_counter_moves();
    table::ClearTable();

    kNodeCountSampleAt = 300 + rng() % 150;
    Board board = game.board;
    sample_nodes = 0;
    sampled_alpha = kMinScore;
    RootSearch<kSamplingSearchMode>(board, 128, Milliseconds(150));
    if (sampled_alpha == kMinScore || sampled_board.InCheck()) {
      continue;
    }
    Score score = evaluation::ScoreBoard(sampled_board);
    Score score_bin_idx = score;
    score_bin_idx += score_bin_size / 2;
    score_bin_idx /= score_bin_size;
    score_bin_idx += n_score_bins / 2;
    score_bin_idx = std::max(0, (int)score_bin_idx);
    score_bin_idx = std::min(n_score_bins - 1, (int)score_bin_idx);
    if (focus == 0) {
      for (int depth = 1; depth <= max_depth; depth++) {
        DepthSearch(sampled_board, depth);
        Score ab_score = get_last_search_score();
        Score dif = ab_score - score;
        dif /= dif_bin_size;
        dif += n_dif_bins / 2;
        dif = std::max(0, (int)dif);
        dif = std::min(n_dif_bins - 1, (int)dif);
        histogram[depth - 1][score_bin_idx][dif]++;
      }
    }
    else if (focus == 1) {
      std::vector<Move> moves = sampled_board.GetMoves<kNonQuiescent>();
      for (int depth = 1; depth <= max_depth; depth++) {
        for (Move move : moves) {
          if (GetMoveType(move) >= kEnPassant) {
            continue;
          }
          sampled_board.Make(move);
          if (sampled_board.InCheck()) {
            sampled_board.UnMake();
            continue;
          }
          Score ab_score = 0;
          if (depth == 1) {
            ab_score = -QSearch(sampled_board);
          }
          else {
            DepthSearch(sampled_board, depth - 1);
            ab_score = -get_last_search_score();
          }
          sampled_board.UnMake();

          Score dif = ab_score - score;
          dif /= dif_bin_size;
          dif += n_dif_bins / 2;
          dif = std::max(0, (int)dif);
          dif = std::min(n_dif_bins - 1, (int)dif);
          histogram[depth - 1][score_bin_idx][dif]++;
        }
      }
    }
    else if (focus == 2) {
      std::vector<Move> moves = sampled_board.GetMoves<kNonQuiescent>();
      for (int depth = 1; depth <= max_depth; depth++) {
        Score max_dif = kMinScore;
        Score max_forcing_dif = kMinScore;
        for (Move move : moves) {
          bool forcing = false;
          if (GetMoveType(move) >= kEnPassant) {
            forcing = true;
          }
          sampled_board.Make(move);
          if (sampled_board.InCheck()) {
            forcing = true;
          }
          Score ab_score = 0;
          if (depth == 1) {
            ab_score = -QSearch(sampled_board);
          }
          else {
            DepthSearch(sampled_board, depth - 1);
            ab_score = -get_last_search_score();
          }
          sampled_board.UnMake();

          if (forcing) {
            max_forcing_dif = std::max(ab_score - score, max_forcing_dif);
          }
          else {
            max_dif = std::max(ab_score - score, max_dif);

          }
        }
        if (max_dif > max_forcing_dif) {
          count1++;
          max_dif /= dif_bin_size;
          max_dif += n_dif_bins / 2;
          max_dif = std::max(0, (int)max_dif);
          max_dif = std::min(n_dif_bins - 1, (int)max_dif);
          histogram[depth - 1][score_bin_idx][max_dif]++;
        }
        count2++;
      }
    }
    if ((id + 1) % 1000 == 0) {
      std::cout << "Evaluated " << (id+1) << " games!" << std::endl;
    }
    if (id % 1000 == 0) {
      std::vector<std::vector <long> > vhistogram(n_score_bins, std::vector<long>(n_dif_bins + 1));
      for (int i = 0; i < n_score_bins; i++) {
        vhistogram[i][0] = (i - (n_score_bins / 2)) * score_bin_size;
      }
      for (int depth = 0; depth < max_depth; depth++) {
        for (int i = 0; i < n_score_bins; i++) {
          for (int j = 0; j < n_dif_bins; j++) {
            vhistogram[i][j + 1] = histogram[depth][i][j];
          }
        }
        switch (focus) {
          case 2:
            parse::Save2dVecToCSV<long>(vhistogram, "data/max_quiet_hist" + std::to_string(depth + 1) + ".csv");
            std::cout << "Max is quiet in " << count1 << "/" << count2 << std::endl;
            break;
          case 1:
            parse::Save2dVecToCSV<long>(vhistogram, "data/v2quiet_hist" + std::to_string(depth + 1) + ".csv");
            break;
          default:
            parse::Save2dVecToCSV<long>(vhistogram, "data/v2hist" + std::to_string(depth + 1) + ".csv");
        }
      }
    }
  }
}


void EvaluateCaptureMoveValue(int n) {
  std::vector<std::vector<Score> > move_scores(6);
  std::vector<Game> games = data::LoadGames();
  for (int i = 0; i < games.size(); i++) {
    Game game = games[i];
    game.set_to_position_after(0);
    while (game.board.get_num_made_moves() < game.moves.size()) {
      const Move move = game.moves[game.board.get_num_made_moves()];
      if (GetMoveType(move) == kCapture
          && !game.board.GivesCheck(move)) {
        PieceType target = game.board.get_piece(GetMoveDestination(move));
        Score before = evaluation::ScoreBoard(game.board);
        game.forward();
        Score after = -evaluation::ScoreBoard(game.board);
        move_scores[GetPieceType(target)].emplace_back(after - before);
      }
      else if (GetMoveType(move) == kEnPassant
          && !game.board.GivesCheck(move)) {
        Score before = evaluation::ScoreBoard(game.board);
        game.forward();
        Score after = -evaluation::ScoreBoard(game.board);
        move_scores[5].emplace_back(after - before);
      }
      else {
        game.forward();
      }
    }
    if ((i + 1) % 10000 == 0) {
      std::cout << "Evaluated " << (i+1) << " games!" << std::endl;
    }
  }
  for (int i = 0; i < 6; i++) {
    std::sort(move_scores[i].begin(), move_scores[i].end());
  }
  for (int j = 0; j < 6; j++) {
    for (int i = 1; i < n; i++) {
      std::cout << i << ":" << move_scores[j][move_scores[j].size() * i / (n)] << " ";
    }
    std::cout << std::endl << std::endl;
  }
}

std::vector<Board> GenerateEvalSampleSet(std::string filename) {
  std::vector<Board> boards;
  std::vector<Game> games = data::LoadGames(1200000, settings::kCCRLPath);
  boards.reserve(games.size());
  HashType last_hash = 42;
  set_print_info(false);
  for (int i = 0; i < games.size(); i++) {
    data::SetGameToRandom(games[i]);
    Board board = SampleEval(games[i].board);
    if (board.get_hash() != last_hash) {
      last_hash = board.get_hash();
      boards.emplace_back(board);
      if (boards.size() % 1000 == 0) {
        std::cout << "\rsampled " << boards.size() << " positions!" << std::flush;
      }
    }
  }
  std::cout << "\rsampled " << boards.size() << " positions!" << std::endl;
  set_print_info(true);
  data::SaveBoardFens(filename, boards);
  return boards;
}

}

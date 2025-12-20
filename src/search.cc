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
 * search.cc
 *
 *  Created on: Nov 6, 2016
 *      Author: Jonathan Rosenthal
 */

#include "move_order.h"
#include "search.h"
#include "net_evaluation.h"
#include "transposition.h"
#include "search_thread.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <optional>
#include <mutex>
#include <random>
#include <tuple>

using OptEntry = std::optional<table::Entry>;

namespace {

enum class NodeType {
  kPV, kNW
};

int32_t contempt = 0;
bool armageddon = false;
std::array<Score, 2> draw_score { kDrawScore, kDrawScore };

Milliseconds rsearch_duration;
Depth rsearch_depth;

int skip_time_check = 0;

#ifdef TUNE
#define EXPR
#else
#define EXPR constexpr
#endif

EXPR Depth kLMROffset = -16;
EXPR int32_t kLMRMult = 86;
EXPR Depth kLMROffsetCap = 15;
EXPR int32_t kLMRMultCap = 51;
EXPR Depth kLMROffsetPV = 21;
EXPR int32_t kLMRMultPV = 86;
EXPR Depth kLMROffsetPVCap = 23;

EXPR NScore kInitialAspirationDelta = 72;
EXPR NScore kSNMPScaling = 709;
EXPR NScore kSNMPOffset = -122;
EXPR NScore kSNMPImproving = 190;
EXPR NScore kFutilityScaling = 685;
EXPR NScore kFutilityOffset = 127;
EXPR NScore kFutilityImproving = 149;
EXPR Depth kLMPBaseNW = 408;
EXPR Depth kLMPBasePV = 817;
EXPR int32_t kLMPScalar = 122;
EXPR int32_t kLMPQuad = 53;
EXPR Depth kLMPImpBaseNW = 678;
EXPR Depth kLMPImpBasePV = 569;
EXPR int32_t kLMPImpScalar = 188;
EXPR int32_t kLMPImpQuad = 83;
EXPR Depth kSingularExtensionDepth = 9;
EXPR Depth kNMPBase = 485;
EXPR Depth kNMPScale = 40;

EXPR std::array<NScore, 4> init_futility_margins() {
  EXPR std::array<NScore, 4> kFutilityMargins = {
    kFutilityOffset, kFutilityOffset + kFutilityScaling, kFutilityOffset + kFutilityScaling * 2, kFutilityOffset + kFutilityScaling * 3};
  return kFutilityMargins;
}

EXPR std::array<NScore, 4> kFutileMargin = init_futility_margins();

EXPR Array3d<Depth, 2, 2, 6> init_lmp_breakpoints() {
  
  Array3d<Depth, 2, 2, 6> lmp{};
  lmp[0][0][0] = 0;
  lmp[0][1][0] = 0;
  lmp[1][0][0] = 0;
  lmp[1][1][0] = 0;
  for (int32_t i = 1; i < lmp[0][0].size(); ++i) {
    int32_t j = i-1;
    lmp[0][0][i] = (kLMPBaseNW + (kLMPScalar*j + kLMPQuad*j*j)) / 128;
    lmp[0][1][i] = (kLMPImpBaseNW + (kLMPImpScalar*j + kLMPImpQuad*j*j)) / 128;
    lmp[1][0][i] = (kLMPBasePV + (kLMPScalar*j + kLMPQuad*j*j)) / 128;
    lmp[1][1][i] = (kLMPImpBasePV + (kLMPImpScalar*j + kLMPImpQuad*j*j)) / 128;
  }
  return lmp;
}

EXPR Array3d<Depth, 2, 2, 6> kLMP = init_lmp_breakpoints();

constexpr Depth lmr_calculator(Depth i, size_t j, double offset, double multiplier) {
  Depth res = std::floor(offset + (std::log(i+1) * std::log(j+1) * multiplier));
  return std::max(std::min(res, i), 0);
}

Array3d<Depth, 64, 64, 4> init_lmr_reductions() {
  constexpr double scale = 0.01;
  const double x = kLMROffset * scale, y = kLMRMult * scale;
  const double x_cap = kLMROffsetCap * scale, y_cap = y * kLMRMultCap * scale;
  const double x_pv = kLMROffsetPV * scale, y_pv = y * kLMRMultPV * scale;
  const double x_pv_cap = kLMROffsetPVCap * scale;
  const double y_pv_cap = y * kLMRMultPV * scale * kLMRMultCap * scale;

  Array3d<Depth, 64, 64, 4> lmr_reductions{};
  for (Depth i = 0; i < 64; ++i) {
    for (size_t j = 0; j < 64; ++j) {
      lmr_reductions[i][j][0] = lmr_calculator(i, j, x, y);
      lmr_reductions[i][j][1] = lmr_calculator(i, j, x_cap, y_cap);
      lmr_reductions[i][j][2] = lmr_calculator(i, j, x_pv, y_pv);
      lmr_reductions[i][j][3] = lmr_calculator(i, j, x_pv_cap, y_pv_cap);
    }
  }
  return lmr_reductions;
}

#ifndef TUNE
#undef EXPR
#define EXPR const
#endif

EXPR Array3d<Depth, 64, 64, 4> lmr_reductions = init_lmr_reductions();

#undef EXPR

template<NodeType node_type>
Depth get_lmr_reduction(const Depth depth, const size_t move_number, bool cap) {
  assert(depth > 0);
  size_t is_pv = node_type == NodeType::kPV ? 2 : 0;
  return lmr_reductions[std::min(depth - 1, 63)][std::min(move_number, (size_t)63)][is_pv + cap];
}

inline NScore GetSNMPMargin(const bool improving, const Depth depth) {
  const NScore multiplier = kSNMPScaling - kSNMPImproving * improving;
  return multiplier * depth + kSNMPOffset;
}

inline bool SNMPMarginSatisfied(const Score &eval, const Score &beta,
                                bool improving, const Depth depth) {
  return eval.value() > beta.value() + GetSNMPMargin(improving, depth);
}

bool uci_show_wdl = true;

Score last_search_score;

bool print_info = true;

bool SwapToFront(std::vector<Move> &moves, const Move move) {
  for (size_t i = 0; i < moves.size(); ++i) {
    if (moves[i] == move) {
      std::swap(moves[i], moves[0]);
      return true;
    }
  }
  return false;
}

size_t min_ply = 0;
constexpr size_t kInfiniteNodes = 1000000000000;
size_t max_nodes = kInfiniteNodes;
bool fixed_search_time;

Time end_time = now();

inline bool finished(search::Thread &thread) {
  if (thread.id == 0) {
    if (skip_time_check <= 0) {
      size_t current_nodes = search::Threads.get_node_count();
      skip_time_check = std::min((size_t)256, max_nodes-current_nodes);
      return end_time <= now() || max_nodes <= current_nodes
                               || search::Threads.end_search.load(std::memory_order_relaxed);
    }
    skip_time_check--;
    return search::Threads.end_search;
  }
  return search::Threads.end_search.load(std::memory_order_relaxed);
}

// Recursively build PV from TT up to param depth
void build_pv(Board &board, std::vector<Move> &pv);

void build_pv(Board &board, std::vector<Move> &pv, Move legal_move) {
  constexpr size_t kMaxPvLength = 100;
  pv.emplace_back(legal_move);
  board.Make(legal_move);
  if (!board.IsDraw() && board.CountRepetitions(min_ply) <= 2
      && pv.size() < kMaxPvLength) {
    build_pv(board, pv);
  }
  board.UnMake();
}

void build_pv(Board &board, std::vector<Move> &pv) {
  OptEntry entry = table::GetEntry(board.get_hash());

  if (entry.has_value()) {
    std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
    for (Move move : moves) {
      if (move == entry->get_best_move()) {
        return build_pv(board, pv, move);
      }
    }
  }
}

inline bool sufficient_bounds(const Board &board, const OptEntry &entry,
                              const Score alpha, const Score beta,
                              const Depth depth) {
  Score score = entry->get_score(board);
  return entry->depth >= depth
      && ((entry->get_bound() == kExactBound)
          || (entry->get_bound() == kLowerBound && score >= beta)
          || (entry->get_bound() == kUpperBound && score <= alpha));
}

inline bool is_null_move_allowed(const Score &eval, const Score &beta,
                                 const Board &board, const Depth depth) {
  return  eval >= beta && depth > 1 && board.has_non_pawn_material(board.get_turn());
}

}

namespace search {

std::vector<Move> GetSortedMovesML(Board &board) {
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  Threads.main_thread->board.SetToSamePosition(board);
  move_order::SortML(moves, *Threads.main_thread);
  return moves;
}

size_t Perft(Board &board, Depth depth) {
  if (depth <= 0) {
    return 1;
  }
  if (depth == 1) {
    return board.GetMoves<kNonQuiescent>().size();
  }
  size_t perft_sum = 0;
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  for (Move move : moves) {
    board.Make(move);
    perft_sum += Perft(board, depth-1);
    board.UnMake();
  }
  return perft_sum;
}

Score QuiescentSearch(Thread &t, Score alpha, const Score beta) {
  assert(beta > alpha);
  t.nodes++;
  Score lower_bound_score = GetMatedOnMoveScore(t.board.get_num_made_moves());
  //Update max ply reached in search
  if (t.max_depth < t.board.get_num_made_moves()) {
    t.max_depth = t.board.get_num_made_moves();
  }

  //End search immediately if trivial draw is reached
  if (t.board.IsTriviallyDrawnEnding()) {
    return draw_score[t.board.get_turn()];
  }

  //TT probe
  const OptEntry entry = table::GetEntry(t.board.get_hash());
  if (entry.has_value()) {
    if (sufficient_bounds(t.board, entry, alpha, beta, 0)) {
      return entry->get_score(t.board);
    }
    // assert(entry.bound == kLowerBound || entry.bound == kExactBound);
    if (entry->get_bound() != kUpperBound) {
      lower_bound_score = entry->get_score(t.board);
    }
  }

  //Static evaluation
  bool in_check = t.board.InCheck();
  Score static_eval = kMinScore;
  if (!in_check) {
    static_eval = t.adjust_static_eval(net_evaluation::ScoreThread(t));

    if (entry.has_value() && entry->get_bound() == kLowerBound && static_eval < entry->get_score(t.board)) {
      static_eval = entry->get_score(t.board);
    }

    //Stand pat
    if (static_eval >= beta) {
      return static_eval;
    }

    //Update alpha in PV nodes if possible
    if (static_eval > alpha) {
      alpha = static_eval;
    }
    lower_bound_score = std::max(static_eval, lower_bound_score);
  }

  //Get moves
  std::vector<Move> moves = t.board.GetMoves<kQuiescent>();

  if (moves.size() == 0) {
    if (in_check) {
      //In check all moves are generated, so if we have no moves in check it is mate.
      return GetMatedOnMoveScore(t.board.get_num_made_moves());
    }
    return lower_bound_score;
  }

  //Move best_move = 0;

  //Sort move list
  if (entry.has_value()) {
    move_order::Sort(moves, t, entry->get_best_move());
    //SortMovesML(moves, board, entry.best_move);
  }
  else {
    move_order::Sort(moves, t, 0);
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
    Score score = -QuiescentSearch(t, -beta, -alpha);
    t.board.UnMake();

    if (score > lower_bound_score) {

      //Return beta if we fail high
      if (score >= beta) {
        table::SaveEntry(t.board, move, score, 0);
        return score;
      }

      //Update alpha in PV nodes if possible
      if (score > alpha) {
        alpha = score;
      }
      lower_bound_score = score;
    }
  }

  //Return the lower bound after all has been said and done.
  return lower_bound_score;
}

inline NScore get_futility_margin(Depth depth, bool improving) {
  return kFutileMargin[depth] + kFutilityImproving * depth * improving;
}

void update_counter_move_history(Thread &t, const std::vector<Move> &quiets, const Depth depth) {
  if (t.board.get_num_made_moves() == 0 || t.board.get_last_move() == kNullMove) {
    return;
  }
  const Color color = t.board.get_turn();
  const Move move = t.board.get_last_move();
  Square opp_des = GetMoveDestination(move);
  PieceType opp_piecetype = GetPieceType(t.board.get_piece(opp_des));
  // TODO deal with promotion situations
  const int32_t score = std::min(depth * depth, 512);

  for (size_t i = 0; i < quiets.size() - 1; ++i) {
    Square src = GetMoveSource(quiets[i]);
    Square des = GetMoveDestination(quiets[i]);
    PieceType piecetype = GetPieceType(t.board.get_piece(GetMoveSource(quiets[i])));
    t.update_continuation_score<1>(opp_piecetype, opp_des, piecetype, des, -score);
    t.update_history_score(color, src, des, -score);
  }
  size_t i = quiets.size() - 1;
  Square src = GetMoveSource(quiets[i]);
  Square des = GetMoveDestination(quiets[i]);
  PieceType piecetype = GetPieceType(t.board.get_piece(GetMoveSource(quiets[i])));
  t.update_continuation_score<1>(opp_piecetype, opp_des, piecetype, des, score);
  t.update_history_score(color, src, des, score);

  if (t.get_height() < 2) {
    return;
  }

  const PieceTypeAndDestination last_own_move = t.get_previous_move(2);
  if (last_own_move.pt == kNoPiece) {
    return;
  }
  const PieceType previous_piecetype = last_own_move.pt;
  const Square previous_des = last_own_move.des;

  for (i = 0; i < quiets.size() - 1; ++i) {
    des = GetMoveDestination(quiets[i]);
    piecetype = GetPieceType(t.board.get_piece(GetMoveSource(quiets[i])));
    t.update_continuation_score<2>(previous_piecetype, previous_des, piecetype, des, -score);
  }
  i = quiets.size() - 1;
  des = GetMoveDestination(quiets[i]);
  piecetype = GetPieceType(t.board.get_piece(GetMoveSource(quiets[i])));
  t.update_continuation_score<2>(previous_piecetype, previous_des, piecetype, des, score);
}

inline Score get_singular_beta(Score beta, Depth depth) {
  WDLScore result = WDLScore{beta.win - 2*depth, beta.win_draw - 2*depth};
  if (result.win < 0) {
    result.win_draw += result.win;
    result.win = 0;
  }
  if (result.win_draw < 0) {
    result.win_draw = 0;
  }
  return result;
}

inline std::tuple<Score, Score, Depth> get_singular_bounds(
                                          Thread &t, const Depth depth,
                                          const OptEntry &entry) {
  const Score beta = entry->get_score(t.board);
  Score rBeta = get_singular_beta(beta, depth);
  Score rAlpha = rBeta.get_previous_score();
  Depth rDepth = (depth - 3) / 2;
  assert(beta.is_static_eval());
  assert(rBeta.is_static_eval());
  assert(rAlpha.is_static_eval());
  
  return {rAlpha, rBeta, rDepth};
}

inline bool singular_conditions_met(NodeType node_type, const Thread &t,
                                    const Depth depth, const OptEntry &entry,
                                    size_t movecount) {
  return depth >= kSingularExtensionDepth-2
        && entry->depth >= std::max(depth, kSingularExtensionDepth) - 3
        && !(node_type == NodeType::kPV && movecount == 1)
        && entry->get_bound() != kUpperBound
        && entry->get_score(t.board).is_static_eval()
        && get_singular_beta(entry->get_score(t.board), depth) > kMinStaticEval;
}

inline void update_killers(Thread &t, const Move move) {
  assert(GetMoveType(move) < kCapture);
  int num_made_moves = t.board.get_num_made_moves();
  if (t.killers[num_made_moves][0] != move) {
    t.killers[num_made_moves][1] = t.killers[num_made_moves][0];
    t.killers[num_made_moves][0] = move;
  }
}

inline void update_counter_moves(Thread &t, const Move move) {
  if (t.board.get_num_made_moves() > 0 && t.board.get_last_move() != kNullMove) {
    PieceType last_moved_piece = kNoPiece;
    const Move last_move = t.board.get_last_move();
    const Square last_destination = GetMoveDestination(last_move);
    if (GetMoveType(last_move) == kCastle) {
      last_moved_piece = kKing;
    }
    else {
      last_moved_piece = GetPieceType(t.board.get_piece(last_destination));
    }
    assert(last_moved_piece != kNoPiece);
    t.counter_moves[t.board.get_turn()][last_moved_piece][last_destination] = move;
  }
}

template<NodeType node_type>
Score AlphaBeta(Thread &t, Score alpha, const Score beta, Depth depth, Move exclude_move = kNullMove) {
  assert(alpha.is_valid());
  assert(beta.is_valid());
  assert(beta > alpha);
  assert(beta.value() == get_next_score(alpha).value() || node_type != NodeType::kNW);
  
  const bool is_root = (node_type == NodeType::kPV) && (t.root_height == t.board.get_num_made_moves());
  const Score original_alpha = alpha;

  //Immediately return 0 if we detect a draw.
  if (!is_root && (t.board.IsDraw() || (settings::kRepsForDraw == 3 && t.board.CountRepetitions(min_ply) >= 2))) {
    t.nodes++;
    if (t.board.IsFiftyMoveDraw() && t.board.InCheck() && t.board.GetMoves<kNonQuiescent>().empty()) {
      return GetMatedOnMoveScore(t.board.get_num_made_moves());
    }
    return draw_score[t.board.get_turn()];
  }

  //We drop to QSearch if we run out of depth.
  if (depth <= 0) {
    if constexpr (!settings::kUseQS) {
      t.nodes++;
      return t.adjust_static_eval(net_evaluation::ScoreThread(t));
    }
    return QuiescentSearch(t, alpha, beta);
  }

  // To avoid counting nodes twice if all we do is fall through to QSearch,
  // we wait until here to count this node.
  t.nodes++;

  //Transposition Table Probe
  OptEntry entry = table::GetEntry(t.board.get_hash());
  
  if (node_type != NodeType::kPV && !exclude_move && entry.has_value()
      && sufficient_bounds(t.board, entry, alpha, beta, depth) ) {
    Score score = entry->get_score(t.board);
    if (score > beta && !score.is_mate_score() && !beta.is_mate_score()) {
      return (score * 3 + beta) / 4;
    }
    return score;
  }
  
  if (depth >= 2 && !exclude_move && !entry.has_value()) {
    depth--;
  }

  const bool in_check = t.board.InCheck();
  Score static_eval = alpha;
  if (entry.has_value() && entry->get_bound() == kExactBound) {
      static_eval = entry->get_score(t.board);
      t.set_static_score(static_eval);
  }
  else if (!in_check) {
    static_eval = t.adjust_static_eval(net_evaluation::ScoreThread(t));
    if (entry.has_value()) {
      if ( (entry->get_bound() == kLowerBound && static_eval < entry->get_score(t.board))
          || (entry->get_bound() == kUpperBound && static_eval > entry->get_score(t.board)) ) {
        static_eval = entry->get_score(t.board);
      }
    }
    t.set_static_score(static_eval);
  }
  else {
    t.set_static_score(kNoScore);
  }
  bool improving = t.improving();
  bool nmp_failed_node = false;

  //Speculative pruning methods
  if (node_type == NodeType::kNW && !exclude_move && beta.is_static_eval() && !in_check) {
    //Static Null Move Pruning
    if (node_type == NodeType::kNW && settings::kUseScoreBasedPruning
        && depth <= 5 && SNMPMarginSatisfied(static_eval, beta,
                                             improving, depth)) {
      if (static_eval.is_mate_score()) {
        return beta;
      }
      return (static_eval + beta) / 2;
    }

    //Null Move Pruning
    if (is_null_move_allowed(static_eval, beta, t.board, depth)) {
      t.set_move(kNullMove);
      t.board.Make(kNullMove);
      const Depth R = (kNMPBase + depth * kNMPScale) / 128;
      Score score = -AlphaBeta<NodeType::kNW>(t, -beta, -alpha,
                                              depth - R);
      t.board.UnMake();
      if (score > kMaxStaticEval) {
        score = kMaxStaticEval;
      }
      if (score >= beta) {
        return score;
      }
      nmp_failed_node = true;
    }
  }

  // idea from http://talkchess.com/forum3/viewtopic.php?f=7&t=74769
  // ELO | 4.67 +- 3.56
  if (node_type == NodeType::kPV && !is_root && depth >= 5 && !entry.has_value()) {
    depth--;
  }

  //Get move list and return result if there are no legal moves
  std::vector<Move> moves = t.board.GetMoves<kNonQuiescent>();
  if (moves.size() == 0) {
    if (in_check) {
      return GetMatedOnMoveScore(t.board.get_num_made_moves());
    }
    return draw_score[t.board.get_turn()];
  }

  Move tt_entry = kNullMove;
  if (entry.has_value()) {
    tt_entry = entry->get_best_move();
  }
  if (is_root) {
    tt_entry = t.best_root_move;
  }

  bool moves_sorted = false, swapped = false;
  if (tt_entry != kNullMove) {
    swapped = SwapToFront(moves, tt_entry);
    assert(!swapped || tt_entry == moves[0]);
  }
  if (!swapped) {
    move_order::SortML(moves, t, tt_entry);
    moves_sorted = true;
  }

  Move best_local_move = tt_entry;
  if (node_type == NodeType::kPV && moves.size() == 1) {
    depth++;
  }

  //Init checking squares for efficient detection of checking moves.
  //Moves detected always give check, but only direct checks are detected.
  //Ie no checks from special moves or discovered checks.
  std::array<BitBoard, 6> checking_squares = t.board.GetDirectCheckingSquares();

  Score lower_bound_score = GetMatedOnMoveScore(t.board.get_num_made_moves());
  std::vector<Move> quiets;
  Score alpha_nw = alpha.get_next_score();
  //Move loop
  for (size_t i = 0; i < moves.size(); ++i) {
    if (i == 1 && !moves_sorted) {
      move_order::SortML(moves, t, tt_entry, 1);
      moves_sorted = true;
    }
    const Move move = moves[i];
    if (move == exclude_move) {
      continue;
    }
    
    Depth e = 0;// Extensions
    if (i == 0 && entry.has_value() && !is_root
        && singular_conditions_met(node_type, t, depth, entry, moves.size())) {
      const auto [rAlpha, rBeta, rDepth] = get_singular_bounds(t, depth, entry);
      Score score = AlphaBeta<NodeType::kNW>(t, rAlpha, rBeta, rDepth, tt_entry);
      if (score <= rAlpha) {
        e = 1 + nmp_failed_node;
      }
      else if (score >= beta) {
        return score;
      }
    }

    Depth reduction = 0;

    if (i && !in_check && !(checking_squares[GetPieceType(t.board.get_piece(GetMoveSource(move)))]
                              & GetSquareBitBoard(GetMoveDestination(move)))) {
      //Late Move Pruning
      assert(depth > 0);
      if (!is_root && (size_t)depth < kLMP[0][0].size() && (i >= (size_t)kLMP[node_type == NodeType::kPV][improving][depth])
          && GetMoveType(move) < kEnPassant) {
        continue;
      }

      //Late Move Reduction factor
      if (!is_root) {
        reduction = get_lmr_reduction<node_type>(depth, i, GetMoveType(move) > kDoublePawnMove);
      }
      else if (i > 2) {
        reduction = get_lmr_reduction<node_type>(depth, i-2, GetMoveType(move) > kDoublePawnMove);
      }
      assert(reduction < depth);

      //Futility Pruning
      if (node_type == NodeType::kNW && settings::kUseScoreBasedPruning
          && depth - reduction <= 3
          && static_eval.value() < (alpha.value() - get_futility_margin(depth - reduction, improving))
          && GetMoveType(move) < kEnPassant) {
        continue;
      }
      
      if (depth == 1 && GetMoveType(move) != kEnPassant && !t.board.NonNegativeSEE(move)) {
        continue;
      }
    }

    if (GetMoveType(move) < kCapture) {
      quiets.emplace_back(move);
    }

    //Make moves, search and unmake
    t.set_move(move);
    t.board.Make(move);
    Score score;
    if (i == 0) {
      //First move gets searched at full depth and window
      score = -AlphaBeta<node_type>(t, -beta, -alpha, depth - 1 + e);
    }
    else {
      //Assume we have searched the best move already and search with closed window and possibly reduction
      score = -AlphaBeta<NodeType::kNW>(t, -alpha_nw, -alpha,
                                              depth - 1 + e - reduction);
      //Research with full depth if our initial search indicates an improvement over Alpha
      if (score > alpha && (node_type == NodeType::kPV || reduction > 0)) {
        score = -AlphaBeta<node_type>(t, -beta, -alpha, depth - 1 + e);
      }
    }
    assert(score.is_valid());
    t.board.UnMake();

    //Ensure we still have time and our score was not prematurely terminated
    if (finished(t)) {
      Threads.end_search = true;
      return alpha;
    }

    //Check if search failed high
    if (score >= beta) {
      if (exclude_move) {
        return score;
      }
      
      table::SaveEntry(t.board, move, score, depth);
      update_counter_moves(t, move);
      if (GetMoveType(move) < kCapture) {
        update_counter_move_history(t, quiets, depth);
        update_killers(t, move);
        if (!in_check && static_eval < score) {
          t.update_error_history(score, depth);
        }
      }
      if (is_root) {
        t.best_root_move = move;
      }

      return score;
    }

    //In PV nodes we might be improving Alpha without breaking Beta
    if (score > alpha) {
      if (GetMoveType(move) < kCapture) {
        update_counter_move_history(t, quiets, depth);
      }
      //Update score and expected best move
      alpha = score;
      alpha_nw = alpha.get_next_score();
      lower_bound_score = score;
      best_local_move = move;
      if (is_root) {
        t.best_root_move = move;
      }
    }
    if (score > lower_bound_score) {
      lower_bound_score = score;
    }
  }
  
  if (node_type != NodeType::kNW && alpha > original_alpha) {
    assert(best_local_move != kNullMove);
    assert(exclude_move == kNullMove);
    // We should save any best move which has improved alpha.
    table::SavePVEntry(t.board, best_local_move, lower_bound_score, depth);
    if (!in_check && GetMoveType(best_local_move) < kCapture) {
      t.update_error_history(lower_bound_score, depth);
    }
  }
  else if (!exclude_move) {
    table::SaveEntry(t.board, best_local_move, lower_bound_score, depth, kUpperBound);
    if (!in_check && GetMoveType(best_local_move) < kCapture && static_eval > lower_bound_score) {
      t.update_error_history(lower_bound_score, depth);
    }
  }

  return lower_bound_score;
}

inline Score PVS(Thread &t, Depth current_depth, const std::vector<Score> &previous_scores) {
  if (current_depth <= 4) {
    Score score = AlphaBeta<NodeType::kPV>(t, kMinScore, kMaxScore, current_depth);
    if (t.id == 0 && !finished(t)) {
      last_search_score = score;
    }
    return score;
  }
  else {
    Score score = previous_scores.back();
    Score delta = WDLScore{kInitialAspirationDelta, kInitialAspirationDelta};
    Score alpha = (score-delta).get_valid_score();
    Score beta = (score+delta).get_valid_score();
    score = AlphaBeta<NodeType::kPV>(t, alpha, beta, current_depth);
    if (t.id == 0 && !finished(t)) {
      last_search_score = score;
    }
    while (!finished(t) && (score <= alpha || score >= beta)) {
      assert(delta.win > 0 && delta.win_draw > 0);
      if (score <= alpha) {
        if (beta.is_static_eval()) {
          beta = (std::max(score, kMinStaticEval) + (beta * 3)) / 4;
          assert(beta.is_valid());
        }
        alpha = (score-delta).get_valid_score();
        if (alpha == score) {
          alpha = score.get_previous_score();
        }
      }
      else if (score >= beta) {
        if (alpha.is_static_eval()) {
          alpha = ((alpha * 3) + std::min(kMaxStaticEval, score)) / 4;
          assert(alpha.is_valid());
        }
        beta = (score + delta).get_valid_score();
        if (beta == score) {
          beta = score.get_next_score();
        }
      }
      assert(score > alpha && score < beta);
      score = AlphaBeta<NodeType::kPV>(t, alpha, beta, current_depth);
      delta *= 2;
      if (t.id == 0 && !finished(t)) {
        last_search_score = score;
      }
    }
    return score;
  }
}

void PrintUCIInfoString(Thread &t, const Depth depth, const Time &begin,
                        const Time &end, const Score &score, const Move best_move) {
  std::vector<Move> pv;
  build_pv(t.board, pv, best_move);
  size_t node_count = Threads.get_node_count();
  auto time_used = std::chrono::duration_cast<Milliseconds>(end-begin);
  if (print_info) {
    std::cout << "info"  
        << " depth "    << depth
        << " seldepth " << (Threads.get_max_depth() - t.board.get_num_made_moves())
        << " time "     << time_used.count()
        << " nodes "    << node_count << " nps " << ((1000*node_count) / (time_used.count()+1));

    // Limiting to hashfull above 1 second reduces my anxiety about hashfull impacting performance.
    // This is also found in SF.
    if (time_used.count() > 1000) {
      std::cout << " hashfull " << table::GetHashfull();
    }

    if (!score.is_mate_score()) {
      std::cout << " score cp ";
      std::cout << (net_evaluation::RemoveContempt(score, t.board.get_turn()).to_cp() / 8);
      if (uci_show_wdl) {
        std::cout << " " << net_evaluation::RemoveContempt(score, t.board.get_turn()).get_uci_string();
      }
    }
    else {
      if (score.is_disadvantage()) {
        NScore n_score = -(score.win - kMinScore.win - (int32_t)t.board.get_num_made_moves()) / 2;
        std::cout << " score mate " << n_score;
        if (uci_show_wdl) {
          std::cout << " wdl 0 0 1000";
        }
      }
      else {
        NScore n_score = (kMaxScore.win - score.win - (int32_t)t.board.get_num_made_moves() + 2) / 2;
        std::cout << " score mate " << n_score;
        if (uci_show_wdl) {
          std::cout << " wdl 1000 0 0";
        }
      }
      //std::cout << " w:" << score.win << " wd:" << score.win_draw;
    }
    std::cout << " pv";
    for (Move move : pv) {
      std::cout << " " << parse::MoveToString(move);
    }
    std::cout << std::endl;
  }
}

void Thread::search() {
  const Time begin = end_time-rsearch_duration;
  double time_factor = 1.0;
  current_depth = 1;
  root_height = board.get_num_made_moves();

  Score score = net_evaluation::ScoreBoard(board);
  set_static_score(score);
  Move last_best = kNullMove;
  std::vector<Score> previous_scores;

  for (Depth depth = current_depth; depth <= rsearch_depth; ++depth) {
    if(finished(*this)) {
      Threads.end_search = true;
      break;
    }

    if (id != 0 && depth > 4) {
      static std::mutex mutex;
      std::lock_guard<std::mutex> lock(mutex);

      size_t count = 0;
      for (Thread* t : Threads.helpers) {
        if (depth <= t->current_depth) {
          count++;
        }
      }
      if (count >= Threads.get_thread_count() / 2) {
        if ((id % 3) != (depth % 3)) {
          continue;
        }
        depth = std::min(depth+1, rsearch_depth);
      }
    }

    current_depth = depth;

    score = PVS(*this, current_depth, previous_scores);
    
    previous_scores.emplace_back(score);
    
    if (id == 0) {
      if (finished(*this)) {
        current_depth--;
      }
      Time end = now();

      PrintUCIInfoString(*this, current_depth, begin, end, last_search_score, best_root_move);

      auto time_used = std::chrono::duration_cast<Milliseconds>(end-begin);
      if (!fixed_search_time) {
        if (last_best == best_root_move) {
          time_factor = std::max(time_factor * 0.9, 0.5);
          if (time_used.count() > (rsearch_duration.count() * time_factor)) {
            end_time = now();
            return;
          }
        }
        else {
          last_best = best_root_move;
          time_factor = 1.0;
        }
      }
    }
  }
}

Move RootSearch(Board &board, Depth depth, Milliseconds duration = Milliseconds(24 * 60 * 60 * 1000)) {
  Threads.is_searching = true;
  rsearch_depth = std::min(depth, settings::kMaxDepth);
  rsearch_duration = duration;
  end_time = now()+rsearch_duration;
  table::UpdateGeneration();
  if (armageddon) {
    net_evaluation::SetContempt(kWhite, 60);
  }
  else {
    net_evaluation::SetContempt(board.get_turn(), contempt);
  }
  draw_score = net_evaluation::GetDrawArray();
  assert(armageddon || contempt != 0 || draw_score[kWhite] == kDrawScore);
  min_ply = board.get_num_made_moves();
  Threads.reset_node_count();
  Threads.reset_depths();
  skip_time_check = std::min((size_t)256, max_nodes);
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  if (moves.size() == 0) {
    std::cout << "info depth 0 score";
    if (board.InCheck()) {
      std::cout << " mate 0";
      if (uci_show_wdl) {
        std::cout << " wdl 0 0 1000";
      }
    }
    else {
      std::cout << " cp 0";
      if (uci_show_wdl) {
        std::cout << " wdl 0 1000 0";
      }
    }
    std::cout << std::endl;
    return kNullMove;
  }
  if (moves.size() == 1 && !fixed_search_time) {
    return moves[0];
  }
  OptEntry entry = table::GetEntry(board.get_hash());
  Move tt_move = kNullMove;
  if (entry.has_value()) {
    tt_move = entry->get_best_move();
  }
  Threads.main_thread->board.SetToSamePosition(board);
  Threads.main_thread->root_height = board.get_num_made_moves();
  Threads.main_thread->max_depth = board.get_num_made_moves();
  move_order::SortML(moves, *Threads.main_thread, tt_move);
  Threads.main_thread->best_root_move = tt_move;
  Threads.end_search = false;
  std::vector<std::thread> helpers;
  for (Thread* t : Threads.helpers) {
    t->board.SetToSamePosition(board);
    t->root_height = board.get_num_made_moves();
    t->best_root_move = tt_move;
    t->max_depth = t->board.get_num_made_moves();
    helpers.emplace_back(std::thread(&Thread::search, t));
  }
  Threads.main_thread->search();
  Threads.end_search = true;
  for (size_t helper_idx = 0; helper_idx < Threads.helpers.size(); ++helper_idx) {
    helpers[helper_idx].join();
  }
  return Threads.main_thread->best_root_move;
}

void set_print_info(bool print_info_) {
  print_info = print_info_;
}

Score get_last_search_score() {
  return last_search_score;
}

size_t get_num_nodes() {
  return Threads.get_node_count();
}

Move DepthSearch(Board board, Depth depth) {
  fixed_search_time = true;
  max_nodes = kInfiniteNodes;
  return RootSearch(board, depth);
}

Move FixedTimeSearch(Board board, Milliseconds duration) {
  fixed_search_time = true;
  max_nodes = kInfiniteNodes;
  return RootSearch(board, 1000, duration);
}

Move TimeSearch(Board board, Milliseconds duration) {
  fixed_search_time = false;
  max_nodes = kInfiniteNodes;
  return RootSearch(board, 1000, duration);
}

Move NodeSearch(Board board, size_t num_nodes) {
  fixed_search_time = true;
  max_nodes = num_nodes;
  return RootSearch(board, 1000);
}

void end_search() {
  end_time = now();
}

void clear_killers_and_counter_moves() {
  Threads.clear_killers_and_countermoves();
}

Score QSearch(Board &board) {
  Threads.main_thread->board.SetToSamePosition(board);
  return QuiescentSearch((*Threads.main_thread), kMinScore, kMaxScore);
}

void SetArmageddon(bool armageddon_) {
  armageddon = armageddon_;
}

void SetUCIShowWDL(bool show_wdl) {
  uci_show_wdl = show_wdl;
}

void SetContempt(int32_t contempt_) {
  contempt = contempt_;
}

#ifdef TUNE

#define SETTER(x) \
void Set##x(int32_t value) { \
  x = value; \
  kFutileMargin = init_futility_margins(); \
  lmr_reductions = init_lmr_reductions(); \
  kLMP = init_lmp_breakpoints(); \
} \
  \
int32_t Get##x() { return x; }

SETTER(kInitialAspirationDelta)

SETTER(kFutilityScaling)
SETTER(kFutilityOffset)
SETTER(kFutilityImproving)

SETTER(kSNMPScaling)
SETTER(kSNMPOffset)
SETTER(kSNMPImproving)

SETTER(kLMROffset)
SETTER(kLMRMult)
SETTER(kLMROffsetCap)
SETTER(kLMRMultCap)
SETTER(kLMROffsetPV)
SETTER(kLMRMultPV)
SETTER(kLMROffsetPVCap)
//SETTER(kLMRMultPVCap)

SETTER(kLMPBaseNW)
SETTER(kLMPBasePV)
SETTER(kLMPScalar)
SETTER(kLMPQuad)

SETTER(kLMPImpBaseNW)
SETTER(kLMPImpBasePV)
SETTER(kLMPImpScalar)
SETTER(kLMPImpQuad)

SETTER(kNMPBase)
SETTER(kNMPScale)
SETTER(kSingularExtensionDepth)

#undef SETTER

#endif

}

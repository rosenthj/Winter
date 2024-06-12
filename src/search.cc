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

struct LMRInitializer {
  double off;
  double mult;
  double off_cap;
  double mult_cap;
  double off_pv;
  double mult_pv;
  double off_pv_cap;
  double mult_pv_cap;
};

Depth lmr_calculator(Depth i, size_t j, double offset, double multiplier) {
  Depth res = std::floor(offset + (std::log(i+1) * std::log(j+1) * multiplier));
  return std::max(std::min(res, i), 0);
}

Array3d<Depth, 64, 64, 4> init_lmr_reductions(const LMRInitializer &x) {
  Array3d<Depth, 64, 64, 4> lmr_reductions;
  for (Depth i = 0; i < 64; ++i) {
    for (size_t j = 0; j < 64; ++j) {
      lmr_reductions[i][j][0] = lmr_calculator(i, j, x.off, x.mult);
      lmr_reductions[i][j][1] = lmr_calculator(i, j, x.off_cap, x.mult * x.mult_cap);
      lmr_reductions[i][j][2] = lmr_calculator(i, j, x.off_pv, x.mult * x.mult_pv);
      lmr_reductions[i][j][3] = lmr_calculator(i, j, x.off_pv_cap, x.mult * x.mult_cap * x.mult_pv);
    }
  }
  return lmr_reductions;
}

const Array2d<Depth, 2, 6> init_lmp_breakpoints(Depth base_nw, Depth base_pv,
                                                    int32_t x, int32_t y) {
  Array2d<Depth, 2, 6> lmp;
  lmp[0][0] = 0;
  lmp[1][0] = 0;
  for (int32_t i = 1; i < lmp[0].size(); ++i) {
    int32_t j = i-1;
    lmp[0][i] = base_nw + ((x*j + y*j*j) / 8);
    lmp[1][i] = base_pv + ((x*j + y*j*j) / 8);
  }
  return lmp;
}

std::array<NScore, 4> init_futility_margins(NScore scaling, NScore offset) {
  std::array<NScore, 4> kFutilityMargins;
  for (size_t i = 0; i < kFutilityMargins.size(); ++i) {
    kFutilityMargins[i] = offset + scaling * i;
  }
  return kFutilityMargins;
}

#ifdef TUNE
NScore kInitialAspirationDelta = 72;
NScore kSNMPScaling = 631;
NScore kSNMPOffset = -46;
NScore kSNMPImproving = 126;
NScore kFutilityScaling = 546;
NScore kFutilityOffset = -10;
NScore kFutilityImproving = 62;
std::array<NScore, 4> kFutileMargin = init_futility_margins(kFutilityScaling,
                                                     kFutilityOffset);
Depth kLMPBaseNW = 4, kLMPBasePV = 6;
int32_t kLMPScalar = 10, kLMPQuad = 6;
Depth kSingularExtensionDepth = 9;
Array2d<Depth, 2, 6> kLMP = init_lmp_breakpoints(kLMPBaseNW, kLMPBasePV, kLMPScalar, kLMPQuad);
#else
constexpr NScore kInitialAspirationDelta = 72;
constexpr NScore kSNMPScaling = 631;
constexpr NScore kSNMPOffset = -46;
constexpr NScore kSNMPImproving = 126;
constexpr NScore kFutilityScaling = 546;
constexpr NScore kFutilityOffset = -10;
constexpr NScore kFutilityImproving = 62;
const std::array<NScore, 4> kFutileMargin = init_futility_margins(kFutilityScaling,
                                                           kFutilityOffset);
const Depth kLMPBaseNW = 4, kLMPBasePV = 6;
const int32_t kLMPScalar = 10, kLMPQuad = 6;
const Depth kSingularExtensionDepth = 9;
const Array2d<Depth, 2, 6> kLMP = init_lmp_breakpoints(kLMPBaseNW, kLMPBasePV, kLMPScalar, kLMPQuad);
#endif
bool uci_show_wdl = true;

// Parameters used to initialize the LMR reduction table
LMRInitializer lmr_initializer {
  -0.16, 0.86,
  0.15, 0.51,
  0.21, 0.86,
  0.23, 0.51 * 0.86
};
Array3d<Depth, 64, 64, 4> lmr_reductions = init_lmr_reductions(lmr_initializer);

template<NodeType node_type>
const Depth get_lmr_reduction(const Depth depth, const size_t move_number, bool cap) {
  assert(depth > 0);
  size_t is_pv = node_type == NodeType::kPV ? 2 : 0;
  return lmr_reductions[std::min(depth - 1, 63)][std::min(move_number, (size_t)63)][is_pv + cap];
}

//std::vector<MoveScore> search_weights(kNumMoveProbabilityFeatures);
//std::vector<MoveScore> search_weights_in_check(kNumMoveProbabilityFeatures);

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

std::mt19937_64 rng;
size_t min_ply = 0;
const size_t kInfiniteNodes = 1000000000000;
size_t max_nodes = kInfiniteNodes;
bool fixed_search_time;

Time end_time = now();

inline bool finished(search::Thread &thread) {
  if (thread.id == 0) {
    if (skip_time_check <= 0) {
      skip_time_check = 256;
      return end_time <= now() || max_nodes < search::Threads.get_node_count()
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

inline bool is_null_move_allowed(const Board &board, const Depth depth) {
  return settings::kUseNullMoves && depth > 1
      && board.has_non_pawn_material(board.get_turn());
      //&& board.get_phase() > 1 * piece_phases[kQueen];// && !board.InCheck();
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

//~ double avg_move_return = 0.;
//~ double avg_move_count = 0.;
//~ double avg_qs_return = 0.;
//~ double avg_qs_count = 0.;
//~ double avg_nw_return = 0.;
//~ double avg_nw_count = 0.;
//~ double avg_nh_return = 0;
//~ double avg_nh_count = 0;

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
    static_eval = net_evaluation::ScoreBoard(t.board);
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
  
  bool moves_sorted = false;
  if (!entry.has_value() || entry->get_best_move() == kNullMove
                         || !SwapToFront(moves, entry->get_best_move())) {
    move_order::Sort(moves, t);
    moves_sorted = true;
  }

  //Move loop
  for (size_t i = 0; i < moves.size(); ++i) {
    if (i == 1 && moves.size() > 2 && !moves_sorted) {
      move_order::Sort(moves, t, 1);
      moves_sorted = true;
    }
    const Move move = moves[i];
    
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
        //~ if (entry.has_value() && entry->get_best_move() == moves[0]) {
          //~ if (i > 0) {
            //~ avg_move_return += i - 1;
            //~ avg_move_count++;
          //~ }
        //~ }
        //~ else {
          //~ avg_qs_return += i;
          //~ avg_qs_count++;
        //~ }
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

void update_capture_history(Thread &t, const Move capture,
                            const int32_t score) {
  const Piece piece = t.board.get_piece(GetMoveSource(capture));
  const Square des = GetMoveDestination(capture);
  if (GetMoveType(capture) == kEnPassant) {
    t.update_capture_score(piece, des, kPawn, score);
  }
  const PieceType piece_type = t.board.get_piece_type(des);
  t.update_capture_score(piece, des, piece_type, score);
}

void update_capture_history(Thread &t, const std::vector<Move> &captures,
                            const Depth depth, const MoveScore last_sign) {
  if (captures.size() == 0) {
    return;
  }
  const int32_t score = std::min(depth * depth, 512);
  for (size_t i = 0; i < captures.size() - 1; ++i) {
    update_capture_history(t, captures[i], -score);
  }
  size_t i = captures.size() - 1;
  update_capture_history(t, captures[i], last_sign * score);
}

void update_counter_move_history(Thread &t, const std::vector<Move> &quiets,
                                 const Depth depth) {
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

inline const Score get_singular_beta(Score beta, Depth depth) {
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
  const Score rBeta = get_singular_beta(beta, depth);
  const Score rAlpha = get_previous_score(rBeta);
  const Depth rDepth = (depth - 3) / 2;
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
  assert(IsQuiet(move));
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
    if (!settings::kUseQS) {
      t.nodes++;
      return net_evaluation::ScoreBoard(t.board);
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
  t.set_static_score(kNoScore);
  bool strict_worsening = false;
  bool nmp_failed_node = false;

  //Speculative pruning methods
  if (node_type == NodeType::kNW && !exclude_move && beta.is_static_eval() && !in_check) {

    //Set static eval from board and TT entry.
    if (entry.has_value()) {
      if (entry->get_bound() == kExactBound) {
        static_eval = entry->get_score(t.board);
      }
      else {
        static_eval = net_evaluation::ScoreBoard(t.board);
        if ( (entry->get_bound() == kLowerBound && static_eval < entry->get_score(t.board))
            || (entry->get_bound() == kUpperBound && static_eval > entry->get_score(t.board)) ) {
          static_eval = entry->get_score(t.board);
        }
      }
    }
    else {
      static_eval = net_evaluation::ScoreBoard(t.board);
    }
    t.set_static_score(static_eval);
    strict_worsening = t.strict_worsening();

    //Static Null Move Pruning
    if (node_type == NodeType::kNW && depth <= 5) {
      NScore margin = kSNMPOffset + (kSNMPScaling - kSNMPImproving * !strict_worsening) * depth;
      if (settings::kUseScoreBasedPruning && static_eval.value() > beta.value() + margin
          && t.board.get_phase() > 1 * piece_phases[kQueen]) {
        if (static_eval.is_mate_score()) {
          return beta;
        }
        return (static_eval + beta) / 2;
      }
    }

    //Null Move Pruning
    if (static_eval >= beta && is_null_move_allowed(t.board, depth)) {
      t.set_move(kNullMove);
      t.board.Make(kNullMove);
      const Depth R = 3 + depth / 5;
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

  bool moves_sorted = false;
  if (tt_entry == kNullMove || !SwapToFront(moves, tt_entry)) {
    move_order::SortML(moves, t);
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
  std::vector<Move> quiets, captures;
  Score alpha_nw = alpha.get_next_score();
  //Move loop
  for (size_t i = 0; i < moves.size(); ++i) {
    if (i == 1 && moves.size() > 2 && !moves_sorted) {
      move_order::SortML(moves, t, 1);
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
      if (!is_root && (size_t)depth < kLMP[0].size() && (i >= (size_t)kLMP[node_type == NodeType::kPV][depth])
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
          && static_eval.value() < (alpha.value() - get_futility_margin(depth - reduction, !strict_worsening))
          && GetMoveType(move) < kEnPassant) {
        continue;
      }
    }

    if (IsQuiet(move)) {
      quiets.emplace_back(move);
    }
    else {
      captures.emplace_back(move);
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
      if (IsQuiet(move)) {
        update_counter_move_history(t, quiets, depth);
        update_killers(t, move);
        update_capture_history(t, captures, depth, -1);
      }
      else {
        update_capture_history(t, captures, depth, 1);
      }
      if (is_root) {
        t.best_root_move = move;
      }
      
      if (node_type == NodeType::kNW) {
        //~ if (entry.has_value() && entry->get_best_move() == moves[0]) {
          //~ if (i > 0) {
            //~ avg_nw_count++;
            //~ avg_nw_return += i-1;
          //~ }
        //~ }
        //~ else {
          //~ avg_nh_count++;
          //~ avg_nh_return += i;
        //~ }
      }

      return score;
    }

    //In PV nodes we might be improving Alpha without breaking Beta
    if (score > alpha) {
      if (IsQuiet(move)) {
        update_counter_move_history(t, quiets, depth);
        //update_capture_history(t, captures, depth, -1);
      }
      else {
        //update_capture_history(t, captures, depth, 1);
      }
      //Update score and expected best move
      alpha = score;
      alpha_nw = get_next_score(alpha);
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
  }
  else if (!exclude_move) {
    table::SaveEntry(t.board, best_local_move, lower_bound_score, depth, kUpperBound);
  }

  return lower_bound_score;
}

inline Score PVS(Thread &t, Depth current_depth, const std::vector<Score> &previous_scores) {
  if (current_depth <= 4) {
    return AlphaBeta<NodeType::kPV>(t, kMinScore, kMaxScore, current_depth);
  }
  else {
    Score score = previous_scores.back();
    Score delta = WDLScore{kInitialAspirationDelta, kInitialAspirationDelta};
    Score alpha = (score-delta).get_valid_score();
    Score beta = (score+delta).get_valid_score();
    score = AlphaBeta<NodeType::kPV>(t, alpha, beta, current_depth);
    while (!finished(t) && (score <= alpha || score >= beta)) {
      assert(delta.win > 0 && delta.win_draw > 0);
      if (score <= alpha) {
        if (beta.is_static_eval()) {
          beta = (std::max(score, kMinStaticEval) + (beta * 3)) / 4;
          assert(beta.is_valid());
        }
        alpha = (score-delta).get_valid_score();
        if (alpha == score) {
          alpha = get_previous_score(score);
        }
      }
      else if (score >= beta) {
        if (alpha.is_static_eval()) {
          alpha = ((alpha * 3) + std::min(kMaxStaticEval, score)) / 4;
          assert(alpha.is_valid());
        }
        beta = (score + delta).get_valid_score();
        if (beta == score) {
          beta = get_next_score(score);
        }
      }
      assert(score > alpha && score < beta);
      score = AlphaBeta<NodeType::kPV>(t, alpha, beta, current_depth);
      delta *= 2;
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
    std::cout << "info "  << " depth " << depth
        << " seldepth " << (Threads.get_max_depth() - t.board.get_num_made_moves())
        << " time " << time_used.count()
        << " nodes " << node_count << " nps " << ((1000*node_count) / (time_used.count()+1));

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
      }
      else {
        NScore n_score = (kMaxScore.win - score.win - (int32_t)t.board.get_num_made_moves() + 2) / 2;
        std::cout << " score mate " << n_score;
      }
      //std::cout << " w:" << score.win << " wd:" << score.win_draw;
    }
    //~ std::cout << " avg " << (avg_move_return / avg_move_count) << " " << (avg_qs_return / avg_qs_count) << " " << (avg_nw_return / avg_nw_count) << " " << (avg_nh_return / avg_nh_count);;
    std::cout << " pv";
    for (Move move : pv) {
      std::cout << " " << parse::MoveToString(move);
    }
    std::cout << std::endl;
  }
}

void Thread::search() {
  const Time begin = now();
  double time_factor = 1.0;
  current_depth = 1;
  root_height = board.get_num_made_moves();
  if (id == 0) {
    end_time = begin+rsearch_duration;
  }

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

    if(!finished(*this)) {
      last_search_score = score;
      previous_scores.emplace_back(score);
      if (id != 0) {
        continue;
      }
      Time end = now();

      PrintUCIInfoString(*this, current_depth, begin, end, score, best_root_move);

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
  rsearch_depth = std::min(depth, settings::kMaxDepth);
  rsearch_duration = duration;
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  assert(moves.size() != 0);
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
  bool tt_move_in_front = SwapToFront(moves, tt_move);
  move_order::SortML(moves, *Threads.main_thread, tt_move_in_front);
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

void SetContempt(int32_t contempt_) {
  contempt = contempt_;
}

void SetArmageddon(bool armageddon_) {
  armageddon = armageddon_;
}

void SetUCIShowWDL(bool show_wdl) {
  uci_show_wdl = show_wdl;
}

#ifdef TUNE
void SetInitialAspirationDelta(int32_t delta) {
  kInitialAspirationDelta = delta;
}

void SetFutilityScaling(int32_t scaling) {
  kFutilityScaling = scaling;
  kFutileMargin = init_futility_margins(kFutilityScaling, kFutilityOffset);
}

void SetFutilityOffset(int32_t offset) {
  kFutilityOffset = offset;
  kFutileMargin = init_futility_margins(kFutilityScaling, kFutilityOffset);
}

void SetFutilityImproving(int32_t offset) {
  kFutilityImproving = offset;
}

void SetSNMPScaling(int32_t score) {
  kSNMPScaling = score;
}

void SetSNMPOffset(int32_t score) {
  kSNMPOffset = score;
}

void SetSNMPImproving(int32_t score) {
  kSNMPImproving = score;
}

void SetLMROffset(int32_t value) {
  lmr_initializer.off = 0.01 * value;
  lmr_reductions = init_lmr_reductions(lmr_initializer);
}

void SetLMRMultiplier(int32_t value) {
  lmr_initializer.mult = 0.01 * value;
  lmr_reductions = init_lmr_reductions(lmr_initializer);
}

void SetLMROffsetCap(int32_t value) {
  lmr_initializer.off_cap = 0.01 * value;
  lmr_reductions = init_lmr_reductions(lmr_initializer);
}

void SetLMRMultiplierCap(int32_t value) {
  lmr_initializer.mult_cap = 0.01 * value;
  lmr_reductions = init_lmr_reductions(lmr_initializer);
}

void SetLMROffsetPV(int32_t value) {
  lmr_initializer.off_pv = 0.01 * value;
  lmr_reductions = init_lmr_reductions(lmr_initializer);
}

void SetLMRMultiplierPV(int32_t value) {
  lmr_initializer.mult_pv = 0.01 * value;
  lmr_reductions = init_lmr_reductions(lmr_initializer);
}

void SetLMROffsetPVCap(int32_t value) {
  lmr_initializer.off_pv_cap = 0.01 * value;
  lmr_reductions = init_lmr_reductions(lmr_initializer);
}

//void SetLMRMultiplierPVCap(int32_t value) {
//  lmr_initializer.mult_pv_cap = 0.01 * value;
//  lmr_reductions = init_lmr_reductions(lmr_initializer);
//}

void ResetLMP() {
  kLMP = init_lmp_breakpoints(kLMPBaseNW, kLMPBasePV, kLMPScalar, kLMPQuad);
}

void SetLMPBaseNW(int32_t value) {
  kLMPBaseNW = value;
  ResetLMP();
}
void SetLMPBasePV(int32_t value) {
  kLMPBasePV = value;
  ResetLMP();
}
void SetLMPScalar(int32_t value) {
  kLMPScalar = value;
  ResetLMP();
}
void SetLMPQuadratic(int32_t value) {
  kLMPQuad = value;
  ResetLMP();
}

void SetSingularExtensionDepth(int32_t depth) {
  kSingularExtensionDepth = depth;
}

#endif

}

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
 * search_thread.cc
 *
 *  Created on: Nov 3, 2018
 *      Author: Jonathan Rosenthal
 */

#include "search_thread.h"
#include "general/types.h"
#include "general/bit_operations.h"
#include <random>
#include <algorithm>
#include <iostream>

namespace {
std::mt19937_64 rng;
}

namespace search {

#ifdef TUNE
#define EXPR
#else
#define EXPR constexpr
#endif

EXPR float kPawnCorrectionScale =  180.5 / 256.0f;
EXPR float kMinorCorrectionScale = 119.3 / 256.0f;
EXPR float kMajorCorrectionScale = 119.3 / 256.0f;
EXPR float kRNGCorrectionScale =      88 / 256.0f;
EXPR float kSafetyCorrectionScale =  160 / 256.0f;
EXPR float kCorrectionLeakScale =    307 / 256.0f;

#undef EXPR

ThreadPool Threads;

Thread::Thread() {
  id = 1;//This should be immediately set to something else. It is set here only to guarantee non-zero for helpers.
  initialized = false;
}

void Thread::set_move(Move move) {
  Depth height = get_height();
  if (move == kNullMove) {
    passed_moves[height] = { kNoPiece, 0 };
  }
  else {
    passed_moves[height].pt = GetPieceType(board.get_piece(GetMoveSource(move)));
    passed_moves[height].des = GetMoveDestination(move);
    assert(passed_moves[get_height()].pt != kNoPiece);
  }
}

PieceTypeAndDestination Thread::get_previous_move(Depth moves_ago) const {
  return passed_moves[std::max(get_height() - moves_ago, 0)];
}

Depth Thread::get_height() const {
  assert(root_height >= 0);
  assert(board.get_num_made_moves() >= (size_t)root_height);
  return std::min((Depth)board.get_num_made_moves() - root_height, settings::kMaxDepth - 1);
}

bool Thread::improving() const {
  Depth height = std::min((Depth)board.get_num_made_moves() - root_height, settings::kMaxDepth - 1);
  // kNoScore is defined as smaller than min score, so the second condition also implies
  // that we have a score at current height.
  assert(height >= 0);
  return height >= 2 && static_scores[height-2] != kNoScore
         && static_scores[height] != kNoScore
         && static_scores[height] > static_scores[height-2];
}

//~ bool Thread::strict_improving() const {
  //~ Depth height = std::min((Depth)board.get_num_made_moves() - root_height, settings::kMaxDepth - 1);
  //~ // kNoScore is defined as smaller than min score, so the second condition also implies
  //~ // that we have a score at current height.
  //~ assert(height >= 0);
  //~ return height >= 2 && static_scores[height] > static_scores[height-2]
                     //~ && static_scores[height-2] != kNoScore;
//~ }

//~ bool Thread::worsening() const {
  //~ Depth height = std::min((Depth)board.get_num_made_moves() - root_height, settings::kMaxDepth - 1);
  //~ // kNoScore is defined as smaller than min score, so the second condition also implies
  //~ // that we have a score at current height.
  //~ assert(height >= 0);
  //~ return height >= 2 && (static_scores[height] < static_scores[height-2]
                      //~ || static_scores[height] == kNoScore);
//~ }

inline float sclamp(const float value, const float lower, const float upper) {
  assert(lower < upper);
  return std::min(std::max(value, lower), upper);
}

inline void gravity_float_update(float &entry, float value, float leak) {
  entry += value - entry * (std::abs(value) + leak) / 1024;
}

void accumulate_errors(const ErrorHistory &history, const HashType error_hash,
                       float &win_error, float &loss_error, float scale=1.0) {
  size_t idx = error_hash % kErrorHistorySize;
  
  win_error  += history[idx][0] * scale;
  loss_error += history[idx][1] * scale;
}

inline size_t get_safety_correction_offset(const Board &board, const Color color) {
  BitBoard king_bitboard = board.get_piece_bitboard(color, kKing);
  Square king_square = bitops::BitBoardToSquare(king_bitboard);
  return color + 2 * (std::min(std::max(GetSquareX(king_square), 1), 6)-1);
}

Score Thread::adjust_static_eval(const Score static_eval) const {
  if (!static_eval.is_static_eval())
    return static_eval;
  
  float win_error = 0, loss_error = 0;
  
  accumulate_errors(pawn_error_history, board.get_pawn_hash(),
                    win_error, loss_error, kPawnCorrectionScale);
  accumulate_errors(major_error_history, board.get_major_hash(),
                    win_error, loss_error, kMajorCorrectionScale);
  accumulate_errors(minor_error_history, board.get_major_hash() >> 32,
                    win_error, loss_error, kMinorCorrectionScale);
  for (int e_idx = 0; e_idx < kNumRngHash; ++e_idx) {
    for (int shift = 0; shift < 4; ++shift) {
      HashType rng_hash = (board.get_rng_hash(e_idx) >> (shift * 16)) & 0xFFFF;
      accumulate_errors(rng_error_history[e_idx][shift], rng_hash,
                        win_error, loss_error, kRNGCorrectionScale);
    }
  }
  for (Color color = kWhite; color <= kBlack; ++color) {
    size_t idx = get_safety_correction_offset(board, color);
    HashType safety_hash = (board.get_safety_hash(idx / 4) >> ((idx % 4) * 16)) & 0xFFFF;
    accumulate_errors(safety_error_history[idx % 4], safety_hash,
                      win_error, loss_error, kSafetyCorrectionScale);
  }
  
  if (board.get_turn() == kBlack) {
    std::swap(win_error, loss_error);
  }
  
  float win = static_eval.get_win_probability();
  float loss = static_eval.get_loss_probability();
    
  constexpr float divisor = 8192.0f;
  win += win_error / divisor;
  loss += loss_error / divisor;
  
  win = sclamp(win, 0.0f, 1.0f);
  loss = sclamp(loss, 0.0f, 1.0f);
  
  float sum = win + loss;
  if (sum > 1.0) {
    win /= sum;
    loss /= sum;
  }
  
  return WDLScore::from_pct(win, loss);
}

void update_specific_history(ErrorHistory &history, const HashType error_hash,
                             float win_val, float loss_val, float leak) {
  size_t idx = error_hash % kErrorHistorySize;

  gravity_float_update(history[idx][0], win_val, leak);
  gravity_float_update(history[idx][1], loss_val, leak);
}

void Thread::update_error_history(const Score eval, Depth depth) {
  assert((Depth)board.get_num_made_moves() >= root_height);  
  Depth height = std::min((Depth)board.get_num_made_moves() - root_height, settings::kMaxDepth - 1);
  Score static_eval = static_scores[height];
  
  if (!static_eval.is_static_eval()) return;
  
  float win_error  = eval.get_win_probability()  - static_eval.get_win_probability();
  float loss_error = eval.get_loss_probability() - static_eval.get_loss_probability();
  if (board.get_turn() == kBlack) {
    std::swap(win_error, loss_error);
  }
  
  constexpr float scale = 64.0f;
  constexpr float limit = 200.0f; // To prevent single-move spikes

  float win_val = sclamp(win_error * depth * scale, -limit, limit);
  float loss_val = sclamp(loss_error * depth * scale, -limit, limit);
  
  // constexpr float kBaseLeak = 1.0f;
  float leak = kCorrectionLeakScale * static_cast<float>(std::min(depth, 16));
  
  update_specific_history(pawn_error_history, board.get_pawn_hash(), win_val, loss_val, leak);
  update_specific_history(major_error_history, board.get_major_hash(), win_val, loss_val, leak);
  update_specific_history(minor_error_history, board.get_major_hash() >> 32, win_val, loss_val, leak);
  for (int e_idx = 0; e_idx < kNumRngHash; ++e_idx) {
    for (int shift = 0; shift < 4; ++shift) {
      HashType rng_hash = (board.get_rng_hash(e_idx) >> (shift * 16)) & 0xFFFF;
      update_specific_history(rng_error_history[e_idx][shift], rng_hash, win_val, loss_val, leak);
    }
  }
  for (Color color = kWhite; color <= kBlack; ++color) {
    size_t idx = get_safety_correction_offset(board, color);
    HashType safety_hash = (board.get_safety_hash(idx / 4) >> ((idx % 4) * 16)) & 0xFFFF;
    update_specific_history(safety_error_history[idx % 4], safety_hash, win_val, loss_val, leak);
  }
}

int32_t Thread::get_history_score(const Color color, const Square src,
                                  const Square des) const {
  return history[color][src][des];
}

void Thread::update_history_score(const Color color, const Square src, const Square des,
                             const int32_t score) {
  history[color][src][des] += 32 * score - history[color][src][des] * std::abs(score) / 512;
}

//~ bool Thread::strict_worsening() const {
  //~ Depth height = std::min((Depth)board.get_num_made_moves() - root_height, settings::kMaxDepth - 1);
  //~ // kNoScore is defined as smaller than min score, so the second condition also implies
  //~ // that we have a score at current height.
  //~ assert(height >= 0);
  //~ return height >= 2 && static_scores[height] < static_scores[height-2];
//~ }

template<int moves_ago>
int32_t Thread::get_continuation_score(const PieceType opp_piecetype, const Square opp_des,
                      const PieceType piecetype, const Square des) const {
  const int idx = moves_ago > 2 ? moves_ago - 2 : moves_ago - 1;
  assert(idx >= 0 && idx < continuation_history.size());
  return continuation_history[idx][opp_piecetype][opp_des][piecetype][des];
}

template<int moves_ago>
void Thread::update_continuation_score(const PieceType opp_piecetype, const Square opp_des,
                       const PieceType piecetype, const Square des, const int32_t score) {
  const int idx = moves_ago > 2 ? moves_ago - 2 : moves_ago - 1;
  assert(idx >= 0 && idx < continuation_history.size());
  continuation_history[idx][opp_piecetype][opp_des][piecetype][des] += 32 * score
      - continuation_history[idx][opp_piecetype][opp_des][piecetype][des]
                    * std::abs(score) / 512;
}

template<int moves_ago>
int32_t Thread::get_continuation_score(const Move move) const {
  if (get_height() < 2) {
    return 0;
  }
  const PieceTypeAndDestination pt_and_des = get_previous_move(2);
  if (pt_and_des.pt == kNoPiece) {
    return 0;
  }
  const PieceType piece_type = GetPieceType(board.get_piece(GetMoveSource(move)));
  const Square des = GetMoveDestination(move);
  const int idx = moves_ago > 2 ? moves_ago - 2 : moves_ago - 1;
  assert(idx >= 0 && idx < continuation_history.size());
  return continuation_history[idx][pt_and_des.pt][pt_and_des.des][piece_type][des];
}

ThreadPool::ThreadPool() {
  main_thread = new Thread();
  main_thread->id = 0;
  is_searching = false;
}

void ThreadPool::set_num_threads(size_t num_threads) {
  assert(num_threads > 0);
  size_t num_helpers = num_threads - 1;

  //Create new threads if we have too few
  while(helpers.size() < num_helpers) {
    helpers.push_back(new Thread());
    helpers.back()->id = helpers.size();
  }

  //Kill helper threads if we have too many
  while(helpers.size() > num_helpers) {
    delete helpers.back();
    helpers.pop_back();
  }
}

void ThreadPool::clear_killers_and_countermoves() {
  for (Thread* thread : helpers) {
    if (thread->initialized) {
      thread->clear_killers_and_counter_moves();
    }
  }
  if (main_thread->initialized) {
    main_thread->clear_killers_and_counter_moves();
  }
}

void ThreadPool::reset_depths() {
  for (Thread* thread : helpers) {
    thread->current_depth = 1;
  }
  main_thread->current_depth = 1;
}

size_t ThreadPool::get_thread_count() const {
  return helpers.size() + 1;
}
size_t ThreadPool::get_node_count() const {
  size_t sum = main_thread->nodes;
  for (Thread* helper : helpers) {
    sum += helper->nodes;
  }
  return sum;
}

size_t ThreadPool::get_max_depth() const {
  size_t max_d = main_thread->max_depth;
  for (Thread* helper : helpers) {
    if (helper->max_depth > max_d) {
      max_d = helper->max_depth;
    }
  }
  return max_d;
}

void ThreadPool::reset_node_count() {
  main_thread->nodes = 0;
  for (Thread* helper : helpers) {
    helper->nodes = 0;
  }
}

template int32_t Thread::get_continuation_score<1>(const PieceType opp_piecetype, const Square opp_des,
                                                   const PieceType piecetype, const Square des) const;

template void Thread::update_continuation_score<1>(const PieceType opp_piecetype, const Square opp_des,
                                                   const PieceType piecetype, const Square des, const int32_t score);

template int32_t Thread::get_continuation_score<2>(const Move move) const;

template void Thread::update_continuation_score<2>(const PieceType opp_piecetype, const Square opp_des,
                                                   const PieceType piecetype, const Square des, const int32_t score);


void SetNumThreads(int32_t value) { Threads.set_num_threads(value); }
size_t GetNumThreads() { return Threads.get_thread_count(); }

#ifdef TUNE

#define SETTER(x) \
void Set##x(int32_t value) { \
  x = value / 256.0f; \
} \
  \
int32_t Get##x() { return std::lround(256 * x); }

SETTER(kPawnCorrectionScale)
SETTER(kMajorCorrectionScale)
SETTER(kMinorCorrectionScale)
SETTER(kRNGCorrectionScale)
SETTER(kCorrectionLeakScale)

#undef SETTER

#endif

}

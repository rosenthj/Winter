/*
 *  Winter is a UCI chess engine.

 *
 *
 *
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
 * search.cc
 *
 *  Created on: Nov 6, 2016
 *      Author: Jonathan Rosenthal
 */

#include "search.h"
#include "net_evaluation.h"
#include "transposition.h"
#include "data.h"
#include "general/feature_indexes.h"
#include "general/hardcoded_params.h"
#include "general/magic.h"
#include "benchmark.h"
#include "search_thread.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <mutex>
#include <random>
#include <utility>

using namespace move_features;

namespace {

enum class NodeType {
  kPV, kNW
};

const int kNormalSearchMode = 0;
const int kSamplingSearchMode = 1;
const int kSamplingEvalMode = 2;
int kNodeCountSampleAt = 1000;
//int kNodeCountSampleEvalAt = 5000;
#ifdef SAMPLE_SEARCH
const int kMaxDepthSampled = 32;
#endif

//std::vector<double_t> fail_high(47);
//std::vector<double_t> all(47);

int32_t contempt = 0;
bool armageddon = false;
std::array<Score, 2> draw_score { kDrawScore, kDrawScore };

int rsearch_mode;
Milliseconds rsearch_duration;
Depth rsearch_depth;

Board sampled_board;
Score sampled_alpha;
#ifdef SEARCH_TRAINING
NodeType sampled_node_type;
Depth sampled_depth;
#endif
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

Vec<NScore, 4> init_futility_margins(NScore s) {
  Vec<NScore, 4> kFutilityMargins(0);
  for (size_t i = 0; i < kFutilityMargins.size(); ++i) {
    kFutilityMargins[i] = s * i;
  }
  return kFutilityMargins;
}

#ifdef TUNE
NScore kInitialAspirationDelta = 40;
NScore kSNMPMargin = 790;
Vec<NScore, 4> kFutileMargin = init_futility_margins(560);
Depth kLMPBaseNW = 3, kLMPBasePV = 5;
int32_t kLMPScalar = 12, kLMPQuad = 4;
Array2d<Depth, 2, 6> kLMP = init_lmp_breakpoints(kLMPBaseNW, kLMPBasePV, kLMPScalar, kLMPQuad);
#else
constexpr NScore kInitialAspirationDelta = 40;
constexpr NScore kSNMPMargin = 790;
const Vec<NScore, 4> kFutileMargin = init_futility_margins(560);
const Depth kLMPBaseNW = 3, kLMPBasePV = 5;
const int32_t kLMPScalar = 12, kLMPQuad = 4;
const Array2d<Depth, 2, 6> kLMP = init_lmp_breakpoints(kLMPBaseNW, kLMPBasePV, kLMPScalar, kLMPQuad);
#endif
bool uci_show_wdl = true;

// Parameters used to initialize the LMR reduction table
LMRInitializer lmr_initializer {
  0.16, 0.74,
  0.33, 0.30,
  0.72, 0.76,
  0.50, 0.30 * 0.76
};
Array3d<Depth, 64, 64, 4> lmr_reductions = init_lmr_reductions(lmr_initializer);

template<NodeType node_type>
const Depth get_lmr_reduction(const Depth depth, const size_t move_number, bool cap) {
  assert(depth > 0);
  size_t is_pv = node_type == NodeType::kPV ? 2 : 0;
  return lmr_reductions[std::min(depth - 1, 63)][std::min(move_number, (size_t)63)][is_pv + cap];
}

std::vector<MoveScore> search_weights(kNumMoveProbabilityFeatures);
std::vector<MoveScore> search_weights_in_check(kNumMoveProbabilityFeatures);

Score last_search_score;

bool print_info = true;

struct Sorter {
  bool operator() (Move i, Move j) {
    return (i >> 16) > (j >> 16);
  };
};

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
size_t sample_nodes = 0;
size_t evaluation_nodes = 0;
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

#ifdef SEARCH_TRAINING
inline Time get_infinite_time() {
  return now()+std::chrono::hours(24);
}
#endif

size_t get_move_priority_idx(const Move move, search::Thread &t, const Move best) {
  if (move == best)
    return qs_move_features::kPWIHashMove;
  else if (GetMoveType(move) > kCapture) {
    return qs_move_features::kPWIPromotion + GetMoveType(move) - kCapture - 1 + (4 * (GetPieceType(t.board.get_piece(GetMoveDestination(move))) / kNoPiece));
  }
  else if (GetMoveType(move) == kCapture) {
    return qs_move_features::kPWICapture
           + 6 * GetPieceType(t.board.get_piece(GetMoveDestination(move)))
           + GetPieceType(t.board.get_piece(GetMoveSource(move)));
    //return 1000 + 10 * GetPieceType(t.board.get_piece(GetMoveDestination(move)))
    //            - GetPieceType(t.board.get_piece(GetMoveSource(move)));
  }
  return qs_move_features::kNumQSMoveProbabilityFeatures;
}

//MoveScore get_move_priority(const Move move, search::Thread &t, const Move best) {
  //if (move == best)
    //return hardcode::qs_params[qs_move_features::kPWIHashMove];
  //else if (GetMoveType(move) > kCapture) {
    //return hardcode::qs_params[qs_move_features::kPWIPromotion + GetMoveType(move) - kCapture - 1 + (4 * (GetPieceType(t.board.get_piece(GetMoveDestination(move))) / kNoPiece))];
  //}
  //else if (GetMoveType(move) == kCapture) {
    //return hardcode::qs_params[qs_move_features::kPWICapture
                               //+ 6 * GetPieceType(t.board.get_piece(GetMoveDestination(move)))
                               //+ GetPieceType(t.board.get_piece(GetMoveSource(move)))];
    ////return 1000 + 10 * GetPieceType(t.board.get_piece(GetMoveDestination(move)))
    ////            - GetPieceType(t.board.get_piece(GetMoveSource(move)));
  //}
  //return 20 + t.get_history_score(t.board.get_turn(), GetMoveSource(move), GetMoveDestination(move)) / 1000;
//}

MoveScore get_move_priority(const Move move, search::Thread &t, const Move best) {
  if (move == best)
    return 20000;
  else if (GetMoveType(move) == kCapture) {
    return 1000 + 10 * GetPieceType(t.board.get_piece(GetMoveDestination(move)))
                - GetPieceType(t.board.get_piece(GetMoveSource(move)));
  }
  else if (GetMoveType(move) > kCapture) {
    return 10000 + GetMoveType(move) - (GetPieceType(t.board.get_piece(GetMoveDestination(move))) / kNoPiece);
  }
  return t.get_history_score(t.board.get_turn(), GetMoveSource(move), GetMoveDestination(move)) / 1000;
}

void SortMoves(std::vector<Move> &moves, search::Thread &t, const Move best_move) {
  for (size_t i = 0; i < moves.size(); ++i) {
    moves[i] |= (get_move_priority(moves[i], t, best_move) << 16);
  }
  std::sort(moves.begin(), moves.end(), Sorter());
  for (size_t i = 0; i < moves.size(); ++i) {
    moves[i] &= 0xFFFFL;
  }
}

template<typename T>
T init() {
  return T(kNumMoveProbabilityFeatures);
}

template<> int init<int>() { return 0; }

template<typename T, bool in_check> inline
void AddFeature(T &s, const size_t index) {
  assert(index < kNumMoveProbabilityFeatures);
  s[index] = 1;
}

template<> inline void AddFeature<MoveScore, false>(MoveScore &s,  const size_t index) {
  s += search_weights[index];
}

template<> inline void AddFeature<MoveScore, true>(MoveScore &s,  const size_t index) {
  s += search_weights_in_check[index];
}

template<typename T, bool in_check> inline
void AddFeature(T &s, const size_t index, int val) {
  s[index] = val;
}

template<> inline void AddFeature<MoveScore, false>(MoveScore &s,  const size_t index, int val) {
  s += search_weights[index] * val;
}

template<> inline void AddFeature<MoveScore, true>(MoveScore &s,  const size_t index, int val) {
  s += search_weights_in_check[index] * val;
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
    if (GetMoveType(move) < kCapture) {
      const Color color = t.board.get_turn();
      const PieceType moving_piece = GetPieceType(t.board.get_piece(GetMoveSource(move)));
      const Square source = GetMoveSource(move);
      const Square destination = GetMoveDestination(move);
      const int32_t score = t.get_continuation_score<1>(last_moved_piece, last_destination,
                                         moving_piece, destination);
      AddFeature<T, in_check>(move_weight, kPWICMH, score / 1000);
      AddFeature<T, in_check>(move_weight, kPWICMH + 1, t.get_continuation_score<2>(move) / 1000);
      AddFeature<T, in_check>(move_weight, kPWIHistory, t.get_history_score(color, source, destination) / 1000);
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

// Sorten moves according to weights given by some classifier
void SortMovesML(std::vector<Move> &moves, search::Thread &t, const Move best_move = kNullMove) {
  MoveOrderInfo info(t.board, best_move);

  //Move ordering is very different if we are in check. Eg a queen move not capturing anything is less likely.
  if (t.board.InCheck()) {
    for (size_t i = 0; i < moves.size(); ++i) {
      moves[i] |= ((10000 + GetMoveWeight<MoveScore, true>(moves[i], t, info)) << 16);
    }
  }
  else {
    for (size_t i = 0; i < moves.size(); ++i) {
      moves[i] |= ((10000 + GetMoveWeight<MoveScore, false>(moves[i], t, info)) << 16);
    }
  }

  std::sort(moves.begin(), moves.end(), Sorter());
  for (size_t i = 0; i < moves.size(); ++i) {
    moves[i] &= 0xFFFFL;
  }
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
  table::Entry entry = table::GetEntry(board.get_hash());
  bool entry_verified = table::ValidateHash(entry, board.get_hash());

  if (entry_verified) {
    std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
    for (Move move : moves) {
      if ((move == entry.get_best_move() && entry_verified)) {
        return build_pv(board, pv, move);
      }
    }
  }
}

inline bool sufficient_bounds(const Board &board, const table::Entry &entry,
                              const Score alpha, const Score beta,
                              const Depth depth) {
  Score score = entry.get_score(board);
  return entry.depth >= depth
      && ((entry.get_bound() == kExactBound)
          || (entry.get_bound() == kLowerBound && score >= beta)
          || (entry.get_bound() == kUpperBound && score <= alpha));
}

inline bool is_null_move_allowed(const Board &board, const Depth depth) {
  return settings::kUseNullMoves && depth > 1
      && board.has_non_pawn_material(board.get_turn());
      //&& board.get_phase() > 1 * piece_phases[kQueen];// && !board.InCheck();
}

#ifdef UNUSED
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
#endif

}

namespace search {

std::vector<Move> GetSortedMovesML(Board &board) {
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  Threads.main_thread->board.SetToSamePosition(board);
  SortMovesML(moves, *Threads.main_thread);
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

Board get_sampled_board() {
  return sampled_board;
}

template<int Mode>
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
  const table::Entry entry = table::GetEntry(t.board.get_hash());
  const bool valid_hash = table::ValidateHash(entry,t.board.get_hash());
  if (valid_hash) {
    if (sufficient_bounds(t.board, entry, alpha, beta, 0)) {
      return entry.get_score(t.board);
    }
    // assert(entry.bound == kLowerBound || entry.bound == kExactBound);
    if (entry.get_bound() != kUpperBound) {
      lower_bound_score = entry.get_score(t.board);
    }
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

    static_eval = net_evaluation::ScoreBoard(t.board);
//    std::cout << "QS Eval return: (w:" << static_eval.win << ", wd:" << static_eval.win_draw << ")" << std::endl;
    if (valid_hash && entry.get_bound() == kLowerBound && static_eval < entry.get_score(t.board)) {
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
  if (valid_hash) {
    //best_move = entry.get_best_move();
    SortMoves(moves, t, entry.get_best_move());
    //SortMovesML(moves, board, entry.best_move);
  }
  else {
    SortMoves(moves, t, 0);
    //SortMovesML(moves, board, 0);
  }
  
  //std::vector<size_t> move_priorities(47);
  //Move loop
  for (Move move : moves) {
    //size_t move_priority_idx = get_move_priority_idx(move, t, best_move);
    //if (!in_check) {
    //  move_priorities[move_priority_idx]++;
    //}
    //SEE pruning
    //Exception for checking moves: -16.03 +/- 10.81
    if (!in_check && GetMoveType(move) != kEnPassant && !t.board.NonNegativeSEE(move)) {
      continue;
    }

    //Make move, search and unmake
    t.board.Make(move);
    Score score = -QuiescentSearch<Mode>(t, -beta, -alpha);
    t.board.UnMake();

    if (score > lower_bound_score) {
      
      //if (!in_check) {
      //  fail_high[move_priority_idx]++;
      //  for (size_t i = 0; i < 46; ++i) {
      //    all[i] += move_priorities[i];
      //    move_priorities[i] = 0;
      //  }
      //}
      
      //Return beta if we fail high
      if (score >= beta) {
        //table::SaveEntry(board, move, score, 0);
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
  return kFutileMargin[depth] + 100 * depth * improving;
}

#ifdef SAMPLE_SEARCH
inline void end_search_time() {
  end_time = now();
}

Score sample_node_and_return_alpha(const Board &board, const Depth depth,
                                   const NodeType node_type, const Score alpha) {
  sampled_board.SetToSamePosition(board);
  sampled_depth = depth;
  sampled_node_type = node_type;
  sampled_alpha = alpha;
  end_search_time();
  return alpha;
}
#endif

std::pair<bool, Score> move_is_singular(Thread &t, const Depth depth,
                                       const std::vector<Move> &moves,
                                       const table::Entry &entry);

void update_counter_move_history(Thread &t, const std::vector<Move> &quiets, const Depth depth) {
  if (t.board.get_num_made_moves() == 0 || t.board.get_last_move() == kNullMove) {
    return;
  }
  const Color color = t.board.get_turn();
  const Move move = t.board.get_last_move();
  Square opp_des = GetMoveDestination(move);
  PieceType opp_piecetype = GetPieceType(t.board.get_piece(opp_des));
  // TODO deal with promotion situations
  const int32_t score = std::min(depth * depth, 200);

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

template<NodeType node_type, int Mode>
Score AlphaBeta(Thread &t, Score alpha, const Score beta, Depth depth) {
  assert(alpha.is_valid());
  assert(beta.is_valid());
  assert(beta > alpha);
  assert(beta.value() == get_next_score(alpha).value() || node_type != NodeType::kNW);

  const Score original_alpha = alpha;

  //Immediately return 0 if we detect a draw.
  if (t.board.IsDraw() || (settings::kRepsForDraw == 3 && t.board.CountRepetitions(min_ply) >= 2)) {
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
    return QuiescentSearch<Mode>(t, alpha, beta);
  }

  // To avoid counting nodes twice if all we do is fall through to QSearch,
  // we wait until here to count this node.
  t.nodes++;

  //Transposition Table Probe
  table::Entry entry = table::GetEntry(t.board.get_hash());
  bool valid_entry = table::ValidateHash(entry,t.board.get_hash());
  if (node_type != NodeType::kPV && valid_entry
      && sufficient_bounds(t.board, entry, alpha, beta, depth) ) {
//    if (entry.get_best_move() != kNullMove && GetMoveType(entry.get_best_move()) < kCapture) {
//      update_counter_move_history(t, {{entry.get_best_move()}}, depth);
//    }
    return entry.get_score(t.board);
  }

  const bool in_check = t.board.InCheck();
  Score static_eval = alpha;
  t.set_static_score(kNoScore);
  bool strict_worsening = false;

  //Speculative pruning methods
  if (node_type == NodeType::kNW && beta.is_static_eval() && !in_check) {

    //Set static eval from board and TT entry.
    if (valid_entry) {
      if (entry.get_bound() == kExactBound) {
        static_eval = entry.get_score(t.board);
      }
      else {
        static_eval = net_evaluation::ScoreBoard(t.board);
        if ( (entry.get_bound() == kLowerBound && static_eval < entry.get_score(t.board))
            || (entry.get_bound() == kUpperBound && static_eval > entry.get_score(t.board)) ) {
          static_eval = entry.get_score(t.board);
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
      NScore margin = (kSNMPMargin - 60 * !strict_worsening) * depth;
      if (settings::kUseScoreBasedPruning && static_eval.value() > beta.value() + margin
          && t.board.get_phase() > 1 * piece_phases[kQueen]) {
        return beta;
      }
    }

    //Null Move Pruning
    if (static_eval >= beta && is_null_move_allowed(t.board, depth)) {
      t.set_move(kNullMove);
      t.board.Make(kNullMove);
      const Depth R = 3 + depth / 5;
      Score score = -AlphaBeta<NodeType::kNW, Mode>(t, -beta, -alpha,
                                    depth - R);
      t.board.UnMake();
      if (score >= beta) {
        return score;
      }
    }
  }

  // idea from http://talkchess.com/forum3/viewtopic.php?f=7&t=74769
  // ELO | 4.67 +- 3.56
  if (node_type == NodeType::kPV && depth >= 5 && !valid_entry) {
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

//  if (Mode == kSamplingSearchMode && node_type == NodeType::kNW && depth <= kMaxDepthSampled) {
//    sample_nodes++;
//    if (sample_nodes >= kNodeCountSampleAt && (!in_check || sample_nodes >= kNodeCountSampleAt + 100)) {
//      return sample_node_and_return_alpha(t.board, depth, node_type, alpha);
//    }
//  }

  Move tt_entry = kNullMove;
  if (valid_entry) {
    tt_entry = entry.get_best_move();
  }

  bool moves_sorted = false, swapped = false;
  if (tt_entry != kNullMove) {
    swapped = SwapToFront(moves, tt_entry);
    assert(!swapped || tt_entry == moves[0]);
  }
  if (!swapped) {
    SortMovesML(moves, t, tt_entry);
    moves_sorted = true;
  }

  Move best_local_move = tt_entry;
  if (node_type == NodeType::kPV && moves.size() == 1) {
    depth++;
  }

  //Init checking squares for efficient detection of checking moves.
  //Moves detected always give check, but only direct checks are detected.
  //Ie no checks from special moves or discovered checks.
  Vec<BitBoard, 6> checking_squares = t.board.GetDirectCheckingSquares();

  Score lower_bound_score = GetMatedOnMoveScore(t.board.get_num_made_moves());
  std::vector<Move> quiets;
  Score alpha_nw = alpha.get_next_score();
  //Move loop
  for (size_t i = 0; i < moves.size(); ++i) {
    if (i == 1 && !moves_sorted) {
      SortMovesML(moves, t, tt_entry);
      moves_sorted = true;
    }
    const Move move = moves[i];

    Depth e = 0;// Extensions
    if (i == 0 
        && depth >= settings::kSingularExtensionDepth-2
        && valid_entry
        && entry.depth >= std::max(depth, settings::kSingularExtensionDepth) - 3
        && !(node_type == NodeType::kPV && moves.size() == 1)
        && entry.get_bound() != kUpperBound
        && entry.get_score(t.board).is_static_eval()
        && get_singular_beta(entry.get_score(t.board), depth) > kMinStaticEval) {
      SortMovesML(moves, t, tt_entry);
      moves_sorted = true;
      auto is_singular = move_is_singular(t, depth, moves, entry);
      if (is_singular.first) {
        e = 1;
      }
      else if (is_singular.second >= beta) {
        return is_singular.second;
      }
    }

    Depth reduction = 0;

    if (i && !in_check && !(checking_squares[GetPieceType(t.board.get_piece(GetMoveSource(move)))]
                              & GetSquareBitBoard(GetMoveDestination(move)))) {
      //Late Move Pruning
      assert(depth > 0);
      if ((size_t)depth < kLMP[0].size() && (i >= (size_t)kLMP[node_type == NodeType::kPV][depth])
          && GetMoveType(move) < kEnPassant) {
        continue;
      }

      //Late Move Reduction factor
      reduction = get_lmr_reduction<node_type>(depth, i, GetMoveType(move) > kDoublePawnMove);
      assert(reduction < depth);

      //Futility Pruning
      if (node_type == NodeType::kNW && settings::kUseScoreBasedPruning
          && depth - reduction <= 3
          && static_eval.value() < (alpha.value() - get_futility_margin(depth - reduction, !strict_worsening))
          && GetMoveType(move) < kEnPassant) {
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
      score = -AlphaBeta<node_type, Mode>(t, -beta, -alpha, depth - 1 + e);
    }
    else {
      //Assume we have searched the best move already and search with closed window and possibly reduction
      score = -AlphaBeta<NodeType::kNW, Mode>(t, -alpha_nw, -alpha,
                                              depth - 1 + e - reduction);
      //Research with full depth if our initial search indicates an improvement over Alpha
      if (score > alpha && (node_type == NodeType::kPV || reduction > 0)) {
        score = -AlphaBeta<node_type, Mode>(t, -beta, -alpha, depth - 1 + e);
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
      if (GetMoveType(move) < kCapture) {
        update_counter_move_history(t, quiets, depth);
      }
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
      return score;
    }

    //In PV nodes we might be improving Alpha without breaking Beta
    if (score > alpha) {
      if (GetMoveType(move) < kCapture) {
        update_counter_move_history(t, quiets, depth);
      }
      //Update score and expected best move
      alpha = score;
      alpha_nw = get_next_score(alpha);
      lower_bound_score = score;
      best_local_move = move;
    }
    if (score > lower_bound_score) {
      lower_bound_score = score;
    }
  }
  if (node_type != NodeType::kNW && alpha > original_alpha) {
    assert(best_local_move != kNullMove);
    // We should save any best move which has improved alpha.
    table::SavePVEntry(t.board, best_local_move, lower_bound_score, depth);
  }
  else {
    table::SaveEntry(t.board, best_local_move, lower_bound_score, depth, kUpperBound);
  }

  return lower_bound_score;
}

std::pair<bool, Score> move_is_singular(Thread &t, const Depth depth,
                                       const std::vector<Move> &moves,
                                       const table::Entry &entry) {
  const Score beta = entry.get_score(t.board);
  const Score rBeta = get_singular_beta(beta, depth);
  const Score rAlpha = get_previous_score(rBeta);
  const Depth rDepth = (depth - 3) / 2;
  assert(beta.is_static_eval());
  assert(rBeta.is_static_eval());
  assert(rAlpha.is_static_eval());
  assert(entry.get_best_move() == moves[0]);
  assert(entry.get_bound() != kUpperBound);

  for(size_t i = 1; i < moves.size(); ++i) {
    t.set_move(moves[i]);
    t.board.Make(moves[i]);
    Score score = -AlphaBeta<NodeType::kNW, kNormalSearchMode>(t, -rBeta, -rAlpha, rDepth);
    t.board.UnMake();
    if (score >= rBeta) {
      return std::make_pair(false, score);
    }
  }
  return std::make_pair(true, rAlpha);
}

template<int Mode>
Score RootSearchLoop(Thread &t, Score original_alpha, const Score beta,
                     Depth current_depth, std::vector<Move> &moves) {
  assert(original_alpha.is_valid());
  assert(beta.is_valid());
  assert(original_alpha < beta);

  Score alpha = original_alpha;
  Score lower_bound_score = kMinScore;
  //const bool in_check = board.InCheck();
  if (settings::kRepsForDraw == 3 && alpha < draw_score[t.board.get_turn()].get_previous_score() && t.board.MoveInListCanRepeat(moves)) {
    if (beta <= draw_score[t.board.get_turn()]) {
      return draw_score[t.board.get_turn()];
    }
    alpha = draw_score[t.board.get_turn()].get_previous_score();
  }
  const bool in_check = t.board.InCheck();
  for (size_t i = 0; i < moves.size(); ++i) {
    t.set_move(moves[i]);
    t.board.Make(moves[i]);
    if (i == 0) {
      Score score = -AlphaBeta<NodeType::kPV, Mode>(t, -beta, -alpha, current_depth - 1);
      assert(score.is_valid());
      if (settings::kRepsForDraw == 3 && score < draw_score[t.board.get_turn()] && t.board.CountRepetitions() >= 2) {
        score = draw_score[t.board.get_turn()];
      }
      t.board.UnMake();
      if (score >= beta) {
        return score;
      }
      alpha = std::max(score, alpha);
      lower_bound_score = std::max(score, lower_bound_score);
    }
    else {
      Depth reduction = 0;
      if (!in_check && !t.board.InCheck() && i > 2) {
        //Late Move Reduction factor
        reduction = get_lmr_reduction<NodeType::kPV>(current_depth, i - 2, GetMoveType(moves[i]) > kDoublePawnMove);
//        if (GetMoveType(moves[i]) > kDoublePawnMove) {
//          reduction = (2 * reduction) / 3;
//        }
      }
      Score score = -AlphaBeta<NodeType::kNW, Mode>(t, -get_next_score(alpha),
                                                    -alpha, current_depth - 1 - reduction);
      if (score > alpha) {
        score = -AlphaBeta<NodeType::kPV, Mode>(t, -beta, -alpha, current_depth - 1);
      }
      if (settings::kRepsForDraw == 3 && score < draw_score[t.board.get_turn()] && t.board.CountRepetitions() >= 2) {
        score = draw_score[t.board.get_turn()];
      }
      lower_bound_score = std::max(score, lower_bound_score);
      t.board.UnMake();
      if (finished(t)) {
        Threads.end_search = true;
        return lower_bound_score;
      }
      if (score >= beta) {
        auto it = moves.rbegin() + moves.size() - i - 1;
        std::rotate(it, it + 1, moves.rend());
        return score;
      }
      else if (score > alpha) {
        alpha = score;
        auto it = moves.rbegin() + moves.size() - i - 1;
        std::rotate(it, it + 1, moves.rend());
      }
    }
  }
  if (t.id == 0) {
    table::SavePVEntry(t.board, moves[0], lower_bound_score, current_depth);
    //table::SavePVEntry(t.board, moves[0]);
  }
  return lower_bound_score;
}

template<int Mode>
inline Score PVS(Thread &t, Depth current_depth, const std::vector<Score> &previous_scores, std::vector<Move> &moves) {
  if (current_depth <= 4) {
    return RootSearchLoop<Mode>(t, kMinScore, kMaxScore, current_depth, moves);
  }
  else {
    Score score = previous_scores.back();
    Score delta = WDLScore{kInitialAspirationDelta, kInitialAspirationDelta};
    Score alpha = (score-delta).get_valid_score();
    Score beta = (score+delta).get_valid_score();
    SortMovesML(moves, t, moves[0]);
//    assert(is_valid_score(alpha));
//    assert(is_valid_score(beta));
    score = RootSearchLoop<Mode>(t, alpha, beta, current_depth, moves);
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
//      assert(is_valid_score(alpha));
//      assert(is_valid_score(beta));
      assert(score > alpha && score < beta);
      score = RootSearchLoop<Mode>(t, alpha, beta, current_depth, moves);
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

    if (rsearch_mode == kNormalSearchMode) {
      score = PVS<kNormalSearchMode>(*this, current_depth, previous_scores, moves);
    }
    else if (rsearch_mode == kSamplingSearchMode) {
      score = PVS<kSamplingSearchMode>(*this, current_depth, previous_scores, moves);
    }
    else {
      assert(rsearch_mode == kSamplingEvalMode);
      score = PVS<kSamplingEvalMode>(*this, current_depth, previous_scores, moves);
    }

    if(!finished(*this)) {
      last_search_score = score;
      previous_scores.emplace_back(score);
      if (id != 0) {
        continue;
      }
      Time end = now();

      PrintUCIInfoString(*this, current_depth, begin, end, score, moves[0]);

      auto time_used = std::chrono::duration_cast<Milliseconds>(end-begin);
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
  rsearch_mode = Mode;
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  assert(moves.size() != 0);
  if (moves.size() == 1 && !fixed_search_time) {
    return moves[0];
  }
  table::Entry entry = table::GetEntry(board.get_hash());
  Move tt_move = kNullMove;
  if (table::ValidateHash(entry,board.get_hash())) {
    tt_move = entry.get_best_move();
  }
  Threads.main_thread->board.SetToSamePosition(board);
  Threads.main_thread->root_height = board.get_num_made_moves();
  Threads.main_thread->max_depth = board.get_num_made_moves();
  SortMovesML(moves, *Threads.main_thread, tt_move);
  Threads.main_thread->moves = moves;
  Threads.end_search = false;
  std::vector<std::thread> helpers;
  for (Thread* t : Threads.helpers) {
    t->board.SetToSamePosition(board);
    t->root_height = board.get_num_made_moves();
    t->moves = moves;
    t->perturb_root_moves();
    while(rng() % 2) {
      t->perturb_root_moves();
    }
    t->max_depth = t->board.get_num_made_moves();
    helpers.emplace_back(std::thread(&Thread::search, t));
  }
  Threads.main_thread->search();
  Threads.end_search = true;
  for (size_t helper_idx = 0; helper_idx < Threads.helpers.size(); ++helper_idx) {
    helpers[helper_idx].join();
  }
  return Threads.main_thread->moves[0];
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
  Threads.clear_killers_and_countermoves();
}

// For move ordering we train a logistic regression classifier to predict whether a move will break beta.
// Alternatively predicting relative move orderings such as the probability of a move being greater or equal
// to all previous moves in the list or better than the previous move in the move ordering performed worse.

#ifdef SEARCH_TRAINING
void TrainSearchParams(bool from_scratch) {
  const int scaling = 128;
  set_print_info(false);
  std::vector<double> weights(kNumMoveProbabilityFeatures);
  std::vector<double> weights_in_check(kNumMoveProbabilityFeatures);
  if (!from_scratch) {
    LoadSearchVariables();
    for (size_t i = 0; i < kNumMoveProbabilityFeatures; ++i) {
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
      for (size_t i = 0; i < moves.size(); ++i) {
        features.emplace_back(GetMoveWeight<std::vector<int>, true>(moves[i], *Threads.main_thread, info));
      }
    }
    else {
      for (size_t i = 0; i < moves.size(); ++i) {
        features.emplace_back(GetMoveWeight<std::vector<int>, false>(moves[i], *Threads.main_thread, info));
      }
    }
    std::vector<Score> scores(features.size());
    size_t low = 0, high = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
      Threads.main_thread->board.Make(moves[i]);
      Score score = -AlphaBeta<NodeType::kNW, kNormalSearchMode>(*Threads.main_thread,
                                                                 -get_next_score(sampled_alpha),
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
    for (size_t i = 0; i < moves.size(); ++i) {
      if (GetMoveType(moves[i]) == kRookPromotion
          || GetMoveType(moves[i]) == kBishopPromotion) {
        continue;
      }
      Score score;
      sampled_board.Make(moves[i]);
      if (sampled_node_type == NodeType::kNW) {
        score = scores[i];
      }
      else {
        Threads.main_thread->board.SetToSamePosition(sampled_board);
        score = -AlphaBeta<NodeType::kPV, kNormalSearchMode>(*Threads.main_thread,
                                                             -get_next_score(sampled_alpha),
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
        for (size_t idx = 0; idx < kNumMoveProbabilityFeatures; ++idx) {
          final_score += features[i][idx] * weights_in_check[idx];
        }
        final_score /= scaling;
        double sigmoid = 1 / ( 1 + std::exp(-final_score) );
        double gradient = sigmoid - target;
        for (size_t idx = 0; idx < kNumMoveProbabilityFeatures; ++idx) {
          weights_in_check[idx] -= (2 * nu * gradient * features[i][idx]) / (1);
        }
      }
      else {
        for (size_t idx = 0; idx < kNumMoveProbabilityFeatures; ++idx) {
          final_score += features[i][idx] * weights[idx];
        }
        final_score /= scaling;
        double sigmoid = 1 / ( 1 + std::exp(-final_score) );
        double gradient = sigmoid - target;
        for (size_t idx = 0; idx < kNumMoveProbabilityFeatures; ++idx) {
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
      for (size_t idx = 0; idx < kNumMoveProbabilityFeatures; ++idx) {
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
  for (size_t i = 0; i < moves_and_scores.size(); ++i) {
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

size_t CountGreater(const Score score, const std::vector<SearchTrainHelper> &moves_and_scores) {
  size_t res = 0;
  for (size_t i = 0; i < moves_and_scores.size(); ++i) {
    if (moves_and_scores[i].score > score)
      res++;
  }
  return res;
}

size_t CountEqual(const Score score, const std::vector<SearchTrainHelper> &moves_and_scores) {
  size_t res = 0;
  for (size_t i = 0; i < moves_and_scores.size(); ++i) {
    if (moves_and_scores[i].score == score)
      res++;
  }
  return res-1;
}

void PrintDataset(const std::vector<SearchParamPositionSample> &samples, const std::string filename) {
  std::ofstream file(filename);
  //print header first
  file << "move, num_moves, better_moves, equal_moves, score, break_beta";
  for (size_t i = 0; i < samples[i].features.size(); ++i) {
    file << ", fe" << i;
  }
  file << std::endl;
  for (size_t i = 0; i < samples.size(); ++i) {
    file << samples[i].move << ", " << samples[i].num_moves << ", "
        << samples[i].better_moves << ", " << samples[i].equal_moves << ", "
        << samples[i].score << ", " << samples[i].break_beta;
    for (size_t j = 0; j < samples[i].features.size(); ++j) {
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
      for (size_t i = 0; i < moves.size(); ++i) {
        features.emplace_back(GetMoveWeight<std::vector<int>, true>(moves[i], *Threads.main_thread, info));
      }
    }
    else {
      for (size_t i = 0; i < moves.size(); ++i) {
        features.emplace_back(GetMoveWeight<std::vector<int>, false>(moves[i], *Threads.main_thread, info));
      }
    }
    std::vector<SearchTrainHelper> moves_and_scores;
    for (size_t i = 0; i < moves.size(); ++i) {
      if (GetMoveType(moves[i]) == kRookPromotion
          || GetMoveType(moves[i]) == kBishopPromotion) {
        continue;
      }
      sampled_board.Make(moves[i]);
      RootSearch<kNormalSearchMode>(sampled_board, 6, Milliseconds(100));
      Score score = -last_search_score;
//      Threads.main_thread->board.SetToSamePosition(sampled_board);
//      Score score = -AlphaBeta<NodeType::kPV, kNormalSearchMode>(*Threads.main_thread,
//                                                       kMinScore,
//                                                       kMaxScore,
//                                                       6);//sampled_depth - 1);
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
    for (size_t i = 0; i < moves_and_scores.size(); ++i) {
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
        PrintDataset(samples, "search_params/DSet4.csv");
      }
      if (!samples_in_check.empty()) {
        PrintDataset(samples_in_check, "search_params/DSetInCheck4.csv");
      }
    }
    if (sampled_positions > 40005) {
      break;
    }
  }
}
#endif

Score QSearch(Board &board) {
  Threads.main_thread->board.SetToSamePosition(board);
  return QuiescentSearch<kNormalSearchMode>((*Threads.main_thread), kMinScore, kMaxScore);
}

Board SampleEval(Board board) {
  evaluation_nodes = 0;
  RootSearch<kSamplingEvalMode>(board, 128, Milliseconds(5000));
  return sampled_board;
}

#ifdef SEARCH_TRAINING
void SaveSearchVariables() {
  std::ofstream file(settings::kSearchParamFile);
  std::ofstream description_file(settings::kSearchParamExplanationFile);
  size_t idx = 0;
  for (size_t i = 0; i < kNumMoveProbabilityFeatures; ++i) {
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
  for (size_t i = 0; i < kNumMoveProbabilityFeatures; ++i) {
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
  size_t idx = 0;
  file << "const std::array<int, " << kNumMoveProbabilityFeatures << "> search_params = {" << std::endl;
  for (size_t i = 0; i < kNumMoveProbabilityFeatures; ++i) {
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
  for (size_t i = 0; i < kNumMoveProbabilityFeatures; ++i) {
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
  for (size_t i = 0; i < kNumMoveProbabilityFeatures; ++i) {
    file >> search_weights[i];
  }
  file.close();

  std::ifstream fileic(settings::kSearchParamInCheckFile);
  for (size_t i = 0; i < kNumMoveProbabilityFeatures; ++i) {
    fileic >> search_weights_in_check[i];
  }
  fileic.close();
}
#endif

void LoadSearchVariablesHardCoded() {
  int c = 0;
  for (size_t i = 0; i < kNumMoveProbabilityFeatures; ++i) {
    search_weights[i] = hardcode::search_params[c];
    search_weights_in_check[i] = hardcode::search_params_in_check[c++];
  }
}

void EvaluateScoreDistributions(const int focus) {
  set_print_info(false);
  size_t count1 = 0, count2 = 0;
  std::vector<Game> games = data::LoadGames(1200000);
  const int max_depth = 3, n_score_bins = 160, score_bin_size = 100,
            n_dif_bins = 800, dif_bin_size = 16;
  std::vector<Array2d<long, n_score_bins, n_dif_bins> > histogram(max_depth);
  for (size_t i = 0; i < max_depth; ++i) {
    for (size_t j = 0; j < n_score_bins; ++j) {
      for (size_t k = 0; k < n_dif_bins; ++k) {
        histogram[i][j][k] = 0;
      }
    }
  }
  size_t id = 0;
  while (true) {
    id++;
    Game game = games[rng() % games.size()];
    size_t index = (rng() % (2 * game.moves.size() / 3)) + (game.moves.size() / 3) - 2;
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
    Score score = net_evaluation::ScoreBoard(sampled_board);
    NScore score_bin_idx = score.to_nscore();
    score_bin_idx += score_bin_size / 2;
    score_bin_idx /= score_bin_size;
    score_bin_idx += n_score_bins / 2;
    score_bin_idx = std::max(0, (int)score_bin_idx);
    score_bin_idx = std::min(n_score_bins - 1, (int)score_bin_idx);
    if (focus == 0) {
      for (Depth depth = 1; depth <= max_depth; ++depth) {
        DepthSearch(sampled_board, depth);
        Score ab_score = get_last_search_score();
        NScore dif = ab_score.to_nscore() - score.to_nscore();
        dif /= dif_bin_size;
        dif += n_dif_bins / 2;
        dif = std::max(0, (int)dif);
        dif = std::min(n_dif_bins - 1, (int)dif);
        histogram[depth - 1][score_bin_idx][dif]++;
      }
    }
    else if (focus == 1) {
      std::vector<Move> moves = sampled_board.GetMoves<kNonQuiescent>();
      for (Depth depth = 1; depth <= max_depth; ++depth) {
        for (Move move : moves) {
          if (GetMoveType(move) >= kEnPassant) {
            continue;
          }
          sampled_board.Make(move);
          if (sampled_board.InCheck()) {
            sampled_board.UnMake();
            continue;
          }
          NScore ab_score = 0;
          if (depth == 1) {
            ab_score = -QSearch(sampled_board).to_nscore();
          }
          else {
            DepthSearch(sampled_board, depth - 1);
            ab_score = -get_last_search_score().to_nscore();
          }
          sampled_board.UnMake();

          NScore dif = ab_score - score.to_nscore();
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
      for (Depth depth = 1; depth <= max_depth; ++depth) {
        NScore max_dif = kMinScore.to_nscore();
        NScore max_forcing_dif = kMinScore.to_nscore();
        for (Move move : moves) {
          bool forcing = false;
          if (GetMoveType(move) >= kEnPassant) {
            forcing = true;
          }
          sampled_board.Make(move);
          if (sampled_board.InCheck()) {
            forcing = true;
          }
          NScore ab_score = 0;
          if (depth == 1) {
            ab_score = -QSearch(sampled_board).to_nscore();
          }
          else {
            DepthSearch(sampled_board, depth - 1);
            ab_score = -get_last_search_score().to_nscore();
          }
          sampled_board.UnMake();

          if (forcing) {
            max_forcing_dif = std::max(ab_score - score.to_nscore(), max_forcing_dif);
          }
          else {
            max_dif = std::max(ab_score - score.to_nscore(), max_dif);

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
      for (size_t i = 0; i < n_score_bins; ++i) {
        vhistogram[i][0] = (i - (n_score_bins / 2)) * score_bin_size;
      }
      for (Depth depth = 0; depth < max_depth; ++depth) {
        for (size_t i = 0; i < n_score_bins; ++i) {
          for (size_t j = 0; j < n_dif_bins; ++j) {
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
  for (size_t i = 0; i < games.size(); ++i) {
    Game game = games[i];
    game.set_to_position_after(0);
    while (game.board.get_num_made_moves() < game.moves.size()) {
      const Move move = game.moves[game.board.get_num_made_moves()];
      if (GetMoveType(move) == kCapture
          && !game.board.GivesCheck(move)) {
        PieceType target = game.board.get_piece(GetMoveDestination(move));
        Score before = net_evaluation::ScoreBoard(game.board);
        game.forward();
        Score after = -net_evaluation::ScoreBoard(game.board);
        move_scores[GetPieceType(target)].emplace_back(after - before);
      }
      else if (GetMoveType(move) == kEnPassant
          && !game.board.GivesCheck(move)) {
        Score before = net_evaluation::ScoreBoard(game.board);
        game.forward();
        Score after = -net_evaluation::ScoreBoard(game.board);
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
  for (int i = 0; i < 6; ++i) {
    std::sort(move_scores[i].begin(), move_scores[i].end());
  }
  for (int j = 0; j < 6; ++j) {
    for (int i = 1; i < n; ++i) {
      std::cout << i << ":" << move_scores[j][move_scores[j].size() * i / (n)].to_nscore() << " ";
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
  for (size_t i = 0; i < games.size(); ++i) {
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

void SetFutilityMargin(int32_t score) {
  kFutileMargin = init_futility_margins(score);
}

void SetSNMPMargin(int32_t score) {
  kSNMPMargin = score;
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

#endif

//void PrintSample() {
//  for (size_t i = 0; i < 45; ++i) {
//    double_t d = fail_high[i] / all[i];
//    size_t count = 100;
//    for (size_t j = 0; j < 45; ++j) {
//      if (d > fail_high[j] / all[j]) {
//        count++;
//      }
//    }
//    std::cout << count << ", ";
//  }
//  std::cout << std::endl;
//}

}

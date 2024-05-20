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
 * transposition.cc
 *
 *  Created on: Dec 10, 2016
 *      Author: Jonathan Rosenthal
 */

#include "transposition.h"
#include "net_evaluation.h"
#include <cassert>
#include <optional>

namespace {

Score score_to_tt_score(const Score score, const int32_t num_made_moves) {
  if (score.is_mate_score()) {
    if (score.is_disadvantage()) {
      return WDLScore{score.win - num_made_moves, score.win_draw - num_made_moves};
    }
    return WDLScore{score.win + num_made_moves, score.win_draw + num_made_moves};
  }
  return score;
}

Score tt_score_to_score(Score score, int32_t num_made_moves) {
  if (score.is_mate_score()) {
    if (score.is_disadvantage()) {
      return WDLScore{score.win + num_made_moves, score.win_draw + num_made_moves};
    }
    return WDLScore{score.win - num_made_moves, score.win_draw - num_made_moves};
  }
  return score;
}

}

namespace table {

// In addition to the main table, a second, smaller table is used for improved PV entry redundancy.
// PV entries are stored in both and TT size is sum of size of main table and PV table.
size_t size = 1600000;
std::vector<Entry> _table(size);

uint8_t current_generation = 0;

void UpdateGeneration() {
  current_generation += (0x1 << 2);
}

void SetTableSize(const int32_t MB_total_int) {
  const size_t MB_total = static_cast<size_t>(MB_total_int);
  const size_t bytes = MB_total << 20;
  size = bytes >> 4;
  size -= size % 4;

  _table.resize(size);
}

size_t HashFunction(const HashType hash) {
  return (hash % size) & ~(0x3);
}

bool ValidateHash(const Entry &entry, const HashType hash){
  HashType best_move_cast = entry.get_best_move();
  return entry.hash == (hash ^ best_move_cast);
}

std::optional<Entry> GetMainEntryIdx(const HashType hash, const size_t idx) {
  for (size_t i = 0; i < 3; ++i) {
    if (ValidateHash(_table[idx+i], hash)) {
      return _table[idx+i];
    }
  }
  return {};
}

std::optional<Entry> GetEntry(const HashType hash) {
  size_t idx = HashFunction(hash);
  std::optional<Entry> entry = GetMainEntryIdx(hash, idx);
  Entry entry_pv = _table.at(idx+3);

  if (!ValidateHash(entry_pv, hash)) {
    return entry;
  }
  if (!entry.has_value()) {
    return entry_pv;
  }
  if (entry->depth > entry_pv.depth) {
    return entry;
  }
  return entry_pv;
}

size_t GetIdxToReplace(const HashType hash, size_t idx) {
  if (ValidateHash(_table[idx], hash)) {
    return idx;
  }
  int worst_score = 1024 + static_cast<int>(_table[idx].depth)
      - 512 * (_table[idx].get_generation() != current_generation);
  size_t worst_idx = idx;

  for (size_t i = 1; i <= 2; ++i) {
    if (ValidateHash(_table[idx+i], hash)) {
      return idx + i;
    }
    int score = 1024 + static_cast<int>(_table[idx+i].depth)
          - 512 * (_table[idx+i].get_generation() != current_generation);
    if (score < worst_score) {
      worst_score = score;
      worst_idx = idx + i;
    }
  }
  return worst_idx;
}

void SaveEntry(const Board &board, const Move best_move, const Score score,
               const Depth depth, const uint8_t bound) {
  HashType hash = board.get_hash();
  size_t idx = HashFunction(hash);
  size_t index = GetIdxToReplace(hash, idx);

  assert(index < _table.size());
  assert(score.is_valid());

  HashType best_move_cast = best_move;
  hash ^= best_move_cast;
  
  if (_table[index].hash != hash || _table[index].depth <= depth + 2) {
    Entry entry;
    entry.hash = hash;
    entry.set_score(score, board);
    entry.set_best_move(best_move);
    entry.set_gen_and_bound(bound);
    assert(entry.get_generation() == current_generation);
    entry.depth = depth;
    _table[index] = entry;
  }
}

void SavePVEntry(const Board &board, const Move best_move, const Score score, const Depth depth) {
  HashType hash = board.get_hash();
  size_t idx = HashFunction(hash);
  size_t index = GetIdxToReplace(hash, idx); // HashFunction(hash);
  size_t index_pv = idx + 3;

  assert(index < _table.size());
  assert(index_pv < _table.size());

  HashType best_move_cast = best_move;
  Entry entry;
  entry.hash = hash ^ best_move_cast;
  entry.set_score(score, board);
  entry.set_best_move(best_move);
  entry.depth = depth;
  entry.set_gen_and_bound(kExactBound);
  _table[index] = entry;
  _table[index_pv] = entry;
}

void ClearTable() {
  for (size_t i = 0; i < _table.size(); i++) {
    _table[i].hash = 0;
    _table[i].set_best_move(kNullMove);
  }
}

void Entry::set_score(const Score score_new, const Board &board) {
  Score score = score_to_tt_score(score_new, board.get_num_made_moves());
  win = score.win;
  win_draw = score.win_draw;
}

Score Entry::get_score(const Board &board) const {
  Score score = WDLScore{win, win_draw};
  return tt_score_to_score(score, board.get_num_made_moves());
}

void Entry::set_gen() {
  gen_and_bound = current_generation | get_bound();
}

void Entry::set_gen_and_bound(uint8_t bound) {
  gen_and_bound = current_generation | bound;
}

size_t GetHashfull() {
  size_t result = 0;
  for (size_t i = 0; i < 1000; ++i) {
    result += (_table[4*i].get_generation() == current_generation);
  }
  return result;
}

}


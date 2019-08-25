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
#include <cassert>

namespace {

bool is_mate_score(const Score score) {
  return (score < kMinScore + 2000) || (score > kMaxScore - 2000);
}

Score score_to_tt_score(const Score score, const size_t num_made_moves) {
  if (is_mate_score(score)) {
    if (score < 0) {
      return score - num_made_moves;
    }
    return score + num_made_moves;
  }
  return score;
}

Score tt_score_to_score(Score score, size_t num_made_moves) {
  if (is_mate_score(score)) {
    if (score < 0) {
      return score + num_made_moves;
    }
    return score - num_made_moves;
  }
  return score;
}

}

namespace table {

// In addition to the main table, a second, smaller table is used for improved PV entry redundancy.
// PV entries are stored in both and TT size is sum of size of main table and PV table.
size_t size = 1600000;
size_t size_pvt = 10001;
std::vector<Entry> table(size);
std::vector<Entry> table_pv(size_pvt);

uint8_t current_generation = 0;

void UpdateGeneration() {
  current_generation += (0x1 << 2);
}

void SetTableSize(const size_t MB) {
  size = (6 * (MB << 16)) / 7;
  size_pvt = size / 6;

  size -= size % 4;

  table.resize(size);
  table_pv.resize(size_pvt);
}

size_t HashFunction(const HashType hash) {
  return (hash % size) & ~(0x3);
}

size_t PVHashFunction(const HashType hash) {
  return hash % size_pvt;
}

Entry GetMainEntryIdx(const HashType hash) {
  size_t idx = HashFunction(hash);
  for (size_t i = 0; i < 3; ++i) {
    if (ValidateHash(table[idx+i], hash)) {
      return table[idx+i];
    }
  }
  return table[idx+3];
}

Entry GetEntry(const HashType hash) {
  Entry entry = GetMainEntryIdx(hash);
  Entry entry_pv = table_pv.at(PVHashFunction(hash));

  if (!ValidateHash(entry, hash)) {
    return entry_pv;
  }
  if (!ValidateHash(entry_pv, hash)) {
    return entry;
  }
  if (entry.depth > entry_pv.depth) {
    return entry;
  }
  return entry_pv;
}

size_t GetIdxToReplace(HashType hash) {
  size_t idx = HashFunction(hash);
  if (ValidateHash(table[idx], hash)) {
    return idx;
  }
  int worst_score = 1024 + static_cast<int>(table[idx].depth)
      - 512 * (table[idx].get_generation() != current_generation);
  size_t worst_idx = idx;

  for (size_t i = 1; i <= 3; ++i) {
    if (ValidateHash(table[idx+i], hash)) {
      return idx + i;
    }
    int score = 1024 + static_cast<int>(table[idx+i].depth)
          - 512 * (table[idx+i].get_generation() != current_generation);
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
  size_t index = GetIdxToReplace(hash); // HashFunction(hash);

  assert(index < table.size());

  HashType best_move_cast = best_move;
  Entry entry;
  entry.hash = hash ^ best_move_cast;
  entry.set_score(score, board);
  entry.set_best_move(best_move);
  entry.set_gen_and_bound(bound);
  entry.depth = depth;
  table[index] = entry;
}

void SavePVEntry(const Board &board, const Move best_move, const Score score, const Depth depth) {
  HashType hash = board.get_hash();
  size_t index = GetIdxToReplace(hash); // HashFunction(hash);
  size_t index_pv = PVHashFunction(hash);

  assert(index < table.size());

  HashType best_move_cast = best_move;
  Entry entry;
  entry.hash = hash ^ best_move_cast;
  entry.set_score(score, board);
  entry.set_best_move(best_move);
  entry.depth = depth;
  entry.set_gen_and_bound(kExactBound);
  table[index] = entry;
  table_pv[index_pv] = entry;
}

bool ValidateHash(const Entry &entry, const HashType hash){
  HashType best_move_cast = entry.get_best_move();
  return entry.hash == (hash ^ best_move_cast);
}

void ClearTable() {
  for (size_t i = 0; i < table.size(); i++) {
    table[i].hash = 0;
    table[i].set_best_move(kNullMove);
  }
  for (size_t i = 0; i < table_pv.size(); i++) {
    table_pv[i].hash = 0;
    table_pv[i].set_best_move(kNullMove);
  }
}

void Entry::set_score(const Score score_new, const Board &board) {
  score = score_to_tt_score(score_new, board.get_num_made_moves());
}

Score Entry::get_score(const Board &board) const {
  return tt_score_to_score(score, board.get_num_made_moves());
}

void Entry::set_gen() {
  gen_and_bound = current_generation | get_bound();
}

void Entry::set_gen_and_bound(uint8_t bound) {
  gen_and_bound = current_generation | bound;
}

}


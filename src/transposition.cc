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
 *      Author: Jonathan
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

int size = 1600000;
//int size_pvt = 10001;
std::vector<DualEntryHolder> table(size);
//std::vector<PVEntry> table_pv(size_pvt);

void SetTableSize(const long MB) {
//  size = (MB << 20) / 21;
  //size_pvt = size / 12;
  size = (MB << 15);
  table.resize(size);
  //table_pv.resize(size_pvt);
}

int HashFunction(const HashType hash) {
  return hash % size;
}

//int PVHashFunction(const HashType hash) {
//  return hash % size_pvt;
//}

Entry GetEntry(const HashType hash) {
  DualEntryHolder entries = table.at(HashFunction(hash));
  if (!ValidateHash(entries.nw_entry, hash)) {
    return entries.pv_entry;
  }
  if (!ValidateHash(entries.pv_entry, hash)) {
    return entries.nw_entry;
  }
  if (entries.nw_entry.depth > entries.pv_entry.depth) {
    return entries.nw_entry;
  }
  return entries.pv_entry;
}

//PVEntry GetPVEntry(const HashType hash) {
//  PVEntry entry = table_pv.at(PVHashFunction(hash));
//  return entry;
//}

void SaveEntry(const Board &board, const Move best_move, const Score score, const Depth depth) {
  HashType hash = board.get_hash();
  int index = HashFunction(hash);

  assert(index >= 0 && index < table.size());

  HashType best_move_cast = best_move;
  Entry entry;
  entry.hash = hash ^ best_move_cast;
  entry.set_score(score, board);
  entry.set_best_move(best_move);
  entry.depth = depth;
  entry.bound = kLowerBound;
  table[index].nw_entry = entry;

//  table[index].hash = hash ^ best_move_cast;
//  table[index].best_move = best_move;
//  table[index].set_score(score, board);
//  table[index].bound = bound;
//  table[index].depth = depth;
}

void SavePVEntry(const Board &board, const Move best_move, const Score score, const Depth depth) {
  HashType hash = board.get_hash();
  int index = HashFunction(hash);

  assert(index >= 0 && index < table.size());

  HashType best_move_cast = best_move;
  Entry entry;
  entry.hash = hash ^ best_move_cast;
  entry.set_score(score, board);
  entry.set_best_move(best_move);
  entry.depth = depth;
  entry.bound = kExactBound;
  table[index].pv_entry = entry;
}

//void SavePVEntry(const Board &board, const Move best_move) {
//  HashType hash = board.get_hash();
//  int index = PVHashFunction(hash);
//  assert(index >= 0 && index < table_pv.size());
//  HashType best_move_cast = best_move;
//  table_pv[index].hash = hash ^ best_move_cast;
//  table_pv[index].best_move = best_move;
//}

bool ValidateHash(const Entry &entry, const HashType hash){
  HashType best_move_cast = entry.get_best_move();
  return entry.hash == (hash ^ best_move_cast);
}

//bool ValidateHash(const PVEntry &entry, const HashType hash){
//  HashType best_move_cast = entry.best_move;
//  return entry.hash == (hash ^ best_move_cast);
//}


void ClearTable() {
  for (unsigned int i = 0; i < table.size(); i++) {
    table[i].nw_entry.hash = 0;
    table[i].nw_entry.set_best_move(kNullMove);
    table[i].pv_entry.hash = 0;
    table[i].pv_entry.set_best_move(kNullMove);
  }
//  for (unsigned int i = 0; i < table_pv.size(); i++) {
//    table_pv[i].hash = 0;
//    table_pv[i].best_move = 0;
//  }
}

void Entry::set_score(const Score score_new, const Board &board) {
  score = score_to_tt_score(score_new, board.get_num_made_moves());
}

Score Entry::get_score(const Board &board) const {
  return tt_score_to_score(score, board.get_num_made_moves());
}


}


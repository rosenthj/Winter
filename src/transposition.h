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
 * transposition.h
 *
 *  Created on: Dec 10, 2016
 *      Author: Jonathan
 */

#ifndef TRANSPOSITION_H_
#define TRANSPOSITION_H_

#include "general/types.h"
#include "board.h"

namespace table {

struct Entry {
  Score get_score(const Board &board) const;
  void set_score(const Score new_score, const Board &board);
  Move get_best_move() const {
    return best_move;
  }
  void set_best_move(const Move move) {
    best_move = move;
  }
  HashType hash;      // 8
  //int32_t bound;
  //Depth depth;
//  int64_t bound; // Temporary hack to see what happens with possible false sharing.
private:
  Score score;        // 4
  uint16_t best_move; // 2
public:
  uint8_t depth;      // 1
  uint8_t bound;      // 1
};

struct DualEntryHolder {
  Entry nw_entry;
  Entry pv_entry;
};


//struct PVEntry {
//  HashType hash;
//  int64_t best_move; //Temporary hack to see what happens with possible false sharing.
//  //Move best_move;
//};

void SetTableSize(const long MB);
Entry GetEntry(const HashType hash);
void SaveEntry(const Board &board, const Move best_move, const Score score, const Depth depth);
void SavePVEntry(const Board &board, const Move best_move, const Score score, const Depth depth);
bool ValidateHash(const Entry &entry, const HashType hash);
//PVEntry GetPVEntry(const HashType hash);
//void SavePVEntry(const Board &board, const Move best_move);
//bool ValidateHash(const PVEntry &entry, const HashType hash);
void ClearTable();

}

#endif /* TRANSPOSITION_H_ */

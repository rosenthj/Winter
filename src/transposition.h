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
  HashType hash;  // 8
  //int32_t bound;  // 4
  //Depth depth;    // 4
  int64_t bound;  // 8 //Temporary hack to see what happens with possible false sharing.
  int64_t depth;  // 8
  Move best_move; // 4
private:
  Score score;    // 4
};

struct CacheLineEntryHolder {
  Entry entries[3];

};

struct PVEntry {
  HashType hash;
  int64_t best_move; //Temporary hack to see what happens with possible false sharing.
  //Move best_move;
};

void SetTableSize(const long MB);
Entry GetEntry(const HashType hash);
void SaveEntry(const Board &board, const Move best_move, const Score score, const int bound,
    const Depth depth);
bool ValidateHash(const Entry &entry, const HashType hash);
PVEntry GetPVEntry(const HashType hash);
void SavePVEntry(const Board &board, const Move best_move);
bool ValidateHash(const PVEntry &entry, const HashType hash);
void ClearTable();

}

#endif /* TRANSPOSITION_H_ */

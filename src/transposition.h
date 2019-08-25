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
 *      Author: Jonathan Rosenthal
 */

#ifndef TRANSPOSITION_H_
#define TRANSPOSITION_H_

#include "general/types.h"
#include "board.h"

namespace table {

struct Entry {
  // Score getter and setter is necessary for mate score handling
  Score get_score(const Board &board) const;
  void set_score(const Score new_score, const Board &board);

  Move get_best_move() const {
    return best_move;
  }
  void set_best_move(const Move move) {
    best_move = move;
  }
  uint8_t get_generation() const {
    return gen_and_bound & 0xfc; // Upper 6 bits are for generation
  }
  /// Sets entry generation to current generation of TT
  void set_gen();

  uint8_t get_bound() const {
    return gen_and_bound & 0x3;
  }
  void set_bound(uint8_t bound) {
    gen_and_bound = get_generation() | bound;
  }

  void set_gen_and_bound(uint8_t bound);

  HashType hash;         // 8 bytes
private:
  Score score;           // 4 bytes
  uint16_t best_move;    // 2 bytes
  uint8_t gen_and_bound; // 1 byte
public:
  uint8_t depth;         // 1 byte
                         // 16 bytes total, 4 entries per cache line.
};

void SetTableSize(const size_t MB);
Entry GetEntry(const HashType hash);
void SaveEntry(const Board &board, const Move best_move, const Score score,
               const Depth depth, const uint8_t bound = kLowerBound);
void SavePVEntry(const Board &board, const Move best_move, const Score score, const Depth depth);
bool ValidateHash(const Entry &entry, const HashType hash);

void UpdateGeneration();
void ClearTable();

}

#endif /* TRANSPOSITION_H_ */

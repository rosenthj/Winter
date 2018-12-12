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
 * bookkeeping.h
 *
 *  Created on: May 23, 2017
 *      Author: Jonathan Rosenthal
 */

#ifndef SRC_GENERAL_BOOKKEEPING_H_
#define SRC_GENERAL_BOOKKEEPING_H_

#include "settings.h"
#include "../board.h"
#include <iostream>
#include <vector>

namespace bookkeeping {

enum Trigger {
  kFailHigh, kImproveAlpha, kLessEqualAlpha, kReturnAlpha
};

struct InfoContainer {
  int NodeType;
  Move tt_entry;
  Depth depth;
  Depth min_ply;
  int move_number;
  int expected_node;
  Trigger trigger;
};

void inc_counter(int counter, int index);
void print_counters();
void print_relative_counters();
void reset_counters();

inline void Inc(int counter, int index) {
  if (settings::bookkeeping_active) {
    inc_counter(counter, index);
  }
}

void log_info(const Board &board, const InfoContainer &info);

}



#endif /* SRC_GENERAL_BOOKKEEPING_H_ */

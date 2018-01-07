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
 *      Author: Jonathan
 */

#ifndef SRC_GENERAL_BOOKKEEPING_H_
#define SRC_GENERAL_BOOKKEEPING_H_

#include "settings.h"

namespace bookkeeping {

void IncrementNWD1NodesToFailHigh(int num_searched_nodes);
void PrintNWD1NodeRequirements();
void ResetNWD1NodeRequirements();

template<int counter_idx>
inline void Inc(int arg = -1) {
  if (settings::bookkeeping_active) {
    if (counter_idx == 0) {
      return IncrementNWD1NodesToFailHigh(arg);
    }
  }
}

}



#endif /* SRC_GENERAL_BOOKKEEPING_H_ */

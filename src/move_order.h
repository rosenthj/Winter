/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2016 Jonas Kuratli, Jonathan Maurer, Jonathan Rosenthal
 *  Copyright (C) 2017-2024 Jonathan Rosenthal
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
 * move_order.h
 *
 * The search namespace is intended to handle all tree search operations.
 * Specifically our program should implement perft, timed alpha-beta and depth
 * limited alpha-beta search algorithms.
 *
 *  Created on: May 27, 2024
 *      Author: Jonathan Rosenthal
 */

#ifndef MOVE_ORDER_H_
#define MOVE_ORDER_H_

#include "general/types.h"
#include "search_thread.h"

namespace move_order {

void Init();
void Sort(std::vector<Move> &moves, search::Thread &t, const Move best_move);
void SortML(std::vector<Move> &moves, search::Thread &t,
            const Move best_move = kNullMove, const size_t start_idx = 0);

#ifdef TUNE_ORDER

void SetWeight(size_t idx, MoveScore value);
MoveScore GetWeight(size_t idx);
void PrintOptions();
void OptionsToFile();
void GenerateCPP();

#endif /* TUNE_ORDER */

}

#endif /* MOVE_ORDER_H_ */

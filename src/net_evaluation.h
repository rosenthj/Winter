/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2019 Jonathan Rosenthal
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
 * net_evaluation.h
 *
 * The evaluation source and header files with associated namespace is used
 * in order to score positions so the search function can differentiate how
 * good different positions are
 *
 *  Created on: June 26, 2019
 *      Author: Jonathan Rosenthal
 */

#ifndef NET_EVALUATION_H_
#define NET_EVALUATION_H_

#include "general/types.h"
#include "general/settings.h"
#include "board.h"
#include <vector>

namespace net_evaluation {

Score ScoreBoard(const Board &board);
// Returns the input features for the net for a specific board position.
// In the future this may become more complicated, depending on how pieces get encoded.
std::vector<int32_t> GetNetInputs(const Board &board);
void init_weights();

void SetContempt(Color color, int32_t value);
std::array<Score, 2> GetDrawArray();

Score AddContempt(Score score, Color color);
Score RemoveContempt(Score score, Color color);
}

#endif /* NET_EVALUATION_H_ */

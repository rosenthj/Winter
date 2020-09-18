/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2016 Jonas Kuratli, Jonathan Maurer, Jonathan Rosenthal
 *  Copyright (C) 2017-2020 Jonathan Rosenthal
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
 * magic.h
 *
 *  Created on: Sep 17, 2020
 *      Author: Jonathan Rosenthal
 */

#ifndef EGTB_H_
#define EGTB_H_

#include <string>
#include "general/types.h"
#include "board.h"

namespace egtb {

WDLScore ProbeWDL(const Board &board);
std::vector<Move> ProbOptimalMoves(Board &board);

void InitTB(std::string path);
int32_t MaxPieces();

}

#endif

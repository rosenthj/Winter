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
 * search.h
 *
 * The search namespace is intended to handle all tree search operations.
 * Specifically our program should implement perft, timed alpha-beta and depth
 * limited alpha-beta search algorithms.
 *
 *  Created on: Nov 1, 2016
 *      Author: Jonathan Rosenthal
 */

#ifndef SEARCH_H_
#define SEARCH_H_

#include "board.h"
#include "general/types.h"

namespace search {

size_t Perft(Board &board, Depth depth);
Move DepthSearch(Board board, Depth depth);
Move TimeSearch(Board board, Milliseconds time);
Move FixedTimeSearch(Board board, Milliseconds time);
Move NodeSearch(Board board, size_t num_nodes);
Board SampleEval(Board board);
Score QSearch(Board &board);
Score SQSearch(Board &board);
Score get_last_search_score();
size_t get_num_nodes();
void set_print_info(bool print_info);
void end_search();
Board get_sampled_board();
std::vector<Move> GetSortedMovesML(Board &board);

void clear_killers_and_counter_moves();

void SetArmageddon(bool armageddon);
void SetContempt(int32_t contempt);
void SetUCIShowWDL(bool show_wdl);

#ifdef TUNE
#define OPTION(x) \
void Set##x(int32_t value); \
int32_t Get##x();

OPTION(kInitialAspirationDelta)
OPTION(kSNMPScaling)
OPTION(kSNMPOffset)
OPTION(kSNMPImproving)
OPTION(kFutilityScaling)
OPTION(kFutilityOffset)
OPTION(kFutilityImproving)
OPTION(kLMROffset)
OPTION(kLMRMult)
OPTION(kLMROffsetPV)
OPTION(kLMRMultPV)
OPTION(kLMROffsetCap)
OPTION(kLMRMultCap)
OPTION(kLMROffsetPVCap)
//OPTION(kLMRMultiplierPVCap)

OPTION(kLMPBaseNW)
OPTION(kLMPBasePV)
OPTION(kLMPScalar)
OPTION(kLMPQuad)
OPTION(kLMPImpBaseNW)
OPTION(kLMPImpBasePV)
OPTION(kLMPImpScalar)
OPTION(kLMPImpQuad)

//OPTION(kLMPDepth)

OPTION(kNMPBase)
OPTION(kNMPScale)
OPTION(kSingularExtensionDepth)
#undef OPTION
#endif

}

#endif /* SEARCH_H_ */

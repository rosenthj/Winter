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

#ifdef SEARCH_TRAINING
void CreateSearchParamDataset(bool from_scratch);
void CreateSearchParamDataset();
void TrainSearchParams(bool from_scratch);
void SaveSearchVariables();
void LoadSearchVariables();
void SaveHardcodeSearchVariables();
#endif
void LoadSearchVariablesHardCoded();

void SetArmageddon(bool armageddon);
void SetContempt(int32_t contempt);
void SetUCIShowWDL(bool show_wdl);

#ifdef TUNE
void SetInitialAspirationDelta(int32_t delta);
void SetSNMPScaling(int32_t score);
void SetSNMPOffset(int32_t score);
void SetSNMPImproving(int32_t score);
void SetFutilityScaling(int32_t score);
void SetFutilityOffset(int32_t score);
void SetFutilityImproving(int32_t score);
void SetLMROffset(int32_t value);
void SetLMRMultiplier(int32_t value);
void SetLMROffsetPV(int32_t value);
void SetLMRMultiplierPV(int32_t value);
void SetLMROffsetCap(int32_t value);
void SetLMRMultiplierCap(int32_t value);
void SetLMROffsetPVCap(int32_t value);
//void SetLMRMultiplierPVCap(int32_t value);
void SetLMPBaseNW(int32_t value);
void SetLMPBasePV(int32_t value);
void SetLMPScalar(int32_t value);
void SetLMPQuadratic(int32_t value);
void SetLMPDepth(int32_t value);
void SetSingularExtensionDepth(int32_t depth);
#endif

}

#endif /* SEARCH_H_ */

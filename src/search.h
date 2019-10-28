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
void set_print_info(bool print_info);
void end_search();
Board get_sampled_board();
std::vector<Move> GetSortedMovesML(Board &board);

std::vector<Board> GenerateEvalSampleSet(std::string filename);

void clear_killers_and_counter_moves();

void CreateSearchParamDataset(bool from_scratch);
void CreateSearchParamDataset();
void TrainSearchParams(bool from_scratch);
void SaveSearchVariables();
void LoadSearchVariables();
void SaveHardcodeSearchVariables();
void LoadSearchVariablesHardCoded();

void EvaluateCaptureMoveValue(int n);
void EvaluateScoreDistributions(const int focus);

void SetContempt(int contempt);
void SetArmageddon(bool armageddon);

#ifdef TUNE
void SetInitialAspirationDelta(Score delta);
void SetFutilityMargin(Score score);
void SetSNMPMargin(Score score);
void SetLMRDiv(double div);
#endif

}

#endif /* SEARCH_H_ */

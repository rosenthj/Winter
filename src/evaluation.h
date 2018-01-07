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
 * evaluation.h
 *
 * The evaluation source and header files with associated namespace is used
 * in order to score positions so the search function can differentiate how
 * good different positions are
 *
 *  Created on: Nov 1, 2016
 *      Author: Jonathan
 */

#ifndef EVALUATION_H_
#define EVALUATION_H_

#include "general/types.h"
#include "general/settings.h"
#include "board.h"

namespace evaluation {

Score ScoreBoard(const Board &board);
void PrintFeatureValues(const Board &board);
void SaveGMMVariables();
void LoadGMMVariables();
void Train(bool from_scratch);
int ScoreToCentipawn(const Score score, const Board &board);
Score GetPawnBaseValue(const Board &board);
Score GetTempoValue(const Board &board);
void RunEMForGMM();
void SampledEMForGMM(int iterations = 500);
void CheckFeatureMagnitude();
void CheckVariableInfluence();
double BoardProbability(const Board &board);
Vec<double, settings::kGMMk> BoardMixtureProbability(const Board &board);
Score EvaluateQuietMoveValue();
void SGDDrawMarginTrain();

}

namespace evaluation_features {

const int kUseQueenActivity = false;

}

#endif /* EVALUATION_H_ */

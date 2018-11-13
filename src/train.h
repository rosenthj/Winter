/*
 * train.h
 *
 *  Created on: Nov 15, 2018
 *      Author: jonathan
 */

#ifndef SRC_TRAIN_H_
#define SRC_TRAIN_H_

#include "evaluation.h"
#include "general/settings.h"
#include "general/types.h"

namespace train {

//void SaveGMMVariables();
//void LoadGMMVariables();
//void LoadGMMVariablesHardCoded();
void Train(bool from_scratch);
////int ScoreToCentipawn(const Score score, const Board &board);
////Score GetPawnBaseValue(const Board &board);
////Score GetTempoValue(const Board &board);
void RunEMForGMM();
void SampledEMForGMM(int iterations = 500);
void RunEMForNFCM();
////void CheckFeatureMagnitude();
////void CheckVariableInfluence();
////Vec<double, settings::kGMMk> BoardMixtureProbability(const Board &board);
////Score EvaluateQuietMoveValue();

//std::vector<PScore> GetEvaluationWeights();

}

#endif /* SRC_TRAIN_H_ */

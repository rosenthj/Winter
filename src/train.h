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

void Train(bool from_scratch);
void RunEMForGMM();
void SampledEMForGMM(int iterations = 500);
void RunEMForNFCM();

}

#endif /* SRC_TRAIN_H_ */

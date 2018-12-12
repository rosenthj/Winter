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
 * train.h
 *
 *  Created on: Nov 15, 2018
 *      Author: Jonathan Rosenthal
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

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
 * test.h
 *
 *  Created on: Dec 25, 2016
 *      Author: Jonathan Rosenthal
 */

#ifndef BENCHMARK_H_
#define BENCHMARK_H_

#include "general/types.h"

namespace benchmark {

double MoveOrderTest();
int TimeToDepthSuite();
double EntropyLossTimedSuite(Milliseconds time_per_position);
double EntropyLossNodeSuite(size_t nodes_per_position);
void PerftSuite();
void SymmetrySuite();
void GenerateDatasetFromEPD();
double ZuriChessDatasetLoss();

}

#endif /* BENCHMARK_H_ */

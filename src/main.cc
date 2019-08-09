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

#include <iostream>

#include "benchmark.h"
#include "transposition.h"
#include "uci.h"
#include "general/types.h"
#include "learning/linear_algebra.h"
#include "general/magic.h"
#include "general/settings.h"
#include "general/debug.h"
#include "general/bookkeeping.h"
#include "board.h"
#include "search.h"
#include "learning/cluster.h"

#include "net_evaluation.h"

int main() {
  debug::EnterFunction(debug::kMain, "Main", "");
  table::SetTableSize(32);

  search::LoadSearchVariablesHardCoded();
  net_evaluation::init_weights();

//  train::RunEMForNFCM();
//  evaluation::RunEMForGMM();

//  benchmark::GenerateDatasetFromEPD();

//  net_evaluation::GenerateDatasetFromEPD();
//  benchmark::ZuriChessDatasetLoss();
//  net_evaluation::EstimateFeatureImpact();
//  std::cout << "Startpos eval: " << net_evaluation::ScoreBoard(Board()) << std::endl;

  //search::SaveHardcodeSearchVariables();

  //The following commented lines remain to remind me how to do stuff =)
  //search::CreateSearchParamDataset(false);
  //if (settings::kExperimental)
  //  search::TrainSearchParams(false);
  //search::CreateSearchParamDataset();
  //evaluation::CheckFeatureMagnitude();
  //evaluation::CheckVariableInfluence();
  //benchmark::EntropyLossTimedSuite(Milliseconds(10));
  //bookkeeping::PrintNWD1NodeRequirements();
  //evaluation::SampledEMForGMM();
  //evaluation::EvaluateQuietMoveValue();
  //search::EvaluateScoreDistributions(2);
  //search::EvaluateCaptureMoveValue(1000);
  //benchmark::TimeToDepthSuite();
  //parse::Save2dVecToCSV<Score>(vecvec,"data/test.csv");
  uci::Loop();
  debug::ExitFunction(debug::kMain);
  return 0;
}

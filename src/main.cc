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

#include "evaluation.h"
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
#include "train.h"
#include "learning/cluster.h"

int main() {
  debug::EnterFunction(debug::kMain, "Main", "");
  table::SetTableSize(32);
  if (settings::kTrainFromScratch) {
    train::Train(true);
  }
//  evaluation::LoadGMMVariables();
  evaluation::LoadGMMVariablesHardCoded();
//  evaluation::LoadMixturesHardCoded();
//  evaluation::LoadVariablesFromFile();
//  evaluation::SaveGMMHardCode("gmm_hardcoded.txt");
//  evaluation::SaveGMMVariablesHardCode("eval_weights_hardcoded.txt");
  search::LoadSearchVariablesHardCoded();
//  train::RunEMForNFCM();
//  evaluation::RunEMForGMM();

//  benchmark::GenerateDatasetFromEPD();

//  for (Square src = 0; src < 64; src++) {
//    for (Square des = 0; des < 64; des++) {
//      for (MoveType mt = kNormalMove; mt <= kQueenPromotion; mt++) {
//        Move move = GetMove(src, des, mt);
//        table::Entry entry;
//        entry.set_best_move(move);
//        if (move != entry.get_best_move()) {
//          std::cout << "Found error! " << move << std::endl;
//        }
//      }
//    }
//  }
//  std::cout << "Finished looking for mistakes!";
  //search::SaveHardcodeSearchVariables();
  //evaluation::SaveGMMHardCode("gmm_hardcode.txt");
  //evaluation::SaveGMMVariablesHardCode("gmm_score_weights_hardcode.txt");
  //The following commented lines remain to remind me how to do stuff =)
  //if (settings::kExperimental)
  //  evaluation::Train(false);
  //search::CreateSearchParamDataset(false);
  //if (settings::kExperimental)
  //  search::TrainSearchParams(false);
  //search::CreateSearchParamDataset();
  //evaluation::SGDDrawMarginTrain();
  //evaluation::CheckFeatureMagnitude();
  //evaluation::CheckVariableInfluence();
  //benchmark::EntropyLossTimedSuite(Milliseconds(10));
  //bookkeeping::PrintNWD1NodeRequirements();
  //evaluation::RunKMeans();
  //evaluation::SampledEMForGMM();
  //evaluation::EvaluateQuietMoveValue();
  //search::EvaluateScoreDistributions(2);
  //search::EvaluateCaptureMoveValue(1000);
  //benchmark::TimeToDepthSuite();
  //evaluation::SaveTScoreVariables("t_variables.txt");
  //evaluation::Train();
  //std::cout << "bla\n";
  //std::vector<std::vector<Score> > vecvec(5, std::vector<Score>(5));
  //parse::Save2dVecToCSV<Score>(vecvec,"data/test.csv");

  uci::Loop();
  debug::ExitFunction(debug::kMain);
  return 0;
}

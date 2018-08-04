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
 * settings.h
 *
 *  Created on: Nov 4, 2016
 *      Author: Jonathan
 */

#ifndef GENERAL_SETTINGS_H_
#define GENERAL_SETTINGS_H_

#include "types.h"
#include <string>

namespace settings {

#ifdef EXPERIMENT
constexpr bool kExperimental = true;
#else
constexpr bool kExperimental = false;
#endif

const std::string engine_name = "Winter";
const std::string engine_version =
                   kExperimental ? "Beta 180723 Experimental"
                                 : "Beta 0.2 RC";
const std::string engine_author = "Jonathan Rosenthal";

const bool kUseGMM = true;
const int kGMMk = kExperimental ? 4 : 4;

const std::string kParamPath = "params/";
const std::string kSampledMixtureFile = kParamPath + "v2Smixtures" + std::to_string(kGMMk) + ".txt";

const std::string kMixtureFile =
                 kExperimental ? kParamPath + "mixtures" + std::to_string(kGMMk) + ".txt"
                               : kParamPath + "mixtures" + std::to_string(kGMMk) + ".txt";
const std::string kGMMParamsFile =
                   kExperimental ? kParamPath + "gmm_params" + std::to_string(kGMMk) + ".txt"
                                 : kParamPath + "gmm_params" + std::to_string(kGMMk) + ".txt";

const std::string kParamExplanationFile = kGMMParamsFile + ".info";

const std::string kSearchParamPath = "search_params/";
const std::string kSearchParamFile =
                     kExperimental ? kSearchParamPath + "sparams.txt"
                                   : kSearchParamPath + "sparams.txt";
const std::string kSearchParamExplanationFile = kSearchParamFile + ".info";

const std::string kCCRLPath = "data/CCRL.ucig";
const std::string kCEGTPath = "data/CEGT.ucig";

const bool kTrainGMMFromScratch = kExperimental ? false : false;
const bool kTrainFromScratch = kExperimental ? false : false;

constexpr bool bookkeeping_active = false;

const bool kUseExtensions = true;
const bool kUseScoreBasedPruning = true;
const bool kUseNullMoves = true && kUseExtensions;
const Depth R = 2;
const Depth kMaxDepth = 128;

}

#endif /* GENERAL_SETTINGS_H_ */

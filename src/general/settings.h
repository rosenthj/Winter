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
 *      Author: Jonathan Rosenthal
 */

#ifndef GENERAL_SETTINGS_H_
#define GENERAL_SETTINGS_H_

#include "types.h"
#include <string>

namespace settings {

const std::string engine_name = "Winter";
const std::string engine_version = "0.7.4";
const std::string engine_author = "Jonathan Rosenthal";

#if defined(__BMI2__)
const std::string compile_arch = "BMI2";
#elif defined(__AVX__)
const std::string compile_arch = "AVX";
#elif defined(__SSE4_2__)
const std::string compile_arch = "SSE4.2";
#elif defined(__SSE4_1__)
const std::string compile_arch = "SSE4.1";
#else
const std::string compile_arch = "";
#endif

const int kNumClusters = 4;

const std::string kParamPath = "params/";
const std::string kSampledMixtureFile = kParamPath + "v2Smixtures" + std::to_string(kNumClusters) + ".txt";

const std::string kMixtureFile = kParamPath + "mixtures_noisy" + std::to_string(kNumClusters) + ".txt";
const std::string kGMMParamsFile = kParamPath + "gmm_params_nfcm" + std::to_string(kNumClusters) + ".txt";

const std::string kParamExplanationFile = kGMMParamsFile + ".info";

const std::string kSearchParamPath = "search_params/";
const std::string kSearchParamVersionExtension = "190516";

const std::string kSearchParamFile = kSearchParamPath + "sparams"
    + kSearchParamVersionExtension + ".txt";
const std::string kSearchParamExplanationFile = kSearchParamFile + ".info";

const std::string kSearchParamInCheckFile = kSearchParamPath + "sparams_in_check"
    + kSearchParamVersionExtension + ".txt";
const std::string kSearchParamInCheckExplanationFile = kSearchParamInCheckFile + ".info";

const std::string kCCRLPath = "data/CCRL.ucig";
const std::string kCEGTPath = "data/CEGT.ucig";

const bool kTrainGMMFromScratch = false;
const bool kTrainFromScratch = false;

constexpr bool bookkeeping_active = false;

const bool kUseExtensions = true;
const bool kUseScoreBasedPruning = true;
const bool kUseNullMoves = true && kUseExtensions;
const Depth kRepsForDraw = 2;
const Depth kMaxDepth = 128;
const Depth kSingularExtensionDepth = 9;

const bool kUseQS = true;

}

#endif /* GENERAL_SETTINGS_H_ */

/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2016 Jonas Kuratli, Jonathan Maurer, Jonathan Rosenthal
 *  Copyright (C) 2017-2024 Jonathan Rosenthal
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
const std::string engine_version = "3.20";

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

constexpr bool kUseExtensions = true;
constexpr bool kUseScoreBasedPruning = true;
constexpr Depth kRepsForDraw = 2;
constexpr Depth kMaxDepth = 128;

constexpr bool kUseQS = true;

}

#endif /* GENERAL_SETTINGS_H_ */

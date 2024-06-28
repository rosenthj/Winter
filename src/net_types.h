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
 * linear_algebra.h
 *
 *  Created on: June 28, 2024
 *      Author: Jonathan Rosenthal
 */

#ifndef NET_TYPES_H_
#define NET_TYPES_H_

#include "general/types.h"
#include "linear_algebra.h"

// NN types
constexpr size_t block_size = 32;
using NetLayerType = Vec<float_t, block_size>;

constexpr size_t full_block_size = 128;
using FullLayerType = Vec<float_t, full_block_size>;

struct NetPieceModule {
  PieceType pt;
  Square sq;
  NetLayerType features;
};

struct PartialEvaluation {
  std::vector<NetPieceModule> pieces;
  FullLayerType global_features;
};

#endif /* NET_TYPES_H_*/

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
constexpr size_t block_size = 2 * 32;                   // Piece relation feature size
using NetLayerType = Vec<int16_t, block_size>;          // Piece relation accumulator type
using NetOutputType = Vec<int32_t, block_size / 2>;     // Output accumulator type

constexpr size_t full_block_size = 2 * 64;              // Full layer feature size
using FullLayerType = Vec<int16_t, full_block_size>;    // Full layer type

struct NetPieceModule {
  NetLayerType features;
  PieceType pt;
  Square sq;
};

struct PieceList {
  static constexpr size_t kMaxPieces = 32;

  void push_back(const NetPieceModule &npm) { data[count++] = npm; }
  void emplace_back(const NetPieceModule &npm) { data[count++] = npm; }
  void clear() { count = 0; }
  size_t size() const { return count; }

  NetPieceModule& operator[](size_t i) { return data[i]; }
  const NetPieceModule& operator[](size_t i) const { return data[i]; }

  NetPieceModule* begin() { return data.data(); }
  NetPieceModule* end() { return data.data() + count; }
  const NetPieceModule* begin() const { return data.data(); }
  const NetPieceModule* end() const { return data.data() + count; }

  std::array<NetPieceModule, kMaxPieces> data;
  size_t count = 0;
};

struct PartialEvaluation {
  PieceList pieces;
  FullLayerType global_features;
};

#endif /* NET_TYPES_H_*/

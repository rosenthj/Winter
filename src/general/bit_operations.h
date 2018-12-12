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
 * bit_operations.h
 *
 *  Created on: Nov 4, 2016
 *      Author: Jonathan Rosenthal
 */

#ifndef GENERAL_BIT_OPERATIONS_H_
#define GENERAL_BIT_OPERATIONS_H_

#include "types.h"
#include "parse.h"

enum Directions {
  kNorth, kNorthEast, kEast, kSouthEast, kSouth, kSouthWest, kWest, kNorthWest
};

namespace bitops {

const BitBoard a_file = parse::StringToBitBoard("a1") | parse::StringToBitBoard("a2")
                      | parse::StringToBitBoard("a3") | parse::StringToBitBoard("a4")
                      | parse::StringToBitBoard("a5") | parse::StringToBitBoard("a6")
                      | parse::StringToBitBoard("a7") | parse::StringToBitBoard("a8");

const BitBoard h_file = parse::StringToBitBoard("h1")
                      | parse::StringToBitBoard("h2") | parse::StringToBitBoard("h3")
                      | parse::StringToBitBoard("h4") | parse::StringToBitBoard("h5")
                      | parse::StringToBitBoard("h6") | parse::StringToBitBoard("h7")
                      | parse::StringToBitBoard("h8");

const BitBoard rook_file = a_file | h_file;
const BitBoard not_a_file = ~a_file;
const BitBoard not_h_file = ~h_file;

const BitBoard light_squares_first_two_ranks =  parse::StringToBitBoard("b1") | parse::StringToBitBoard("d1")
                              | parse::StringToBitBoard("f1") | parse::StringToBitBoard("h1")
                              | parse::StringToBitBoard("a2") | parse::StringToBitBoard("c2")
                              | parse::StringToBitBoard("e2") | parse::StringToBitBoard("g2");

const BitBoard light_squares = light_squares_first_two_ranks
                             | (light_squares_first_two_ranks << (2 * 8))
                             | (light_squares_first_two_ranks << (4 * 8))
                             | (light_squares_first_two_ranks << (6 * 8));

const BitBoard dark_squares = ~light_squares;

inline BitBoard N(const BitBoard bb) { return bb << 8; }
inline BitBoard S(const BitBoard bb) { return bb >> 8; }
inline BitBoard E(const BitBoard bb) { return (bb << 1) & not_a_file; }
inline BitBoard W(const BitBoard bb) { return (bb >> 1) & not_h_file; }
inline BitBoard NE(const BitBoard bb) { return (bb << 9) & not_a_file; }
inline BitBoard NW(const BitBoard bb) { return (bb << 7) & not_h_file; }
inline BitBoard SE(const BitBoard bb) { return (bb >> 7) & not_a_file; }
inline BitBoard SW(const BitBoard bb) { return (bb >> 9) & not_h_file; }

template<int Direction>
inline BitBoard Dir(const BitBoard bb) {
  switch (Direction) {
    case kNorth: return N(bb);
    case kNorthEast: return NE(bb);
    case kEast: return E(bb);
    case kSouthEast: return SE(bb);
    case kSouth: return S(bb);
    case kSouthWest: return SW(bb);
    case kWest: return W(bb);
    case kNorthWest: return NW(bb);
    default: return bb;
  }
}

inline BitBoard FillNorth(BitBoard bb, BitBoard empty) {
  bb |= empty & (bb << 8);
  empty &= empty << 8;
  bb |= empty & (bb << 16);
  empty &= empty << 16;
  bb |= empty & (bb << 32);
  return bb;
}

inline BitBoard FillSouth(BitBoard bb, BitBoard empty) {
  bb |= (empty & (bb >> 8));
  empty &= (empty >> 8);
  bb |= (empty & (bb >> 16));
  empty &= (empty >> 16);
  bb |= empty & (bb >> 32);
  return bb;
}

inline BitBoard FillEast(BitBoard bb, BitBoard empty) {
  empty &= not_a_file;
  bb |= empty & (bb << 1);
  empty &= empty << 1;
  bb |= empty & (bb << 2);
  empty &= empty << 2;
  bb |= empty & (bb << 4);
  return bb;
}

inline BitBoard FillWest(BitBoard bb, BitBoard empty) {
  empty &= not_h_file;
  bb |= empty & (bb >> 1);
  empty &= empty >> 1;
  bb |= empty & (bb >> 2);
  empty &= empty >> 2;
  bb |= empty & (bb >> 4);
  return bb;
}

inline BitBoard FillNorthWest(BitBoard bb, BitBoard empty) {
  empty &= not_h_file;
  bb |= empty & (bb << 7);
  empty &= empty << 7;
  bb |= empty & (bb << 14);
  empty &= empty << 14;
  bb |= empty & (bb << 28);
  return bb;
}

inline BitBoard FillNorthEast(BitBoard bb, BitBoard empty) {
  empty &= not_a_file;
  bb |= empty & (bb << 9);
  empty &= empty << 9;
  bb |= empty & (bb << 18);
  empty &= empty << 18;
  bb |= empty & (bb << 36);
  return bb;
}

inline BitBoard FillSouthWest(BitBoard bb, BitBoard empty) {
  empty &= not_h_file;
  bb |= empty & (bb >> 9);
  empty &= empty >> 9;
  bb |= empty & (bb >> 18);
  empty &= empty >> 18;
  bb |= empty & (bb >> 36);
  return bb;
}

inline BitBoard FillSouthEast(BitBoard bb, BitBoard empty) {
  empty &= not_a_file;
  bb |= empty & (bb >> 7);
  empty &= empty >> 7;
  bb |= empty & (bb >> 14);
  empty &= empty >> 14;
  bb |= empty & (bb >> 28);
  return bb;
}

// return the number of trailing zeros
inline int NumberOfTrailingZeros(const BitBoard x) {
  //return _tzcnt_u64(x);
  return __builtin_ctzll(x);
}
// return the leading number of zeros
inline int NumberOfLeadingZeros(const BitBoard x) {
  //return _lzcnt_u64(x);
  return __builtin_clzll(x);
}

// return number of set 1 bits
inline int PopCount(const BitBoard x) {
  //return _mm_popcnt_u64(x);
  return __builtin_popcountll(x);
}

// return the least significant 1 bit
inline BitBoard GetLSB(BitBoard x) {
  return x & -x;
}

// remove the least significant 1 bit from the param
inline void PopLSB(BitBoard &x) {
  x &= x-1;
}

}

#endif /* GENERAL_BIT_OPERATIONS_H_ */

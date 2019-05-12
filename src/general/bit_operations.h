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

constexpr BitBoard a1_bitboard = 0x1L;
constexpr BitBoard b1_bitboard = a1_bitboard << 1;
constexpr BitBoard c1_bitboard = a1_bitboard << 2;
constexpr BitBoard d1_bitboard = a1_bitboard << 3;
constexpr BitBoard e1_bitboard = a1_bitboard << 4;
constexpr BitBoard f1_bitboard = a1_bitboard << 5;
constexpr BitBoard g1_bitboard = a1_bitboard << 6;
constexpr BitBoard h1_bitboard = a1_bitboard << 7;

constexpr BitBoard a_file = a1_bitboard            | a1_bitboard << (1 * 8)
                          | a1_bitboard << (2 * 8) | a1_bitboard << (3 * 8)
                          | a1_bitboard << (4 * 8) | a1_bitboard << (5 * 8)
                          | a1_bitboard << (6 * 8) | a1_bitboard << (7 * 8);

constexpr BitBoard b_file = a_file << 1;
constexpr BitBoard c_file = a_file << 2;
constexpr BitBoard d_file = a_file << 3;
constexpr BitBoard e_file = a_file << 4;
constexpr BitBoard f_file = a_file << 5;
constexpr BitBoard g_file = a_file << 6;
constexpr BitBoard h_file = a_file << 7;

constexpr BitBoard rook_file = a_file | h_file;
constexpr BitBoard not_a_file = ~a_file;
constexpr BitBoard not_h_file = ~h_file;

constexpr BitBoard first_rank = a1_bitboard      | a1_bitboard << 1
                              | a1_bitboard << 2 | a1_bitboard << 3
                              | a1_bitboard << 4 | a1_bitboard << 5
                              | a1_bitboard << 6 | a1_bitboard << 7;

constexpr BitBoard second_rank  = first_rank << (1 * 8);
constexpr BitBoard third_rank   = first_rank << (2 * 8);
constexpr BitBoard fourth_rank  = first_rank << (3 * 8);
constexpr BitBoard fifth_rank   = first_rank << (4 * 8);
constexpr BitBoard sixth_rank   = first_rank << (5 * 8);
constexpr BitBoard seventh_rank = first_rank << (6 * 8);
constexpr BitBoard eigth_rank   = first_rank << (7 * 8);

constexpr BitBoard odd_ranks = first_rank | third_rank | fifth_rank | seventh_rank;
constexpr BitBoard even_ranks = ~odd_ranks;

constexpr BitBoard odd_files = a_file | c_file | e_file | g_file;
constexpr BitBoard even_files = ~odd_files;

constexpr BitBoard dark_squares = (odd_ranks & odd_files) | (even_ranks & even_files);
constexpr BitBoard light_squares = ~dark_squares;

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

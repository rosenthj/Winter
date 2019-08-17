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
 * magic.cc
 *
 *  Created on: Dec 16, 2016
 *      Author: Jonathan Rosenthal
 */

#include "magic.h"
#include "bit_operations.h"
#include "types.h"
#include <immintrin.h>
#include <cassert>
#include <array>


namespace {

constexpr std::array<BitBoard, 64> bishopMask = {
    0x40201008040200L, 0x402010080400L,   0x4020100a00L,     0x40221400L,
    0x2442800L,        0x204085000L,      0x20408102000L,    0x2040810204000L,
    0x20100804020000L, 0x40201008040000L, 0x4020100a0000L,   0x4022140000L,
    0x244280000L,      0x20408500000L,    0x2040810200000L,  0x4081020400000L,
    0x10080402000200L, 0x20100804000400L, 0x4020100a000a00L, 0x402214001400L,
    0x24428002800L,    0x2040850005000L,  0x4081020002000L,  0x8102040004000L,
    0x8040200020400L,  0x10080400040800L, 0x20100a000a1000L, 0x40221400142200L,
    0x2442800284400L,  0x4085000500800L,  0x8102000201000L,  0x10204000402000L,
    0x4020002040800L,  0x8040004081000L,  0x100a000a102000L, 0x22140014224000L,
    0x44280028440200L, 0x8500050080400L,  0x10200020100800L, 0x20400040201000L,
    0x2000204081000L,  0x4000408102000L,  0xa000a10204000L,  0x14001422400000L,
    0x28002844020000L, 0x50005008040200L, 0x20002010080400L, 0x40004020100800L,
    0x20408102000L,    0x40810204000L,    0xa1020400000L,    0x142240000000L,
    0x284402000000L,   0x500804020000L,   0x201008040200L,   0x402010080400L,
    0x2040810204000L,  0x4081020400000L,  0xa102040000000L,  0x14224000000000L,
    0x28440200000000L, 0x50080402000000L, 0x20100804020000L, 0x40201008040200L
};

constexpr std::array<BitBoard, 64> rookMask = {
    0x101010101017eL,    0x202020202027cL,    0x404040404047aL,    0x8080808080876L,
    0x1010101010106eL,   0x2020202020205eL,   0x4040404040403eL,   0x8080808080807eL,
    0x1010101017e00L,    0x2020202027c00L,    0x4040404047a00L,    0x8080808087600L,
    0x10101010106e00L,   0x20202020205e00L,   0x40404040403e00L,   0x80808080807e00L,
    0x10101017e0100L,    0x20202027c0200L,    0x40404047a0400L,    0x8080808760800L,
    0x101010106e1000L,   0x202020205e2000L,   0x404040403e4000L,   0x808080807e8000L,
    0x101017e010100L,    0x202027c020200L,    0x404047a040400L,    0x8080876080800L,
    0x1010106e101000L,   0x2020205e202000L,   0x4040403e404000L,   0x8080807e808000L,
    0x1017e01010100L,    0x2027c02020200L,    0x4047a04040400L,    0x8087608080800L,
    0x10106e10101000L,   0x20205e20202000L,   0x40403e40404000L,   0x80807e80808000L,
    0x17e0101010100L,    0x27c0202020200L,    0x47a0404040400L,    0x8760808080800L,
    0x106e1010101000L,   0x205e2020202000L,   0x403e4040404000L,   0x807e8080808000L,
    0x7e010101010100L,   0x7c020202020200L,   0x7a040404040400L,   0x76080808080800L,
    0x6e101010101000L,   0x5e202020202000L,   0x3e404040404000L,   0x7e808080808000L,
    0x7e01010101010100L, 0x7c02020202020200L, 0x7a04040404040400L, 0x7608080808080800L,
    0x6e10101010101000L, 0x5e20202020202000L, 0x3e40404040404000L, 0x7e80808080808000L
};

#ifndef __BMI2__
const BitBoard bishopMagicNumber[] = {
    0x10020800408200L, 0x8080800802000L, 0x4040082000000L, 0x4040080000000L, 0x2021000000000L, 0x901008000000L, 0x880808040000L, 0x1002082201000L,
    0x81001020400L, 0x80204040020L, 0x40802004000L, 0x40400800000L, 0x20210000000L, 0x10402400000L, 0x8084104000L, 0x20082011000L,
    0x10000410100100L, 0x8000210040080L, 0x4000800440008L, 0x800802004000L, 0x1000820080000L, 0x2000022012000L, 0x2000100900400L, 0x2000101010100L,
    0x20040020080200L, 0x8200008020080L, 0x8020004040010L, 0x10040000401020L, 0x840000802000L, 0x2020000209000L, 0x2040000440200L, 0x4010000410080L,
    0x4100400400400L, 0x2100400020800L, 0x805000010400L, 0x20080080080L, 0x4040400001100L, 0x20080020004400L, 0x1020080040400L, 0x1004100020100L,
    0x882010000800L, 0x1010802002000L, 0x804040400800L, 0x4200800800L, 0x80100400400L, 0x2100202000020L, 0x20020082000100L, 0x10040040800040L,
    0x840120100000L, 0x820090040000L, 0x10080900000L, 0x42020000L, 0x1002020000L, 0x40810010000L, 0x8081004004000L, 0x10040084004000L,
    0x802410020800L, 0x20080841000L, 0x201008800L, 0x420200L, 0x10020200L, 0x1020010100L, 0x80208020400L, 0x2021004008080L,
};

const BitBoard rookMagicNumber[] = {
    0x80004000201080L, 0x40100020004000L, 0x80088020001000L, 0x80041000080080L, 0x80028008000400L, 0x100010008020400L, 0x80010002000080L, 0x80010000402080L,
    0x800020400080L, 0x401000200040L, 0x802000100080L, 0x800800801000L, 0x800800040080L, 0x808002000400L, 0x1000100020004L, 0x800080004100L,
    0x20008080004000L, 0x10004020004000L, 0x808010002000L, 0x808010000800L, 0x808008000400L, 0x808004000200L, 0x808001000200L, 0x20000408104L,
    0x800080204000L, 0x400080802000L, 0x100080802000L, 0x100080080080L, 0x40080080080L, 0x20080800400L, 0x20080800100L, 0x800080004100L,
    0x80004000402000L, 0x401000402000L, 0x100080802000L, 0x800800801000L, 0x800800800400L, 0x40080800200L, 0x800200800100L, 0x800040800100L,
    0x400080008020L, 0x500020004000L, 0x100020008080L, 0x100008008080L, 0x80004008080L, 0x40002008080L, 0x1000200010004L, 0x4084020001L,
    0x800040002080L, 0x804000200080L, 0x801000200080L, 0x80010008080L, 0x40080080080L, 0x800200040080L, 0x1000200040100L, 0x800041000080L,
    0x201041008001L, 0x801021004001L, 0x410020000811L, 0x100004210009L, 0x2001020040802L, 0x1000400080201L, 0x1000200008401L, 0x1000080220041L,
};
#endif

constexpr std::array<char, 64> bishopShiftBits = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

constexpr std::array<char, 64> rookShiftBits = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

constexpr std::array<BitBoard, 64> knightAttackMap = {
    0x20400,            0x50800,            0xa1100,            0x142200,
    0x284400,           0x508800,           0xa01000,           0x402000,
    0x2040004,          0x5080008,          0xa110011,          0x14220022,
    0x28440044,         0x50880088,         0xa0100010,         0x40200020,
    0x204000402,        0x508000805,        0xa1100110a,        0x1422002214,
    0x2844004428,       0x5088008850,       0xa0100010a0,       0x4020002040,
    0x20400040200,      0x50800080500,      0xa1100110a00,      0x142200221400,
    0x284400442800,     0x508800885000,     0xa0100010a000,     0x402000204000,
    0x2040004020000,    0x5080008050000,    0xa1100110a0000,    0x14220022140000,
    0x28440044280000,   0x50880088500000,   0xa0100010a00000,   0x40200020400000,
    0x204000402000000,  0x508000805000000,  0xa1100110a000000,  0x1422002214000000,
    0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
    0x400040200000000,  0x800080500000000,  0x1100110a00000000, 0x2200221400000000,
    0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000,
    0x4020000000000,    0x8050000000000,    0x110a0000000000,   0x22140000000000,
    0x44280000000000,   0x88500000000000,   0x10a00000000000,   0x20400000000000
};

constexpr std::array<BitBoard, 64> kingAttackMap = {
    0x302,              0x705,              0xe0a,              0x1c14,
    0x3828,             0x7050,             0xe0a0,             0xc040,
    0x30203,            0x70507,            0xe0a0e,            0x1c141c,
    0x382838,           0x705070,           0xe0a0e0,           0xc040c0,
    0x3020300,          0x7050700,          0xe0a0e00,          0x1c141c00,
    0x38283800,         0x70507000,         0xe0a0e000,         0xc040c000,
    0x302030000,        0x705070000,        0xe0a0e0000,        0x1c141c0000,
    0x3828380000,       0x7050700000,       0xe0a0e00000,       0xc040c00000,
    0x30203000000,      0x70507000000,      0xe0a0e000000,      0x1c141c000000,
    0x382838000000,     0x705070000000,     0xe0a0e0000000,     0xc040c0000000,
    0x3020300000000,    0x7050700000000,    0xe0a0e00000000,    0x1c141c00000000,
    0x38283800000000,   0x70507000000000,   0xe0a0e000000000,   0xc040c000000000,
    0x302030000000000,  0x705070000000000,  0xe0a0e0000000000,  0x1c141c0000000000,
    0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000,
    0x203000000000000,  0x507000000000000,  0xa0e000000000000,  0x141c000000000000,
    0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000
};


const std::array<std::array<int, 64>, 64> initDistMap() {
  std::array<std::array<int, 64>, 64> distanceMap;
    for (Square a = 0; a < 64; a++) {
      for (Square b = 0; b < 64; b++) {
        distanceMap[a][b] = std::max(std::abs(GetSquareX(a)-GetSquareX(b)),
                                     std::abs(GetSquareY(a)-GetSquareY(b)));
      }
    }
    return distanceMap;
}

const std::array<std::array<BitBoard, 64>, 64> generateAttackVectorMaps() {
  std::array<std::array<BitBoard, 64>, 64> attackVectorMap;
    BitBoard src = 1;
    Square i = 0;
    while (src) {
        BitBoard des = 1;
        Square j = 0;
        while (des) {
            if (des & bitops::N(bitops::FillNorth(src, ~des)))
              attackVectorMap[i][j] = bitops::N(bitops::FillNorth(src, ~des));
            else if (des & bitops::NE(bitops::FillNorthEast(src, ~des)))
              attackVectorMap[i][j] = bitops::NE(bitops::FillNorthEast(src, ~des));
            else if (des & bitops::E(bitops::FillEast(src, ~des)))
              attackVectorMap[i][j] = bitops::E(bitops::FillEast(src, ~des));
            else if (des & bitops::SE(bitops::FillSouthEast(src, ~des)))
              attackVectorMap[i][j] = bitops::SE(bitops::FillSouthEast(src, ~des));
            else if (des & bitops::S(bitops::FillSouth(src, ~des)))
              attackVectorMap[i][j] = bitops::S(bitops::FillSouth(src, ~des));
            else if (des & bitops::SW(bitops::FillSouthWest(src, ~des)))
              attackVectorMap[i][j] = bitops::SW(bitops::FillSouthWest(src, ~des));
            else if (des & bitops::W(bitops::FillWest(src, ~des)))
              attackVectorMap[i][j] = bitops::W(bitops::FillWest(src, ~des));
            else if (des & bitops::NW(bitops::FillNorthWest(src, ~des)))
              attackVectorMap[i][j] = bitops::NW(bitops::FillNorthWest(src, ~des));
            des <<= 1;
            j++;
        }
        src <<= 1;
        i++;
    }
    return attackVectorMap;
}

BitBoard getConfigurationOfEmpty(BitBoard mask, int configuration) {
    BitBoard pieceConfiguration = 0;
    BitBoard piecePosition;
    while (mask != 0) {
        piecePosition = mask & -mask;
        if ((configuration & 0x1) == 1) {
            pieceConfiguration |= piecePosition;
        }
        configuration >>= 1;
        mask &= mask-1;
    }
    return ~pieceConfiguration;
}

const std::array<std::array<BitBoard, (1L << 9)>, 64> generateBishopAttackMaps() {
    std::array<std::array<BitBoard, (1L << 9)>, 64> bishopMagic;
    //For each square:
    for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
        BitBoard origin = GetSquareBitBoard(squareIndex);
        //get mask bits
        BitBoard maskBits = bishopMask[squareIndex];
#ifndef __BMI2__
        //get magic number
        BitBoard magic = bishopMagicNumber[squareIndex];
#endif
        //For each possible configuration of masked bits
        for (int configuration = 0; configuration < (0x1L << bishopShiftBits[squareIndex]); configuration++) {
            BitBoard empty = getConfigurationOfEmpty(maskBits, configuration);
            //Calculate attack map
            BitBoard attackMap = bitops::NE(bitops::FillNorthEast(origin, empty)) |
                    bitops::SE(bitops::FillSouthEast(origin, empty)) |
                    bitops::SW(bitops::FillSouthWest(origin, empty)) |
                    bitops::NW(bitops::FillNorthWest(origin, empty));
            //Calculate Index via multiplication of magic number
            //Save calculated attack map to calculated index for square index of piece
#ifdef __BMI2__
              bishopMagic[squareIndex][_pext_u64(~empty, maskBits)] = attackMap;
#else
              bishopMagic[squareIndex][(int) ((~empty * magic) >> (64 - bishopShiftBits[squareIndex]))] = attackMap;
#endif
        //end
        }
    //end
    }
    return bishopMagic;
}

const std::array<std::array<BitBoard, (1L << 12)>, 64> generateRookAttackMaps() {
    std::array<std::array<BitBoard, (1L << 12)>, 64> rookMagic;
    //For each square:
    for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
        BitBoard origin = GetSquareBitBoard(squareIndex);
        //get mask bits
        BitBoard maskBits = rookMask[squareIndex];
#ifndef __BMI2__
        //get magic number
        BitBoard magic = rookMagicNumber[squareIndex];
#endif
        //For each possible configuration of masked bits
        for (int configuration = 0; configuration < (0x1L << rookShiftBits[squareIndex]); configuration++) {
            BitBoard empty = getConfigurationOfEmpty(maskBits, configuration);
            //Calculate attack map
            BitBoard attackMap = bitops::N(bitops::FillNorth(origin, empty)) |
                bitops::E(bitops::FillEast(origin, empty)) |
                bitops::S(bitops::FillSouth(origin, empty)) |
                bitops::W(bitops::FillWest(origin, empty));
            //Calculate Index via multiplication of magic number
            //Save calculated attack map to calculated index for square index of piece
#ifdef __BMI2__
              rookMagic[squareIndex][_pext_u64(~empty, maskBits)] = attackMap;
#else
              rookMagic[squareIndex][(int) ((~empty * magic) >> (64 - rookShiftBits[squareIndex]))] = attackMap;
#endif
        //end
        }
    //end
    }
    return rookMagic;
}

const std::array<BitBoard, 64> initKingSafetyMap() {
  std::array<BitBoard, 64> king_map;
  for (Square a = 0; a < 64; a++) {
    Square pos = a;
    if (GetSquareX(a) == 0) {
      assert(pos <= 56);
      pos++;
    }
    else if (GetSquareX(a) == 7) {
      assert(pos >= 7);
      pos--;
    }
    if (GetSquareY(a) == 0) {
      assert(pos < 8);
      pos += 8;
    }
    else if (GetSquareY(a) == 7) {
      assert(pos >= 56);
      pos -= 8;
    }
    king_map[a] = kingAttackMap[pos] | GetSquareBitBoard(pos);
  }
  return king_map;
}

const std::array<BitBoard, 64> initFileMap() {
  std::array<BitBoard, 64> file_map;
  for (Square a = 0; a < 64; a++) {
    BitBoard bb = GetSquareBitBoard(a);
    bb = bitops::FillNorth(bb, ~0);
    bb = bitops::FillSouth(bb, ~0);
    file_map[a] = bb;
  }
  return file_map;
}

const std::array<BitBoard, 64> file_map = initFileMap();
const std::array<std::array<int, 64>, 64> distance_map = initDistMap();
const std::array<BitBoard, 64> kingSafetyMap = initKingSafetyMap();
const std::array<std::array<BitBoard, 64>, 64> attackVectorMap = generateAttackVectorMaps();
const std::array<std::array<BitBoard, (1L << 9)>, 64> bishopMagic = generateBishopAttackMaps();
const std::array<std::array<BitBoard, (1L << 12)>, 64> rookMagic = generateRookAttackMaps();

}

namespace magic{

template<> BitBoard GetAttackMap<kKnight>(const int &index, BitBoard allPieces) {
    return knightAttackMap[index];
}

template<> BitBoard GetAttackMap<kBishop>(const int &index, BitBoard allPieces) {
#ifdef __BMI2__
    return bishopMagic[index][_pext_u64(allPieces, bishopMask[index])];
#else
  allPieces &= bishopMask[index];
  return bishopMagic[index][(int) ((allPieces * bishopMagicNumber[index]) >> (64 - bishopShiftBits[index]))];
#endif
}

template<> BitBoard GetAttackMap<kRook>(const int &index, BitBoard allPieces) {
#ifdef __BMI2__
    return rookMagic[index][_pext_u64(allPieces, rookMask[index])];
#else
    allPieces &= rookMask[index];
    return rookMagic[index][(int) ((allPieces * rookMagicNumber[index]) >> (64 - rookShiftBits[index]))];
#endif
}

template<> BitBoard GetAttackMap<kQueen>(const int &index, BitBoard allPieces) {
    return GetAttackMap<kBishop>(index, allPieces) | GetAttackMap<kRook>(index, allPieces);
}

template<> BitBoard GetAttackMap<kKing>(const int &index, BitBoard allPieces) {
    return kingAttackMap[index];
}

BitBoard getAttackVectors(BitBoard src, BitBoard des) {
    BitBoard res = 0;
    Square i = bitops::NumberOfTrailingZeros(src);
    while (des) {
        res |= attackVectorMap[i][bitops::NumberOfTrailingZeros(des)];
        bitops::PopLSB(des);
    }
    return res;
}

BitBoard GetAttackMap(PieceType piece_type, Square square, BitBoard all_pieces) {
  switch (piece_type) {
  case kQueen: return GetAttackMap<kQueen>(square, all_pieces);
  case kRook: return GetAttackMap<kRook>(square, all_pieces);
  case kBishop: return GetAttackMap<kBishop>(square, all_pieces);
  case kKnight: return GetAttackMap<kKnight>(square, all_pieces);
  default: return 0;
  }
}

int GetSquareDistance(const Square a, const Square b) {
  return distance_map[a][b];
}

BitBoard GetKingArea(const Square square) {
  return kingSafetyMap[square];
}

BitBoard GetSquareFile(const Square square) {
  return file_map[square];
}

}

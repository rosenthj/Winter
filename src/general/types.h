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
 * types.h
 *
 *  Created on: Nov 1, 2016
 *      Author: Jonathan Rosenthal
 */

#ifndef GENERAL_TYPES_H_
#define GENERAL_TYPES_H_

#include <cassert>
#include <cstdint>
#include <chrono>
#include <array>

//A bitboard is a 64bit boolean feature representation of the board.
typedef uint64_t BitBoard;
//A square is defined by its index
typedef int32_t Square;
//A move references a source, a destination square and a move type, but no piece.
typedef int32_t Move;
//A movetype references whether a move is a normal move or a special move
//such as a pawn promotion or castling.
typedef Move MoveType;
//A piece is an object with a PieceType and Color
typedef int32_t Piece;
//A piecetype is a reference to one of King,Queen,Rook,Bishop,Knight or Pawn
typedef Piece PieceType;
//A color is a reference to one of the players.
typedef Piece Color;
//A score is a value assigned to a position which represents how good the position
//for the player to move.
typedef int32_t Score;
typedef int32_t Depth;
typedef double Probability;
typedef int32_t CastlingRights;
typedef uint64_t HashType;
typedef std::chrono::time_point<std::chrono::steady_clock> Time;
typedef std::chrono::milliseconds Milliseconds;

inline Time now() { return std::chrono::steady_clock::now(); }

template<typename T, size_t i, size_t j>
using Array2d = std::array<std::array< T, j>, i>;

template<typename T, size_t i, size_t j, size_t k>
using Array3d = std::array<Array2d< T, j, k>, i>;

const Color kWhite = 0;
const Color kBlack = 1;

const PieceType kPawn = 0;
const PieceType kKnight = 1;
const PieceType kBishop = 2;
const PieceType kRook = 3;
const PieceType kQueen = 4;
const PieceType kKing = 5;
const PieceType kNoPiece = 6;
const int kNumPieceTypes = 7;

const int kNumPlayers = 2;

const int kBoardLength = 8;
const int kBoardSize = kBoardLength * kBoardLength;

const int kNonQuiescent = 0;
const int kQuiescent = 1;

const MoveType kNormalMove = 0;
const MoveType kCastle = 1;
const MoveType kDoublePawnMove = 2;
const MoveType kEnPassant = 3;
const MoveType kCapture = 4;
//To get promotion piece we can subtract (kKnightPromotion - kKnight) from the movetype
const MoveType kKnightPromotion = 5;
const MoveType kBishopPromotion = 6;
const MoveType kRookPromotion = 7;
const MoveType kQueenPromotion = 8;

const Move kNullMove = 0;

const CastlingRights kWSCastle = 1;
const CastlingRights kWLCastle = kWSCastle << 1;
const CastlingRights kBSCastle = kWSCastle << 2;
const CastlingRights kBLCastle = kWSCastle << 3;

const Score kMaxScore = 100000;
const Score kMinScore = -kMaxScore;
const Score kNoScore = kMinScore-1;

const int kLowerBound = 1;
const int kUpperBound = 2;
const int kExactBound = kLowerBound | kUpperBound;

const int kOpening = 0;
const int kEnding = 1;

const int piece_phases[7] = {0, 3, 3, 4, 9, 0, 0};
const int kMaxPhase = 16 * piece_phases[kPawn] + 4 * piece_phases[kKnight]
              + 4 * piece_phases[kBishop] + 4 * piece_phases[kRook]
              + 2 * piece_phases[kQueen];

const size_t kPhaseVecLength = 4;

const int64_t kThousand = 1000;
const int64_t kMillion = kThousand * kThousand;
const int64_t kBillion = kThousand * kMillion;

/**
 * In the following we define data extraction and translation functions,
 * however we will not be defining bit data manipulation functions.
 * For bit twiddling functions see bitoperations.
 */
inline int32_t GetSquareX(const Square square) {
  assert(square >= 0 && square < 64);
  return square % 8; }
inline int32_t GetSquareY(const Square square) {
  assert(square >= 0 && square < 64);
  return square / 8;
}
inline Square GetSquare(const int32_t x, const int32_t y) {
  assert(x >= 0 && x < 8 && y >= 0 && y < 8);
  return x + y * 8;
}

inline Square GetMoveSource(const Move move) { return (move >> 6) & 0x3F; }
inline Square GetMoveDestination(const Move move) { return move & 0x3F; }
inline MoveType GetMoveType(const Move move) { return move >> 12; }
inline int IsMoveForcing(const Move move) {
  return (GetMoveType(move) + 5) >> 3;
}
inline Move GetMove(const Square source, const Square destination,
                    const MoveType move_type = kNormalMove) {
  assert(source >= 0 && source < 64 && destination >= 0 && destination < 64);
  assert(source != destination || move_type == kNormalMove);
  return move_type << 12 | (source << 6) | destination;
}

inline constexpr Piece GetPiece(const Color color, const PieceType piece_type) {
  return (color << 3) | piece_type;
}
inline Color GetPieceColor(const Piece piece) { return piece >> 3; }
inline PieceType GetPieceType(const Piece piece) { return piece & 0x7; }

inline BitBoard GetSquareBitBoard(const Square square) {
  assert(square >= 0 && square < 64);
  return ((BitBoard) 1) << square;
}


#endif /* GENERAL_TYPES_H_ */

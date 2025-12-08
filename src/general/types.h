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

#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include "wdl_score.h"

// A bitboard is a 64bit boolean feature representation of the board.
using BitBoard = uint64_t;
// A square is defined by its index
using Square = int32_t;
// A move references a source, a destination square and a move type, but no piece.
using Move = int32_t;
// A movetype references whether a move is a normal move or a special move
// such as a pawn promotion or castling.
using MoveType = Move;
// MoveScore is used to order moves.
using MoveScore = int32_t;
// A piece is an object with a PieceType and Color
using Piece = int32_t;
// A piecetype is a reference to one of King,Queen,Rook,Bishop,Knight or Pawn
using PieceType = Piece;
// A color is a reference to one of the players.
using Color = Piece;
// A score is a value assigned to a position which represents how good the position
// for the player to move.
using Score = WDLScore;
using NScore = int32_t;
using Depth = int32_t;
using Probability = double;
using CastlingRights = int32_t;
using HashType = uint64_t;
using Time = std::chrono::time_point<std::chrono::steady_clock>;
using Milliseconds = std::chrono::milliseconds;

inline Time now() { return std::chrono::steady_clock::now(); }

template<typename T, size_t i, size_t j>
using Array2d = std::array<std::array< T, j>, i>;

template<typename T, size_t i, size_t j, size_t k>
using Array3d = std::array<Array2d< T, j, k>, i>;

constexpr size_t kErrorHistorySize = 16384;
using ErrorHistory = Array2d<float, kErrorHistorySize, 3>;

constexpr Color kWhite = 0;
constexpr Color kBlack = 1;

constexpr PieceType kPawn = 0;
constexpr PieceType kKnight = 1;
constexpr PieceType kBishop = 2;
constexpr PieceType kRook = 3;
constexpr PieceType kQueen = 4;
constexpr PieceType kKing = 5;
constexpr PieceType kNoPiece = 6;
constexpr int kNumPieceTypes = 7;

constexpr int kNumPlayers = 2;

constexpr int kBoardLength = 8;
constexpr int kBoardSize = kBoardLength * kBoardLength;

constexpr int kNonQuiescent = 0;
constexpr int kQuiescent = 1;

constexpr MoveType kNormalMove = 0;
constexpr MoveType kCastle = 1;
constexpr MoveType kDoublePawnMove = 2;
constexpr MoveType kEnPassant = 3;
constexpr MoveType kCapture = 4;
//To get promotion piece we can subtract (kKnightPromotion - kKnight) from the movetype
constexpr MoveType kKnightPromotion = 5;
constexpr MoveType kBishopPromotion = 6;
constexpr MoveType kRookPromotion = 7;
constexpr MoveType kQueenPromotion = 8;

constexpr Move kNullMove = 0;

constexpr CastlingRights kWSCastle = 1;
constexpr CastlingRights kWLCastle = kWSCastle << 1;
constexpr CastlingRights kBSCastle = kWSCastle << 2;
constexpr CastlingRights kBLCastle = kWSCastle << 3;

constexpr NScore kRescale = WDLScore::scale;
constexpr Score kMaxStaticEval = WDLScore::get_max_static();
constexpr Score kMinStaticEval = WDLScore::get_min_static();

constexpr Score kMinMatingScore = WDLScore::get_min_mating();
constexpr Score kMaxMatedScore = WDLScore::get_max_mated();

constexpr Score kMaxScore = WDLScore::get_max_score();
constexpr Score kMinScore = WDLScore::get_min_score();
constexpr Score kNoScore = WDLScore::get_no_score();
constexpr Score kDrawScore = WDLScore::get_draw_score();

inline constexpr Score GetMatedOnMoveScore(int32_t ply) {
  return WDLScore::mated_on_ply(ply);
}

constexpr NScore kMaxNScore = WDLScore::get_max_score().to_nscore();
constexpr NScore kMinNScore = -kMaxNScore;

inline constexpr Color other_color(Color color) {
  return color ^ 0x1;
}

constexpr int kLowerBound = 1;
constexpr int kUpperBound = 2;
constexpr int kExactBound = kLowerBound | kUpperBound;

constexpr int kOpening = 0;
constexpr int kEnding = 1;

constexpr int64_t kThousand = 1000;
constexpr int64_t kMillion = kThousand * kThousand;
constexpr int64_t kBillion = kThousand * kMillion;

/**
 * In the following we define data extraction and translation functions,
 * however we will not be defining bit data manipulation functions.
 * For bit twiddling functions see bitoperations.
 */
inline constexpr int32_t GetSquareX(const Square square) {
//  assert(square >= 0 && square < 64);
  return square % 8;
}
inline constexpr int32_t GetSquareY(const Square square) {
//  assert(square >= 0 && square < 64);
  return square / 8;
}
inline constexpr Square GetSquare(const int32_t x, const int32_t y) {
//  assert(x >= 0 && x < 8 && y >= 0 && y < 8);
  return x + y * 8;
}

inline constexpr Square GetMirroredSquare(const Square square) {
  return GetSquare(GetSquareX(square), 8 - 1 - GetSquareY(square));
}

inline constexpr Square GetMoveSource(const Move move) { return (move >> 6) & 0x3F; }
inline constexpr Square GetMoveDestination(const Move move) { return move & 0x3F; }
inline constexpr MoveType GetMoveType(const Move move) { return move >> 12; }
inline constexpr int IsMoveForcing(const Move move) {
  return (GetMoveType(move) + 5) >> 3;
}
inline constexpr Move GetMove(const Square source, const Square destination,
                    const MoveType move_type = kNormalMove) {
//  assert(source >= 0 && source < 64 && destination >= 0 && destination < 64);
//  assert(source != destination || move_type == kNormalMove);
  return move_type << 12 | (source << 6) | destination;
}

inline constexpr Piece GetPiece(const Color color, const PieceType piece_type) {
  return (color << 3) | piece_type;
}
inline constexpr Color GetPieceColor(const Piece piece) { return piece >> 3; }
inline constexpr PieceType GetPieceType(const Piece piece) { return piece & 0x7; }

inline constexpr BitBoard GetSquareBitBoard(const Square square) {
//  assert(square >= 0 && square < 64);
  return ((BitBoard) 1) << square;
}


#endif /* GENERAL_TYPES_H_ */

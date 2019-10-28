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
using Score = int32_t;
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

const int kNonQuiescent = 0;
const int kQuiescent = 1;

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

constexpr Score kRescale = 4000; // Used to transform wpct into Score.
constexpr Score kMaxStaticEval = kRescale;
constexpr Score kMinStaticEval = -kMaxStaticEval;

constexpr Score kMinMatingScore = kMaxStaticEval + 100;
constexpr Score kMaxMatedScore = -kMinMatingScore;

constexpr Score kNumMateInScores = 2000;
constexpr Score kMaxScore = kMinMatingScore + kNumMateInScores;
constexpr Score kMinScore = -kMaxScore;
constexpr Score kNoScore = kMinScore-1;

inline Score get_next_score(Score score) {
  if (score == kMaxStaticEval) {
    return kMinMatingScore;
  }
  if (score == kMaxMatedScore) {
    return kMinStaticEval;
  }
  return score + 1;
}

inline Score get_previous_score(Score score) {
  if (score == kMinMatingScore) {
    return kMaxStaticEval;
  }
  if (score == kMinStaticEval) {
    return kMaxMatedScore;
  }
  return score - 1;
}

// Returns true for scores in range for static eval. This includes draw_score.
inline constexpr bool is_static_eval(Score score) {
  return (score >= kMinStaticEval) && (score <= kMaxStaticEval);
}

inline constexpr bool is_mate_score(Score score) {
  return (score <= kMaxMatedScore) || (score >= kMinMatingScore);
}

inline constexpr bool is_valid_score(Score score) {
  return score == kNoScore || (score >= kMinScore && score <= kMaxScore &&
      (is_static_eval(score) || is_mate_score(score)));
}

inline Score wpct_to_score(float x) {
  return std::round(kRescale * (2*x - 1));
}

inline constexpr float score_to_wpct(Score x) {
  return (((float)x / kRescale) + 1.0) / 2.0;
}

inline Score wpct_to_cp(float wpct) {
  constexpr float kEpsilon = 0.000001;
  wpct = std::max(std::min(wpct, 1-kEpsilon), kEpsilon);
  return std::round(std::log(wpct / (1-wpct)) * 1024);
}

// Rounds score to next valid score
inline Score get_valid_score(Score score) {
  if (!is_valid_score(score) || score == kNoScore) {
    if (score < kMinScore) {
      return kMinScore;
    }
    if (score < kMinStaticEval) {
      return kMinStaticEval;
    }
    if (score > kMaxScore) {
      return kMaxScore;
    }
    if (score > kMaxStaticEval) {
      return kMaxStaticEval;
    }
    assert(false); // Unreachable
  }
  return score;
}

constexpr int kLowerBound = 1;
constexpr int kUpperBound = 2;
constexpr int kExactBound = kLowerBound | kUpperBound;

constexpr int kOpening = 0;
constexpr int kEnding = 1;

constexpr int piece_phases[7] = {0, 3, 3, 4, 9, 0, 0};
constexpr int kMaxPhase = 16 * piece_phases[kPawn] + 4 * piece_phases[kKnight]
              + 4 * piece_phases[kBishop] + 4 * piece_phases[kRook]
              + 2 * piece_phases[kQueen];

constexpr size_t kPhaseVecLength = 4;

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

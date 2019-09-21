/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2019 Jonathan Rosenthal
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
 * net_evaluation.h
 *
 * The evaluation source and header files with associated namespace is used
 * in order to score positions so the search function can differentiate how
 * good different positions are
 *
 *  Created on: June 26, 2019
 *      Author: Jonathan Rosenthal
 */

#ifndef NET_EVALUATION_H_
#define NET_EVALUATION_H_

#include "general/types.h"
#include "general/settings.h"
#include "board.h"
#include <vector>

namespace net_evaluation {

Score ScoreBoard(const Board &board);
// Returns the input features for the net for a specific board position.
// In the future this may become more complicated, depending on how pieces get encoded.
std::vector<int32_t> GetNetInputs(const Board &board);
void GenerateDatasetFromEPD();
void init_weights();
void EstimateFeatureImpact();
void GenerateDatasetFromUCIGames(std::string filename, std::string out_name = "eval_dataset.csv",
                                 size_t reroll_pct = 0);

void SetContempt(int value, Color color);
std::array<Score, 2> GetDrawArray();
Score GetUnbiasedScore(Score score, Color color);

}

// TODO: Move to external file
namespace net_features {

// TODO: remove outdated features
// Side dependent eval features
constexpr size_t kPawnCountIdx = 0; // 0-8  // The number of pawns
constexpr size_t kKnightCountIdx = kPawnCountIdx + 9; // 9-11
constexpr size_t kBishopCountIdx = kKnightCountIdx + 3; // 12-14
constexpr size_t kRookCountIdx = kBishopCountIdx + 3; // 15-17
constexpr size_t kQueenCountIdx = kRookCountIdx + 3; // 18
constexpr size_t kKingPSTIdx = kQueenCountIdx + 1; // 19-28
constexpr size_t kKnightPSTIdx = kKingPSTIdx + 10; // 29-38
constexpr size_t kActivityBonusIndex = kKnightPSTIdx + 10; // 39-43
constexpr size_t kAbstractActivityIndex = kActivityBonusIndex + 5; // 44-48 //No King
constexpr size_t kDoublePawnPenaltyIndex = kAbstractActivityIndex + 5; // 49 //No king
constexpr size_t kIsolatedPawnIndex = kDoublePawnPenaltyIndex + 1; // 50
constexpr size_t kDirectPawnShieldBonus = kIsolatedPawnIndex + 1; // 51
constexpr size_t kKingVectorExposure = kDirectPawnShieldBonus + 1; // 52-53
constexpr size_t kKnightOutpost = kKingVectorExposure + 2; // 54
//constexpr size_t kCastlingRight = kKnightOutpost + 1;
constexpr size_t kPawnAttackIndex = kKnightOutpost + 1; // 55
constexpr size_t kMinorAttackIndex = kPawnAttackIndex + 1; // 56
constexpr size_t kKingAttackDistance = kMinorAttackIndex + 1; // 57-60 // kKingDefenceDistance + 4; // No pawns, no king
constexpr size_t kKingAttack = kKingAttackDistance + 4; // 61-64 // No pawns, no king
constexpr size_t kKnightSquaresIndex = kKingAttack + kQueen - kPawn; // 65-73
constexpr size_t kBishopMobility = kKnightSquaresIndex + 9; // 74-87
constexpr size_t kRookMobility = kBishopMobility + 14; // 88-102
constexpr size_t kRookOpenFile = kRookMobility + 15; // 103
constexpr size_t kQueenMobility = kRookOpenFile + 1; // 104-128
constexpr size_t kSafeChecks = kQueenMobility + 25; // 129
constexpr size_t kUnSafeChecks = kSafeChecks + 1; // 130
constexpr size_t kUnprotectedPieces = kUnSafeChecks + 1; // 131
constexpr size_t kBishopVsKingPosition = kUnprotectedPieces + 1; // 132-167
constexpr size_t kOpposedPawn = kBishopVsKingPosition + 36; // 168-191
constexpr size_t kUnopposedPawn = kOpposedPawn + (2 * 2 * 6); // 192-215
constexpr size_t kCoveredPassedPawn = kUnopposedPawn + (2 * 2 * 6); // 216-239
constexpr size_t kUncoveredPassedPawn = kCoveredPassedPawn + (2 * 2 * 6); // 240-263
constexpr size_t kPawnEvalIndex = kOpposedPawn;
constexpr size_t kSideDependentFeatureCount = kUncoveredPassedPawn + (2 * 2 * 6);


// The remaining features cannot be asigned to a side, but may effect the evaluation in some way.
constexpr size_t kOppositeColoredBishops = 2 * kSideDependentFeatureCount;
constexpr size_t kTotalNumFeatures = kOppositeColoredBishops + 1;

// TODO: Find a nice way to get rid of this, NN should be able to handle this better.
const int kMinorKingAttackWeight = 2;
const int kRookKingAttackWeight = 3;
const int kQueenKingAttackWeight = 5;

const int kKingAttackerScaling[7] = {0, 0, 4, 6, 7, 7, 8};
}


#endif /* NET_EVALUATION_H_ */

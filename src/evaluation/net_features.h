/*
 *  Winter is a UCI chess engine.
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
 * This is a header file containing handcrafted features used by Winter nets.
 *
 *      Author: Jonathan Rosenthal
 */

#ifndef NET_FEATURES_H_
#define NET_FEATURES_H_

namespace net_features {

// TODO: remove outdated features
// Side dependent eval features
constexpr size_t kPawnCountIdx = 0; // 0-8  // The number of pawns
constexpr size_t kKnightCountIdx = kPawnCountIdx + 9; // 9-11
constexpr size_t kBishopCountIdx = kKnightCountIdx + 3; // 12-14
constexpr size_t kRookCountIdx = kBishopCountIdx + 3; // 15-17
constexpr size_t kQueenCountIdx = kRookCountIdx + 3; // 18
//constexpr size_t kKingPSTIdx = kQueenCountIdx + 1; // 19-28
//constexpr size_t kKnightPSTIdx = kKingPSTIdx + 10; // 29-38
constexpr size_t kKnightPSTIdx = kQueenCountIdx + 1;
constexpr size_t kActivityBonusIndex = kKnightPSTIdx + 10; // 39-43
constexpr size_t kAbstractActivityIndex = kActivityBonusIndex + 5; // 44-48 //No King
constexpr size_t kDoublePawnPenaltyIndex = kAbstractActivityIndex + 5; // 49 TODO remove
constexpr size_t kIsolatedPawnIndex = kDoublePawnPenaltyIndex + 1; // 50 TODO remove
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
constexpr size_t kSideDependentFeatureCount = kBishopVsKingPosition + 36;


// The remaining features cannot be asigned to a side, but may effect the evaluation in some way.
constexpr size_t kOppositeColoredBishops = 2 * kSideDependentFeatureCount;
constexpr size_t kTotalNumFeatures = kOppositeColoredBishops + 1;

// TODO: Find a nice way to get rid of this, NN should be able to handle this better.
const int kMinorKingAttackWeight = 2;
const int kRookKingAttackWeight = 3;
const int kQueenKingAttackWeight = 5;

const int kKingAttackerScaling[7] = {0, 0, 4, 6, 7, 7, 8};

constexpr size_t kChanOpposedPawnsIdx = 0;
constexpr size_t kChanUnoppposedPawnsIdx = kChanOpposedPawnsIdx + 1;
constexpr size_t kChanCoveredPassedPawnsIdx = kChanUnoppposedPawnsIdx + 1;
constexpr size_t kChanUncoveredPassedPawnsIdx = kChanCoveredPassedPawnsIdx + 1;
constexpr size_t kChanKingsIdx = kChanUncoveredPassedPawnsIdx + 1;

constexpr size_t kChannelsPerSide = kChanKingsIdx + 1;
constexpr size_t kNumChannels = 2 * kChannelsPerSide;
}

#endif /* NET_FEATURES_H_ */

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
 * feature_indexes.h
 *
 *  Created on: May 16, 2017
 *      Author: Jonathan Rosenthal
 */

#ifndef SRC_GENERAL_FEATURE_INDEXES_H_
#define SRC_GENERAL_FEATURE_INDEXES_H_

#include "types.h"
#include <array>

const int kPSTindex[64] = {
    0, 1, 2, 3, 3, 2, 1, 0,
    1, 4, 5, 6, 6, 5, 4, 1,
    2, 5, 7, 8, 8, 7, 5, 2,
    3, 6, 8, 9, 9, 8, 6, 3,
    3, 6, 8, 9, 9, 8, 6, 3,
    2, 5, 7, 8, 8, 7, 5, 2,
    1, 4, 5, 6, 6, 5, 4, 1,
    0, 1, 2, 3, 3, 2, 1, 0
};

struct FeatureInfo {
  FeatureInfo(std::string info_, int idx_, Score min = kMinScore,
              Score max = kMaxScore, int encoded_info_ = 0) :
    info(info_), idx(idx_), min_value(min),
    max_value(max), encoded_info(encoded_info_){ };
  const std::string info;
  const size_t idx;
  const Score min_value;
  const Score max_value;
  const int encoded_info;
};

namespace positional_features {

const int kMinorKingAttackWeight = 2;
const int kRookKingAttackWeight = 3;
const int kQueenKingAttackWeight = 5;

const int kKingAttackerScaling[7] = {0, 0, 4, 6, 7, 7, 8};

const size_t kKingPSTIndex = 0;
const size_t kKnightPSTIndex = kKingPSTIndex + 10;
const size_t kBaseValueIndex = kKnightPSTIndex + 10;
const size_t kBishopPairIndex = kBaseValueIndex + 5;//No King
const size_t kActivityBonusIndex = kBishopPairIndex + 1;
const size_t kAbstractActivityIndex = kActivityBonusIndex + 5;//No King
const size_t kPassedPawnBonusIndex = kAbstractActivityIndex + 5;//No king
//const size_t kPassedPawnUnblocked = kPassedPawnBonusIndex + 6;
//const size_t kPassedPawnPushBonusIndex = kPassedPawnBonusIndex + 6;
const size_t kDoublePawnPenaltyIndex = kPassedPawnBonusIndex + 12;//kRookBehindPasser + 1;
const size_t kIsolatedPawnIndex = kDoublePawnPenaltyIndex + 1;
const size_t kTempoBonusIndex = kIsolatedPawnIndex + 1;
const size_t kDirectPawnShieldBonus = kTempoBonusIndex + 1;
const size_t kKingVectorExposure = kDirectPawnShieldBonus + 1;
const size_t kKnightOutpost = kKingVectorExposure + 2;
const size_t kCastlingRight = kKnightOutpost + 1;
const size_t kPawnAttackIndex = kCastlingRight + 1;
const size_t kMinorAttackIndex = kPawnAttackIndex + 1;
const size_t kKingDefenceDistance = kMinorAttackIndex + 1;
const size_t kKingAttackDistance = kKingDefenceDistance + 4; // No pawns, no king
const size_t kKingAttack = kKingAttackDistance + 4; // No pawns, no king
const size_t kKnightSquaresIndex = kKingAttack + kQueen - kPawn;
const size_t kBishopMobility = kKnightSquaresIndex + 9;
const size_t kRookMobility = kBishopMobility + 14;
const size_t kRookOpenFile = kRookMobility + 15;
const size_t kQueenMobility = kRookOpenFile + 1;
const size_t kSafeChecks = kQueenMobility + 28;
const size_t kUnSafeChecks = kSafeChecks + 1;
const size_t kUnprotectedPieces = kUnSafeChecks + 1;
const size_t kBishopVsKingPosition = kUnprotectedPieces + 1;
const size_t kOpposedPawn = kBishopVsKingPosition + 36;
const size_t kUnopposedPawn = kOpposedPawn + (2 * 2 * 6);
const size_t kCoveredPassedPawn = kUnopposedPawn + (2 * 2 * 6);
const size_t kUncoveredPassedPawn = kCoveredPassedPawn + (2 * 2 * 6);
const size_t kPawnEvalIndex = kOpposedPawn;
const size_t kNumFeatures = kUncoveredPassedPawn + (2 * 2 * 6);



const std::array<FeatureInfo, 33> kFeatureInfos = {{
    FeatureInfo("King Piece Square Table", kKingPSTIndex),
    FeatureInfo("Knight Piece Square Table", kKnightPSTIndex, 0, kMaxScore),
    FeatureInfo("Piece Base Value", kBaseValueIndex, kMinScore, kMaxScore),
    FeatureInfo("Bishop Pair", kBishopPairIndex, 0, kMaxScore),
    FeatureInfo("Piece Activity Bonus", kActivityBonusIndex, 0, kMaxScore),
    FeatureInfo("Abstract Piece Activity Bonus", kAbstractActivityIndex, 0, kMaxScore),
    FeatureInfo("Passed Pawn Bonus", kPassedPawnBonusIndex, 0, kMaxScore),
    //FeatureInfo("Passed Pawn Unblocked", kPassedPawnUnblocked),
    //FeatureInfo("Rook Behind Passed Pawn Bonus", kRookBehindPasser, -5, kMaxScore),
    FeatureInfo("Double Pawn Penalty", kDoublePawnPenaltyIndex, kMinScore, 0),
    FeatureInfo("Isolated Pawn", kIsolatedPawnIndex, kMinScore, 0),
    FeatureInfo("Tempo Bonus", kTempoBonusIndex, 0, kMaxScore),
    FeatureInfo("Direct Pawn Shield Bonus", kDirectPawnShieldBonus),
    FeatureInfo("King Vector Exposure", kKingVectorExposure),
    FeatureInfo("Knight Outpost", kKnightOutpost),
    FeatureInfo("Castling Right", kCastlingRight, 0, kMaxScore),
    FeatureInfo("Pawn Attack", kPawnAttackIndex, 0, kMaxScore),
    FeatureInfo("Minor Attack", kMinorAttackIndex, 0, kMaxScore),
    FeatureInfo("King Defence Distance", kKingDefenceDistance),
    FeatureInfo("King Attack Distance", kKingAttackDistance, kMinScore, 0),
    FeatureInfo("King Attack Danger", kKingAttack, 0, kMaxScore),
    FeatureInfo("Safe Knight Square Count", kKnightSquaresIndex, kMinScore, kMaxScore, 1),
    FeatureInfo("Bishop Mobility", kBishopMobility, kMinScore, kMaxScore, 1),
    FeatureInfo("Rook Mobility", kRookMobility, kMinScore, kMaxScore, 1),
    FeatureInfo("Rook on Open File", kRookOpenFile, 0, kMaxScore),
    FeatureInfo("Queen Mobility", kQueenMobility, kMinScore, kMaxScore, 1),
    FeatureInfo("Safe Checks", kSafeChecks),
    FeatureInfo("Unsafe Checks", kUnSafeChecks),
    FeatureInfo("Unprotected Pieces", kUnprotectedPieces),
    FeatureInfo("Bishop's Position Relative To King", kBishopVsKingPosition),
    FeatureInfo("Opposed Pawns", kOpposedPawn),
    FeatureInfo("Unopposed Pawns", kUnopposedPawn),
    FeatureInfo("Covered Passed Pawns", kCoveredPassedPawn),
    FeatureInfo("Uncovered Passed Pawns", kUncoveredPassedPawn),
    FeatureInfo("Num Features Placeholder", kNumFeatures)
}};

}

namespace move_features {

const size_t kPWIHashMove = 0;
const size_t kPWIKiller = kPWIHashMove + 1;
const size_t kPWICounterMove = kPWIKiller + 2;
const size_t kPWIPieceTypeXTargetPieceType = kPWICounterMove + 1;
const size_t kPWIMoveType = kPWIPieceTypeXTargetPieceType + 36;
const size_t kPWIMoveSource = kPWIMoveType + 9;
const size_t kPWIMoveDestination = kPWIMoveSource + 10;
const size_t kPWIKnightMoveSource = kPWIMoveDestination + 10;
const size_t kPWIKnightMoveDestination = kPWIKnightMoveSource + 10;
const size_t kPWICaptureLastMoved = kPWIKnightMoveDestination + 10;
const size_t kPWISEE = kPWICaptureLastMoved + 1;
const size_t kPWIGivesCheck = kPWISEE + 2;
const size_t kPWITabooDestination = kPWIGivesCheck + 2;
const size_t kPWIForcingChanges = kPWITabooDestination + 1;
const size_t kPWIPawnRankDestination = kPWIForcingChanges + 4;
//const size_t kNumMoveProbabilityFeatures = kPWIForcingChanges + 4;
const size_t kPWIPassedRankDestination = kPWIPawnRankDestination + 6;
const size_t kPWIPawnAttack = kPWIPassedRankDestination + 6;
const size_t kPWIPieceUnderAttack = kPWIPawnAttack + 1;
const size_t kPWIHistory = kPWIPieceUnderAttack + 2;
const size_t kPWICMH = kPWIHistory + 1;
const size_t kNumMoveProbabilityFeatures = kPWICMH + 2;

const std::array<FeatureInfo, 21> kFeatureInfos = {{
    FeatureInfo("Hash Move", kPWIHashMove),
    FeatureInfo("Killer Move", kPWIKiller),
    FeatureInfo("Counter Move", kPWICounterMove),
    FeatureInfo("Moving and Target Piece Type", kPWIPieceTypeXTargetPieceType),
    FeatureInfo("Move Type", kPWIMoveType),
    FeatureInfo("Move Source", kPWIMoveSource),
    FeatureInfo("Move Destination", kPWIMoveDestination),
    FeatureInfo("Knight Move Source", kPWIKnightMoveSource),
    FeatureInfo("Knight Move Destination", kPWIKnightMoveDestination),
    FeatureInfo("Move Captures Last Moved Piece", kPWICaptureLastMoved),
    FeatureInfo("Move SEE", kPWISEE),
    FeatureInfo("Move Gives Check", kPWIGivesCheck),
    FeatureInfo("Move Destination Taboo", kPWITabooDestination),
    FeatureInfo("Changes Between Non-/Forcing States", kPWIForcingChanges),
    FeatureInfo("Rank of pawn destination", kPWIPawnRankDestination),
    FeatureInfo("Rank of passed pawn destination", kPWIPassedRankDestination),
    FeatureInfo("Pawn move attacks a piece", kPWIPawnAttack),
    FeatureInfo("Piece under attack", kPWIPieceUnderAttack),
    FeatureInfo("History", kPWIHistory),
    FeatureInfo("CMH", kPWICMH),
    FeatureInfo("Num Features Placeholder", kNumMoveProbabilityFeatures)
}};

}


#endif /* SRC_GENERAL_FEATURE_INDEXES_H_ */

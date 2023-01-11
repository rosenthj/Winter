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
  FeatureInfo(std::string info_, int idx_, NScore min = kMinNScore,
              NScore max = kMaxNScore, int encoded_info_ = 0) :
    info(info_), idx(idx_), min_value(min),
    max_value(max), encoded_info(encoded_info_){ };
  const std::string info;
  const size_t idx;
  const NScore min_value;
  const NScore max_value;
  const int encoded_info;
};

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

namespace qs_move_features {

const size_t kPWIHashMove = 0;
const size_t kPWIPromotion = kPWIHashMove + 1;
const size_t kPWICapture = kPWIPromotion + 8;
const size_t kNumQSMoveProbabilityFeatures = kPWICapture + 6 * 6;

}


#endif /* SRC_GENERAL_FEATURE_INDEXES_H_ */

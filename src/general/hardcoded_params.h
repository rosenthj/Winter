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
 * hardcoded_params.h
 *
 *  Created on: Nov 6, 2018
 *      Author: Jonathan Rosenthal
 */

#ifndef SRC_GENERAL_HARDCODED_PARAMS_H_
#define SRC_GENERAL_HARDCODED_PARAMS_H_

#include <array>

//The arrays in this file are automatically generated!
namespace hardcode {

// Search Parameters
// ------------------------------------------------------------------------------------------------


constexpr std::array<int, 117> search_params = {
    32000000,   //Hash move
    1952000,    //Killer move
    1440000,    //Killer move
    1616000,    //Counter move
    -1184000,   //Moving and target piece type
    1984000,    //Moving and target piece type
    480000,     //Moving and target piece type
    2704000,    //Moving and target piece type
    5200000,    //Moving and target piece type
    48000,      //Moving and target piece type
    0,          //Moving and target piece type
    -112000,    //Moving and target piece type
    -64000,     //Moving and target piece type
    1472000,    //Moving and target piece type
    4976000,    //Moving and target piece type
    16000,      //Moving and target piece type
    -128000,    //Moving and target piece type
    -1872000,   //Moving and target piece type
    2096000,    //Moving and target piece type
    1440000,    //Moving and target piece type
    5424000,    //Moving and target piece type
    -80000,     //Moving and target piece type
    -304000,    //Moving and target piece type
    1392000,    //Moving and target piece type
    1968000,    //Moving and target piece type
    2016000,    //Moving and target piece type
    4528000,    //Moving and target piece type
    -32000,     //Moving and target piece type
    -1984000,   //Moving and target piece type
    -224000,    //Moving and target piece type
    -144000,    //Moving and target piece type
    448000,     //Moving and target piece type
    2624000,    //Moving and target piece type
    -48000,     //Moving and target piece type
    -1312000,   //Moving and target piece type
    864000,     //Moving and target piece type
    688000,     //Moving and target piece type
    2880000,    //Moving and target piece type
    48000,      //Moving and target piece type
    -944000,    //Moving and target piece type
    -80000,     //Move type
    2288000,    //Move type
    -720000,    //Move type
    3184000,    //Move type
    4624000,    //Move type
    432000,     //Move type
    -16000000,  //Move type
    -16000000,  //Move type
    6400000,    //Move type
    576000,     //Move source
    240000,     //Move source
    256000,     //Move source
    336000,     //Move source
    -144000,    //Move source
    96000,      //Move source
    80000,      //Move source
    -128000,    //Move source
    80000,      //Move source
    -208000,    //Move source
    -1104000,   //Move destination
    -720000,    //Move destination
    -608000,    //Move destination
    -512000,    //Move destination
    -64000,     //Move destination
    -64000,     //Move destination
    144000,     //Move destination
    288000,     //Move destination
    176000,     //Move destination
    464000,     //Move destination
    2880000,    //Knight move source
    1296000,     //Knight move source
    448000,     //Knight move source
    704000,     //Knight move source
    672000,     //Knight move source
    496000,     //Knight move source
    160000,     //Knight move source
    32000,      //Knight move source
    -112000,     //Knight move source
    -272000,    //Knight move source
    -3696000,   //Knight move destination
    -2464000,   //Knight move destination
    -1680000,   //Knight move destination
    -944000,    //Knight move destination
    -1264000,    //Knight move destination
    -176000,    //Knight move destination
    272000,     //Knight move destination
    352000,     //Knight move destination
    672000,     //Knight move destination
    912000,     //Knight move destination
    1760000,    //Move captures last moved piece
    -6224000,   //Move SEE
    -3696000,   //Move SEE
    1504000,     //Move gives check
    2800000,    //Move gives check
    -5488000,   //Move destination is taboo
    -384000,    //Changes between non-/forcing states
    288000,     //Changes between non-/forcing states
    -240000,    //Changes between non-/forcing states
    688000,     //Changes between non-/forcing states
    -848000,    //Rank of pawn destination
    48000,      //Rank of pawn destination
    864000,     //Rank of pawn destination
    2304000,    //Rank of pawn destination
    16000,      //Rank of pawn destination
    176000,     //Rank of pawn destination
    -912000,    //Rank of passed pawn destination
    528000,     //Rank of passed pawn destination
    912000,     //Rank of passed pawn destination
    944000,     //Rank of passed pawn destination
    1744000,    //Rank of passed pawn destination
    16000,      //Rank of passed pawn destination
    1984000,    //Pawn move attacks piece
    1232000,     //Piece is under attack
    2576000,    //Piece is under attack
    137,      //History heuristic
    219,     //Countermove History
    47,      //Countermove History
};

constexpr std::array<int, 117> search_params_in_check = {
    32000000,   //Hash move
    880000,     //Killer move
    640000,     //Killer move
    1984000,    //Counter move
    -784000,    //Moving and target piece type
    -800000,    //Moving and target piece type
    -320000,    //Moving and target piece type
    480000,     //Moving and target piece type
    256000,     //Moving and target piece type
    160000,     //Moving and target piece type
    -768000,    //Moving and target piece type
    -1152000,    //Moving and target piece type
    0,      //Moving and target piece type
    -288000,    //Moving and target piece type
    416000,     //Moving and target piece type
    -336000,    //Moving and target piece type
    16000,      //Moving and target piece type
    176000,     //Moving and target piece type
    -96000,     //Moving and target piece type
    48000,      //Moving and target piece type
    736000,     //Moving and target piece type
    -48000,     //Moving and target piece type
    -176000,    //Moving and target piece type
    -400000,    //Moving and target piece type
    304000,     //Moving and target piece type
    912000,     //Moving and target piece type
    960000,     //Moving and target piece type
    112000,      //Moving and target piece type
    -272000,    //Moving and target piece type
    -384000,    //Moving and target piece type
    160000,     //Moving and target piece type
    -160000,    //Moving and target piece type
    608000,     //Moving and target piece type
    48000,      //Moving and target piece type
    -1344000,    //Moving and target piece type
    -80000,     //Moving and target piece type
    -320000,    //Moving and target piece type
    672000,     //Moving and target piece type
    672000,     //Moving and target piece type
    -1488000,    //Moving and target piece type
    -16000,     //Move type
    -32000,     //Move type
    -288000,    //Move type
    -2128000,   //Move type
    4128000,    //Move type
    2320000,    //Move type
    -16000000,  //Move type
    -16000000,  //Move type
    6368000,    //Move type
    544000,     //Move source
    0,      //Move source
    128000,      //Move source
    400000,     //Move source
    -32000,     //Move source
    -16000,     //Move source
    -48000,     //Move source
    -368000,    //Move source
    -464000,    //Move source
    -752000,    //Move source
    -2240000,   //Move destination
    -1056000,    //Move destination
    -1184000,    //Move destination
    -1344000,    //Move destination
    -176000,    //Move destination
    0,      //Move destination
    176000,     //Move destination
    480000,     //Move destination
    592000,     //Move destination
    880000,     //Move destination
    304000,     //Knight move source
    -64000,     //Knight move source
    112000,      //Knight move source
    64000,      //Knight move source
    0,      //Knight move source
    -32000,     //Knight move source
    64000,      //Knight move source
    224000,     //Knight move source
    -64000,     //Knight move source
    112000,      //Knight move source
    272000,     //Knight move destination
    -2016000,   //Knight move destination
    -1760000,   //Knight move destination
    -2160000,   //Knight move destination
    -912000,    //Knight move destination
    -208000,    //Knight move destination
    96000,      //Knight move destination
    16000,      //Knight move destination
    0,      //Knight move destination
    592000,     //Knight move destination
    0,      //Move captures last moved piece
    -5472000,   //Move SEE
    -5744000,   //Move SEE
    1168000,     //Move gives check
    896000,     //Move gives check
    -5984000,   //Move destination is taboo
    -64000,     //Changes between non-/forcing states
    896000,     //Changes between non-/forcing states
    -2224000,   //Changes between non-/forcing states
    16000,      //Changes between non-/forcing states
    -464000,    //Rank of pawn destination
    544000,     //Rank of pawn destination
    576000,     //Rank of pawn destination
    -80000,     //Rank of pawn destination
    48000,      //Rank of pawn destination
    96000,      //Rank of pawn destination
    -960000,    //Rank of passed pawn destination
    928000,     //Rank of passed pawn destination
    1872000,    //Rank of passed pawn destination
    1088000,     //Rank of passed pawn destination
    2112000,    //Rank of passed pawn destination
    32000,      //Rank of passed pawn destination
    -160000,    //Pawn move attacks piece
    432000,     //Piece is under attack
    -176000,    //Piece is under attack
    100,      //History heuristic
    210,     //Countermove History
    29,      //Countermove History
};


//constexpr std::array<int, 45> qs_params = {
//    150,                          // Hash Move
//    104, 100, 100, 115,           // Promotion Capture
//    100, 100, 100, 112,           // Promotion
//    105, 101, 103, 108, 110, 102, // Capture Pawn
//    116, 119, 111, 106, 114, 122, // Capture Knight
//    120, 121, 123, 107, 109, 118, // Capture Bishop
//    126, 113, 124, 125, 117, 128, // Capture Rook
//    133, 129, 130, 131, 127, 100, // Capture Queen
//    100, 100, 100, 100, 100, 100, // Capture King
//    //B1210887, B17-88196534
//};

}

#endif /* SRC_GENERAL_HARDCODED_PARAMS_H_ */

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
    2000, // Hash Move
    106,  // Killer Move
    98,   // Killer Move
    94,   // Counter Move
    -78,  // Moving and Target Piece Type
    116,  // Moving and Target Piece Type
    36,   // Moving and Target Piece Type
    181,  // Moving and Target Piece Type
    319,  // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    5,    // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    94,   // Moving and Target Piece Type
    313,  // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    -114, // Moving and Target Piece Type
    147,  // Moving and Target Piece Type
    95,   // Moving and Target Piece Type
    341,  // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    -18,  // Moving and Target Piece Type
    84,   // Moving and Target Piece Type
    116,  // Moving and Target Piece Type
    123,  // Moving and Target Piece Type
    293,  // Moving and Target Piece Type
    -1,   // Moving and Target Piece Type
    -120, // Moving and Target Piece Type
    -27,  // Moving and Target Piece Type
    1,    // Moving and Target Piece Type
    32,   // Moving and Target Piece Type
    161,  // Moving and Target Piece Type
    3,    // Moving and Target Piece Type
    -81,  // Moving and Target Piece Type
    56,   // Moving and Target Piece Type
    37,   // Moving and Target Piece Type
    189,  // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    -58,  // Moving and Target Piece Type
    0,    // Move Type
    142,  // Move Type
    -48,  // Move Type
    192,  // Move Type
    287,  // Move Type
    31,   // Move Type
    -1000,// Move Type
    -1000,// Move Type
    404,  // Move Type
    45,   // Move Source
    24,   // Move Source
    25,   // Move Source
    19,   // Move Source
    -8,   // Move Source
    -1,   // Move Source
    0,    // Move Source
    -7,   // Move Source
    -11,  // Move Source
    -16,  // Move Source
    -83,  // Move Destination
    -47,  // Move Destination
    -44,  // Move Destination
    -28,  // Move Destination
    -6,   // Move Destination
    0,    // Move Destination
    4,    // Move Destination
    13,   // Move Destination
    19,   // Move Destination
    30,   // Move Destination
    186,  // Knight Move Source
    80,   // Knight Move Source
    38,   // Knight Move Source
    44,   // Knight Move Source
    36,   // Knight Move Source
    29,   // Knight Move Source
    19,   // Knight Move Source
    0,    // Knight Move Source
    -17,  // Knight Move Source
    -22,  // Knight Move Source
    -226, // Knight Move Destination
    -159, // Knight Move Destination
    -102, // Knight Move Destination
    -67,  // Knight Move Destination
    -78,  // Knight Move Destination
    -12,  // Knight Move Destination
    3,    // Knight Move Destination
    27,   // Knight Move Destination
    44,   // Knight Move Destination
    60,   // Knight Move Destination
    101,  // Move Captures Last Moved Piece
    -387, // Move SEE
    -244, // Move SEE
    93,   // Move Gives Check
    180,  // Move Gives Check
    -343, // Move Destination Taboo
    0,    // Changes Between Non-/Forcing States
    0,    // Changes Between Non-/Forcing States
    -22,  // Changes Between Non-/Forcing States
    41,   // Changes Between Non-/Forcing States
    -46,  // Rank of pawn destination
    -8,   // Rank of pawn destination
    69,   // Rank of pawn destination
    141,  // Rank of pawn destination
    0,    // Rank of pawn destination
    0,    // Rank of pawn destination
    -50,  // Rank of passed pawn destination
    35,   // Rank of passed pawn destination
    51,   // Rank of passed pawn destination
    64,   // Rank of passed pawn destination
    101,  // Rank of passed pawn destination
    0,    // Rank of passed pawn destination
    119,  // Pawn move attacks a piece
    86,   // Piece under attack
    159,  // Piece under attack
    7,    // History // TODO: try smoothing histories so weights = 1.
    14,   // CMH
    2     // CMH
};

constexpr std::array<int, 117> search_params_in_check = {
    2000, // Hash Move
    60,   // Killer Move
    53,   // Killer Move
    131,  // Counter Move
    -50,  // Moving and Target Piece Type
    -47,  // Moving and Target Piece Type
    -21,  // Moving and Target Piece Type
    21,   // Moving and Target Piece Type
    8,    // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    -49,  // Moving and Target Piece Type
    -68,  // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    -22,  // Moving and Target Piece Type
    30,   // Moving and Target Piece Type
    -19,  // Moving and Target Piece Type
    -2,   // Moving and Target Piece Type
    3,    // Moving and Target Piece Type
    3,    // Moving and Target Piece Type
    9,    // Moving and Target Piece Type
    50,   // Moving and Target Piece Type
    -12,  // Moving and Target Piece Type
    -4,   // Moving and Target Piece Type
    -27,  // Moving and Target Piece Type
    25,   // Moving and Target Piece Type
    61,   // Moving and Target Piece Type
    64,   // Moving and Target Piece Type
    13,   // Moving and Target Piece Type
    -14,  // Moving and Target Piece Type
    -27,  // Moving and Target Piece Type
    14,   // Moving and Target Piece Type
    -11,  // Moving and Target Piece Type
    40,   // Moving and Target Piece Type
    4,    // Moving and Target Piece Type
    -81,  // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    -11,  // Moving and Target Piece Type
    38,   // Moving and Target Piece Type
    44,   // Moving and Target Piece Type
    -82,  // Moving and Target Piece Type
    0,    // Move Type
    0,    // Move Type
    -18,  // Move Type
    -134, // Move Type
    254,  // Move Type
    151,  // Move Type
    -1000,// Move Type
    -1000,// Move Type
    395,  // Move Type
    35,   // Move Source
    -3,   // Move Source
    11,   // Move Source
    17,   // Move Source
    4,    // Move Source
    0,    // Move Source
    -1,   // Move Source
    -23,  // Move Source
    -29,  // Move Source
    -32,  // Move Source
    -131, // Move Destination
    -75,  // Move Destination
    -78,  // Move Destination
    -76,  // Move Destination
    -19,  // Move Destination
    0,    // Move Destination
    8,    // Move Destination
    30,   // Move Destination
    35,   // Move Destination
    45,   // Move Destination
    0,    // Knight Move Source
    -6,   // Knight Move Source
    0,    // Knight Move Source
    3,    // Knight Move Source
    0,    // Knight Move Source
    0,    // Knight Move Source
    0,    // Knight Move Source
    0,    // Knight Move Source
    0,    // Knight Move Source
    -4,   // Knight Move Source
    0,    // Knight Move Destination
    -127, // Knight Move Destination
    -105, // Knight Move Destination
    -131, // Knight Move Destination
    -62,  // Knight Move Destination
    -13,  // Knight Move Destination
    0,    // Knight Move Destination
    1,    // Knight Move Destination
    0,    // Knight Move Destination
    45,   // Knight Move Destination
    5,    // Move Captures Last Moved Piece
    -344, // Move SEE
    -369, // Move SEE
    61,   // Move Gives Check
    65,   // Move Gives Check
    -367, // Move Destination Taboo
    0,    // Changes Between Non-/Forcing States
    55,   // Changes Between Non-/Forcing States
    -138, // Changes Between Non-/Forcing States
    0,    // Changes Between Non-/Forcing States
    -24,  // Rank of pawn destination
    40,   // Rank of pawn destination
    40,   // Rank of pawn destination
    0,    // Rank of pawn destination
    0,    // Rank of pawn destination
    0,    // Rank of pawn destination
    -66,  // Rank of passed pawn destination
    56,   // Rank of passed pawn destination
    124,  // Rank of passed pawn destination
    70,   // Rank of passed pawn destination
    129,  // Rank of passed pawn destination
    0,    // Rank of passed pawn destination
    -8,   // Pawn move attacks a piece
    34,   // Piece under attack
    0,    // Piece under attack
    7,    // History
    13,   // CMH
    2     // CMH
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

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
    2000,   //Hash move
    122,    //Killer move
    90,     //Killer move
    101,    //Counter move
    -74,    //Moving and target piece type
    124,    //Moving and target piece type
    30,     //Moving and target piece type
    169,    //Moving and target piece type
    325,    //Moving and target piece type
    3,      //Moving and target piece type
    0,      //Moving and target piece type
    -7,     //Moving and target piece type
    -4,     //Moving and target piece type
    92,     //Moving and target piece type
    311,    //Moving and target piece type
    1,      //Moving and target piece type
    -8,     //Moving and target piece type
    -117,   //Moving and target piece type
    131,    //Moving and target piece type
    90,     //Moving and target piece type
    339,    //Moving and target piece type
    -5,     //Moving and target piece type
    -19,    //Moving and target piece type
    87,     //Moving and target piece type
    123,    //Moving and target piece type
    126,    //Moving and target piece type
    283,    //Moving and target piece type
    -2,     //Moving and target piece type
    -124,   //Moving and target piece type
    -14,    //Moving and target piece type
    -9,     //Moving and target piece type
    28,     //Moving and target piece type
    164,    //Moving and target piece type
    -3,     //Moving and target piece type
    -82,    //Moving and target piece type
    54,     //Moving and target piece type
    43,     //Moving and target piece type
    180,    //Moving and target piece type
    3,      //Moving and target piece type
    -59,    //Moving and target piece type
    -5,     //Move type
    143,    //Move type
    -45,    //Move type
    199,    //Move type
    289,    //Move type
    27,     //Move type
    -1000,  //Move type
    -1000,  //Move type
    400,    //Move type
    36,     //Move source
    15,     //Move source
    16,     //Move source
    21,     //Move source
    -9,     //Move source
    6,      //Move source
    5,      //Move source
    -8,     //Move source
    5,      //Move source
    -13,    //Move source
    -69,    //Move destination
    -45,    //Move destination
    -38,    //Move destination
    -32,    //Move destination
    -4,     //Move destination
    -4,     //Move destination
    9,      //Move destination
    18,     //Move destination
    11,     //Move destination
    29,     //Move destination
    180,    //Knight move source
    81,     //Knight move source
    28,     //Knight move source
    44,     //Knight move source
    42,     //Knight move source
    31,     //Knight move source
    10,     //Knight move source
    2,      //Knight move source
    -7,     //Knight move source
    -17,    //Knight move source
    -231,   //Knight move destination
    -154,   //Knight move destination
    -105,   //Knight move destination
    -59,    //Knight move destination
    -79,    //Knight move destination
    -11,    //Knight move destination
    17,     //Knight move destination
    22,     //Knight move destination
    42,     //Knight move destination
    57,     //Knight move destination
    110,    //Move captures last moved piece
    -389,   //Move SEE
    -231,   //Move SEE
    94,     //Move gives check
    175,    //Move gives check
    -343,   //Move destination is taboo
    -24,    //Changes between non-/forcing states
    18,     //Changes between non-/forcing states
    -15,    //Changes between non-/forcing states
    43,     //Changes between non-/forcing states
    -53,    //Rank of pawn destination
    3,      //Rank of pawn destination
    54,     //Rank of pawn destination
    144,    //Rank of pawn destination
    1,      //Rank of pawn destination
    11,     //Rank of pawn destination
    -57,    //Rank of passed pawn destination
    33,     //Rank of passed pawn destination
    57,     //Rank of passed pawn destination
    59,     //Rank of passed pawn destination
    109,    //Rank of passed pawn destination
    1,      //Rank of passed pawn destination
    124,    //Pawn move attacks piece
    77,     //Piece is under attack
    161,    //Piece is under attack
    7,      //History heuristic
    14,     //Countermove History
    2,      //Countermove History
};

constexpr std::array<int, 117> search_params_in_check = {
    2000,   //Hash move
    55,     //Killer move
    40,     //Killer move
    124,    //Counter move
    -49,    //Moving and target piece type
    -50,    //Moving and target piece type
    -20,    //Moving and target piece type
    30,     //Moving and target piece type
    16,     //Moving and target piece type
    10,     //Moving and target piece type
    -48,    //Moving and target piece type
    -72,    //Moving and target piece type
    0,      //Moving and target piece type
    -18,    //Moving and target piece type
    26,     //Moving and target piece type
    -21,    //Moving and target piece type
    1,      //Moving and target piece type
    11,     //Moving and target piece type
    -6,     //Moving and target piece type
    3,      //Moving and target piece type
    46,     //Moving and target piece type
    -3,     //Moving and target piece type
    -11,    //Moving and target piece type
    -25,    //Moving and target piece type
    19,     //Moving and target piece type
    57,     //Moving and target piece type
    60,     //Moving and target piece type
    7,      //Moving and target piece type
    -17,    //Moving and target piece type
    -24,    //Moving and target piece type
    10,     //Moving and target piece type
    -10,    //Moving and target piece type
    38,     //Moving and target piece type
    3,      //Moving and target piece type
    -84,    //Moving and target piece type
    -5,     //Moving and target piece type
    -20,    //Moving and target piece type
    42,     //Moving and target piece type
    42,     //Moving and target piece type
    -93,    //Moving and target piece type
    -1,     //Move type
    -2,     //Move type
    -18,    //Move type
    -133,   //Move type
    258,    //Move type
    145,    //Move type
    -1000,  //Move type
    -1000,  //Move type
    398,    //Move type
    34,     //Move source
    0,      //Move source
    8,      //Move source
    25,     //Move source
    -2,     //Move source
    -1,     //Move source
    -3,     //Move source
    -23,    //Move source
    -29,    //Move source
    -47,    //Move source
    -140,   //Move destination
    -66,    //Move destination
    -74,    //Move destination
    -84,    //Move destination
    -11,    //Move destination
    0,      //Move destination
    11,     //Move destination
    30,     //Move destination
    37,     //Move destination
    55,     //Move destination
    19,     //Knight move source
    -4,     //Knight move source
    7,      //Knight move source
    4,      //Knight move source
    0,      //Knight move source
    -2,     //Knight move source
    4,      //Knight move source
    14,     //Knight move source
    -4,     //Knight move source
    7,      //Knight move source
    17,     //Knight move destination
    -126,   //Knight move destination
    -110,   //Knight move destination
    -135,   //Knight move destination
    -57,    //Knight move destination
    -13,    //Knight move destination
    6,      //Knight move destination
    1,      //Knight move destination
    0,      //Knight move destination
    37,     //Knight move destination
    0,      //Move captures last moved piece
    -342,   //Move SEE
    -359,   //Move SEE
    73,     //Move gives check
    56,     //Move gives check
    -374,   //Move destination is taboo
    -4,     //Changes between non-/forcing states
    56,     //Changes between non-/forcing states
    -139,   //Changes between non-/forcing states
    1,      //Changes between non-/forcing states
    -29,    //Rank of pawn destination
    34,     //Rank of pawn destination
    36,     //Rank of pawn destination
    -5,     //Rank of pawn destination
    3,      //Rank of pawn destination
    6,      //Rank of pawn destination
    -60,    //Rank of passed pawn destination
    58,     //Rank of passed pawn destination
    117,    //Rank of passed pawn destination
    68,     //Rank of passed pawn destination
    132,    //Rank of passed pawn destination
    2,      //Rank of passed pawn destination
    -10,    //Pawn move attacks piece
    27,     //Piece is under attack
    -11,    //Piece is under attack
    7,      //History heuristic
    13,     //Countermove History
    2,      //Countermove History
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

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
    2000, 
    122, 
    90, 
    101, 
    -74, 
    124, 
    30, 
    169, 
    325, 
    3, 
    0, 
    -7, 
    -4, 
    92, 
    311, 
    1, 
    -8, 
    -117, 
    131, 
    90, 
    339, 
    -5, 
    -19, 
    87, 
    123, 
    126, 
    283, 
    -2, 
    -124, 
    -14, 
    -9, 
    28, 
    164, 
    -3, 
    -82, 
    54, 
    43, 
    180, 
    3, 
    -59, 
    -5, 
    143, 
    -45, 
    199, 
    289, 
    27, 
    -1000, 
    -1000, 
    400, 
    36, 
    15, 
    16, 
    21, 
    -9, 
    6, 
    5, 
    -8, 
    5, 
    -13, 
    -69, 
    -45, 
    -38, 
    -32, 
    -4, 
    -4, 
    9, 
    18, 
    11, 
    29, 
    180, 
    81, 
    28, 
    44, 
    42, 
    31, 
    10, 
    2, 
    -7, 
    -17, 
    -231, 
    -154, 
    -105, 
    -59, 
    -79, 
    -11, 
    17, 
    22, 
    42, 
    57, 
    110, 
    -389, 
    -231, 
    94, 
    175, 
    -343, 
    -24, 
    18, 
    -15, 
    43, 
    -53, 
    3, 
    54, 
    144, 
    1, 
    11, 
    -57, 
    33, 
    57, 
    59, 
    109, 
    1, 
    124, 
    77, 
    161, 
    7, 
    14, 
    2, 
};

constexpr std::array<int, 117> search_params_in_check = {
    2000, 
    55, 
    40, 
    124, 
    -49, 
    -50, 
    -20, 
    30, 
    16, 
    10, 
    -48, 
    -72, 
    0, 
    -18, 
    26, 
    -21, 
    1, 
    11, 
    -6, 
    3, 
    46, 
    -3, 
    -11, 
    -25, 
    19, 
    57, 
    60, 
    7, 
    -17, 
    -24, 
    10, 
    -10, 
    38, 
    3, 
    -84, 
    -5, 
    -20, 
    42, 
    42, 
    -93, 
    -1, 
    -2, 
    -18, 
    -133, 
    258, 
    145, 
    -1000, 
    -1000, 
    398, 
    34, 
    0, 
    8, 
    25, 
    -2, 
    -1, 
    -3, 
    -23, 
    -29, 
    -47, 
    -140, 
    -66, 
    -74, 
    -84, 
    -11, 
    0, 
    11, 
    30, 
    37, 
    55, 
    19, 
    -4, 
    7, 
    4, 
    0, 
    -2, 
    4, 
    14, 
    -4, 
    7, 
    17, 
    -126, 
    -110, 
    -135, 
    -57, 
    -13, 
    6, 
    1, 
    0, 
    37, 
    0, 
    -342, 
    -359, 
    73, 
    56, 
    -374, 
    -4, 
    56, 
    -139, 
    1, 
    -29, 
    34, 
    36, 
    -5, 
    3, 
    6, 
    -60, 
    58, 
    117, 
    68, 
    132, 
    2, 
    -10, 
    27, 
    -11, 
    7, 
    13, 
    2, 
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

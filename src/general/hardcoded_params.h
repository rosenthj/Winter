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
   32000000, // Hash Move
    2154664, // Killer Move
    1576504, // Killer Move
    1708379, // Counter Move
   -1236533, // Moving and Target Piece Type
    1795991, // Moving and Target Piece Type
     592636, // Moving and Target Piece Type
    2671432, // Moving and Target Piece Type
    5251253, // Moving and Target Piece Type
     188710, // Moving and Target Piece Type
    -149365, // Moving and Target Piece Type
    -124028, // Moving and Target Piece Type
      66289, // Moving and Target Piece Type
    1478976, // Moving and Target Piece Type
    4706824, // Moving and Target Piece Type
     217006, // Moving and Target Piece Type
    -278779, // Moving and Target Piece Type
   -1923326, // Moving and Target Piece Type
    2234991, // Moving and Target Piece Type
    1452927, // Moving and Target Piece Type
    5235257, // Moving and Target Piece Type
     -99762, // Moving and Target Piece Type
    -304959, // Moving and Target Piece Type
    1298009, // Moving and Target Piece Type
    2038293, // Moving and Target Piece Type
    1983854, // Moving and Target Piece Type
    4469201, // Moving and Target Piece Type
      79606, // Moving and Target Piece Type
   -1774482, // Moving and Target Piece Type
    -454607, // Moving and Target Piece Type
     -18530, // Moving and Target Piece Type
     553689, // Moving and Target Piece Type
    2797785, // Moving and Target Piece Type
    -268423, // Moving and Target Piece Type
   -1212366, // Moving and Target Piece Type
     782266, // Moving and Target Piece Type
     736684, // Moving and Target Piece Type
    2898685, // Moving and Target Piece Type
     -26558, // Moving and Target Piece Type
    -798559, // Moving and Target Piece Type
      23404, // Move Type
    2428731, // Move Type
    -981233, // Move Type
    3336077, // Move Type
    4687890, // Move Type
     539453, // Move Type
  -15894977, // Move Type
  -15846195, // Move Type
    6383424, // Move Type
     689920, // Move Source
     197408, // Move Source
     301296, // Move Source
     132414, // Move Source
    -295624, // Move Source
     165906, // Move Source
      63817, // Move Source
      -2365, // Move Source
      67238, // Move Source
    -164598, // Move Source
    -817029, // Move Destination
    -524273, // Move Destination
    -488457, // Move Destination
    -521405, // Move Destination
    -223606, // Move Destination
     -30577, // Move Destination
     255134, // Move Destination
     298183, // Move Destination
     172840, // Move Destination
     466062, // Move Destination
    2935824, // Knight Move Source
    1327079, // Knight Move Source
     410792, // Knight Move Source
     808417, // Knight Move Source
     624518, // Knight Move Source
     506504, // Knight Move Source
     238792, // Knight Move Source
     255567, // Knight Move Source
     -93471, // Knight Move Source
    -137412, // Knight Move Source
   -3870739, // Knight Move Destination
   -2560806, // Knight Move Destination
   -1593656, // Knight Move Destination
    -879453, // Knight Move Destination
   -1568866, // Knight Move Destination
    -274619, // Knight Move Destination
     309324, // Knight Move Destination
     167247, // Knight Move Destination
     701341, // Knight Move Destination
     772726, // Knight Move Destination
    1685080, // Move Captures Last Moved Piece
   -6214045, // Move SEE
   -3579836, // Move SEE
    1419783, // Move Gives Check
    2877587, // Move Gives Check
   -5574528, // Move Destination Taboo
    -276844, // Changes Between Non-/Forcing States
      97275, // Changes Between Non-/Forcing States
    -320837, // Changes Between Non-/Forcing States
     592427, // Changes Between Non-/Forcing States
    -822363, // Rank of pawn destination
     108732, // Rank of pawn destination
     996717, // Rank of pawn destination
    2535474, // Rank of pawn destination
      27076, // Rank of pawn destination
     180931, // Rank of pawn destination
    -925314, // Rank of passed pawn destination
     561775, // Rank of passed pawn destination
     711985, // Rank of passed pawn destination
    1059624, // Rank of passed pawn destination
    1971046, // Rank of passed pawn destination
    -282056, // Rank of passed pawn destination
    1994821, // Pawn move attacks a piece
    1229618, // Piece under attack
    2779705, // Piece under attack
        135, // History
        213, // CMH
         62, // CMH
};

constexpr std::array<int, 117> search_params_in_check = {
   32000000, // Hash Move
    1077216, // Killer Move
     704447, // Killer Move
    2157663, // Counter Move
    -627424, // Moving and Target Piece Type
    -916102, // Moving and Target Piece Type
    -502558, // Moving and Target Piece Type
     593352, // Moving and Target Piece Type
     312073, // Moving and Target Piece Type
     -23711, // Moving and Target Piece Type
    -726884, // Moving and Target Piece Type
   -1349972, // Moving and Target Piece Type
    -216148, // Moving and Target Piece Type
    -137757, // Moving and Target Piece Type
     341513, // Moving and Target Piece Type
    -201154, // Moving and Target Piece Type
     -57087, // Moving and Target Piece Type
     259622, // Moving and Target Piece Type
    -390195, // Moving and Target Piece Type
      48208, // Moving and Target Piece Type
     777516, // Moving and Target Piece Type
     103582, // Moving and Target Piece Type
    -138733, // Moving and Target Piece Type
    -624402, // Moving and Target Piece Type
     291374, // Moving and Target Piece Type
     803955, // Moving and Target Piece Type
     841967, // Moving and Target Piece Type
     243762, // Moving and Target Piece Type
    -211747, // Moving and Target Piece Type
    -372031, // Moving and Target Piece Type
     129081, // Moving and Target Piece Type
      26653, // Moving and Target Piece Type
     617770, // Moving and Target Piece Type
    -128124, // Moving and Target Piece Type
   -1352904, // Moving and Target Piece Type
    -142041, // Moving and Target Piece Type
    -271658, // Moving and Target Piece Type
     771954, // Moving and Target Piece Type
     682899, // Moving and Target Piece Type
   -1618050, // Moving and Target Piece Type
     164330, // Move Type
    -164040, // Move Type
    -472388, // Move Type
   -2145379, // Move Type
    3951106, // Move Type
    2219606, // Move Type
  -15827956, // Move Type
  -15936333, // Move Type
    6265340, // Move Type
     634479, // Move Source
     -16913, // Move Source
     -11804, // Move Source
     547391, // Move Source
     -39913, // Move Source
     -28221, // Move Source
    -121327, // Move Source
    -424594, // Move Source
    -522397, // Move Source
    -800967, // Move Source
   -2360436, // Move Destination
   -1053659, // Move Destination
   -1045081, // Move Destination
   -1291775, // Move Destination
    -163142, // Move Destination
    -196645, // Move Destination
     438317, // Move Destination
     415583, // Move Destination
     686006, // Move Destination
     893825, // Move Destination
     326374, // Knight Move Source
     274840, // Knight Move Source
     325637, // Knight Move Source
     316506, // Knight Move Source
     -61307, // Knight Move Source
    -233903, // Knight Move Source
     185440, // Knight Move Source
     177725, // Knight Move Source
      -5379, // Knight Move Source
     -65790, // Knight Move Source
     491073, // Knight Move Destination
   -2196198, // Knight Move Destination
   -1812650, // Knight Move Destination
   -2344976, // Knight Move Destination
    -948039, // Knight Move Destination
    -245370, // Knight Move Destination
     -90455, // Knight Move Destination
      49340, // Knight Move Destination
    -147363, // Knight Move Destination
     539609, // Knight Move Destination
    -163141, // Move Captures Last Moved Piece
   -5504645, // Move SEE
   -5866300, // Move SEE
    1343023, // Move Gives Check
     765795, // Move Gives Check
   -5908332, // Move Destination Taboo
     -66533, // Changes Between Non-/Forcing States
     960262, // Changes Between Non-/Forcing States
   -2132336, // Changes Between Non-/Forcing States
    -195703, // Changes Between Non-/Forcing States
    -550172, // Rank of pawn destination
     621878, // Rank of pawn destination
     579702, // Rank of pawn destination
    -142838, // Rank of pawn destination
     101988, // Rank of pawn destination
     167316, // Rank of pawn destination
    -949352, // Rank of passed pawn destination
     966296, // Rank of passed pawn destination
    1876461, // Rank of passed pawn destination
     946722, // Rank of passed pawn destination
    2006260, // Rank of passed pawn destination
     301236, // Rank of passed pawn destination
     121188, // Pawn move attacks a piece
     547978, // Piece under attack
    -173528, // Piece under attack
         95, // History
        205, // CMH
         19, // CMH
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

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

//First Centroid set to zero vector normalized.
//const std::array<double, 8+(4*4)> NFCM_params = {
//    // Means and Standard Deviations
//    1.4501, 1.34111,
//    1.81476, 1.38448,
//    2.59293, 1.45835,
//    1.10424, 0.930082,
//    // Centroids
//    -1.08127, -1.31079, -1.77798, -1.18725,
//    -0.54272, -0.524388, -0.355711, -0.904568,
//    -0.198028, -0.06039, 0.39646, 0.672892,
//    1.30958, 1.18496, 0.880138, 0.83792
//};

//// First Centroid set to zero vector normalized. Last centroid set to start pos.
//// m = 3
const std::array<double, 8+(4*4)> NFCM_params = {
    // Means and Standard Deviations
    1.4501, 1.34111,
    1.81476, 1.38448,
    2.59293, 1.45835,
    1.10424, 0.930082,
    // Centroids
    -1.08127, -1.31079, -1.77798, -1.18725,
    -0.502385, -0.486815, -0.339176, -0.733863,
    0.00748032, 0.146452, 0.515402, 0.631396,
    1.90134, 1.57839, 0.964835, 0.963098
};

// First Centroid set to zero vector normalized. Last centroid set to start pos.
// m = 3, k = 2
//const std::array<double, 8+(2*4)> NFCM_params = {
//    // Means and Standard Deviations
//    1.4501, 1.34111,
//    1.81476, 1.38448,
//    2.59293, 1.45835,
//    1.10424, 0.930082,
//    // Centroids
//    -1.08127, -1.31079, -1.77798, -1.18725,
//    1.90134, 1.57839, 0.964835, 0.963098
//};

// First Centroid set to zero vector normalized. Last centroid set to start pos.
// m = 3, k = 3
//const std::array<double, 8+(3*4)> NFCM_params = {
//    // Means and Standard Deviations
//    1.4501, 1.34111,
//    1.81476, 1.38448,
//    2.59293, 1.45835,
//    1.10424, 0.930082,
//    // Centroids
//    -1.08127, -1.31079, -1.77798, -1.18725,
//    -0.275169, -0.173044, 0.134105, 0.0741034,
//    1.90134, 1.57839, 0.964835, 0.963098
//};

// First Centroid set to zero vector normalized. Last centroid set to start pos.
// 2 norm
//const std::array<double, 8+(4*4)> NFCM_params = {
//    // Means and Standard Deviations
//    1.4501, 1.34111,
//    1.81476, 1.38448,
//    2.59293, 1.45835,
//    1.10424, 0.930082,
//    // Centroids
//    -1.08127, -1.31079, -1.77798, -1.18725,
//    -0.532835, -0.513867, -0.332504, -0.834612,
//    -0.0106584, 0.15961, 0.552674, 0.716478,
//    1.90134, 1.57839, 0.964835, 0.963098
//};

// First Centroid set to zero vector normalized. Last centroid set to start pos.
// 1 norm
//const std::array<double, 8+(4*4)> NFCM_params = {
//    // Means and Standard Deviations
//    1.4501, 1.34111,
//    1.81476, 1.38448,
//    2.59293, 1.45835,
//    1.10424, 0.930082,
//    // Centroids
//    -1.08127, -1.31079, -1.77798, -1.18725,
//    -0.41084, -0.430795, -0.305968, -0.598237,
//    -0.0201105, 0.0956384, 0.442632, 0.540985,
//    1.90134, 1.57839, 0.964835, 0.963098
//};


//Longstanding
const std::array<double, (4+4*(6+1)*6)> gmm_components = {
    // Component 0 weights
    222663,
    // Component 0 means
    2.40964, 0.977044, 1.31626, 1.54092, 1.8223, 7.09794,
    // Component 0 covariances
    2.96515, 0.0694169, 0.00460904, 0.393509, -0.294552, 0.165724,
    0.0694169, 1.06206, 0.394117, 0.244791, 0.0601271, 0.282553,
    0.00460904, 0.394117, 1.31427, 0.297653, 0.0730756, 0.363438,
    0.393509, 0.244791, 0.297653, 1.08065, 0.0033547, 0.430578,
    -0.294552, 0.0601271, 0.0730756, 0.0033547, 0.151062, 0.120902,
    0.165724, 0.282553, 0.363438, 0.430578, 0.120902, 3.87244,

    // Component 1 weights
    414421,
    // Component 1 means
    1.84333, 0.876377, 1.24353, 1.98172, 1.57393e-05, 5.78166,
    // Component 1 covariances
    2.09752, 0.14954, 0.0481654, -0.18065, 3.4422e-05, 0.0927936,
    0.14954, 0.940539, 0.244203, 0.264519, -1.06067e-05, 0.594929,
    0.0481654, 0.244203, 1.18511, 0.193639, -7.80222e-06, 0.645741,
    -0.18065, 0.264519, 0.193639, 1.84778, -2.11617e-05, 1.16145,
    3.4422e-05, -1.06067e-05, -7.80222e-06, -2.11617e-05, 6.13515e-05, 6.03287e-06,
    0.0927936, 0.594929, 0.645741, 1.16145, 6.03287e-06, 4.82623,

    // Component 2 weights
    227172,
    // Component 2 means
    1.06022, 1.71965, 2.29815, 3.99999, 2.00002, 7.8095,
    // Component 2 covariances
    1.54972, -0.23174, -0.320814, -6.84996e-05, 3.82866e-05, 0.172129,
    -0.23174, 1.17329, 0.0603312, 3.76082e-05, -2.09451e-05, 0.0498102,
    -0.320814, 0.0603312, 1.60519, 3.91054e-05, -2.21072e-05, 0.115985,
    -6.84996e-05, 3.76082e-05, 3.91054e-05, 8.93635e-05, -4.72941e-05, 3.98723e-05,
    3.82866e-05, -2.09451e-05, -2.21072e-05, -4.72941e-05, 2.67607e-05, -2.3214e-05,
    0.172129, 0.0498102, 0.115985, 3.98723e-05, -2.3214e-05, 2.86477,

    // Component 3 weights
    321025,
    // Component 3 means
    0.465111, 3.80207, 3.80207, 3.99998, 2.00001, 7.42735,
    // Component 3 covariances
    0.909343, -0.401743, -0.401727, -5.88459e-05, 3.29333e-05, 0.107059,
    -0.401743, 0.190547, 0.190534, 6.29088e-05, -3.52574e-05, -0.0478098,
    -0.401727, 0.190534, 0.190532, 5.38853e-05, -3.04027e-05, -0.0478155,
    -5.88459e-05, 6.29088e-05, 5.38853e-05, 6.32375e-05, -3.34676e-05, 2.15548e-05,
    3.29333e-05, -3.52574e-05, -3.04027e-05, -3.34676e-05, 1.89371e-05, -1.26776e-05,
    0.107059, -0.0478098, -0.0478155, 2.15548e-05, -1.26776e-05, 0.886092
};

//SL181109
//const std::array<double, (4+4*(6+1)*6)> gmm_components = {
    //// Component 0 weights
    //232312,
    //// Component 0 means
    //1.14705, 3.0032, 3.64416, 3.9962, 1.99397, 7.54282,
    //// Component 0 covariances
    //1.36886, -0.474315, -0.421452, -0.00679908, -0.00599843, -0.0201269,
    //-0.474315, 1.29115, 0.180038, -0.00225386, -0.00218856, 0.0190156,
    //-0.421452, 0.180038, 0.365504, -0.000954343, -0.00082838, -0.00343129,
    //-0.00679908, -0.00225386, -0.000954343, 0.123464, -0.000702406, -0.00233938,
    //-0.00599843, -0.00218856, -0.00082838, -0.000702406, 0.0585644, 0.00605468,
    //-0.0201269, 0.0190156, -0.00343129, -0.00233938, 0.00605468, 1.2366,

    //// Component 1 weights
    //360226,
    //// Component 1 means
    //2.71602, 1.35498, 1.5448, 2.33994, 0.956696, 7.5496,
    //// Component 1 covariances
    //2.85683, -0.236128, -0.0900309, -0.200555, 0.119449, -0.268961,
    //-0.236128, 1.4422, 0.559829, 0.679997, -0.23237, 0.358512,
    //-0.0900309, 0.559829, 1.67375, 0.868948, -0.0611065, 0.665372,
    //-0.200555, 0.679997, 0.868948, 2.25224, -0.0223966, 1.18327,
    //0.119449, -0.23237, -0.0611065, -0.0223966, 0.602407, 0.667716,
    //-0.268961, 0.358512, 0.665372, 1.18327, 0.667716, 5.96681,

    //// Component 2 weights
    //250071,
    //// Component 2 means
    //1.75169, 1.47608, 1.62392, 2.86597, 2.02147, 6.9647,
    //// Component 2 covariances
    //1.98944, -0.00802263, 0.0437159, -0.198428, 0.0106339, -0.0231454,
    //-0.00802263, 1.56616, 0.299455, 0.477873, -0.00421203, 0.285777,
    //0.0437159, 0.299455, 1.29074, 0.206236, -0.00226505, 0.23902,
    //-0.198428, 0.477873, 0.206236, 1.91085, -0.0101433, 0.374712,
    //0.0106339, -0.00421203, -0.00226505, -0.0101433, 0.0451938, -0.000140486,
    //-0.0231454, 0.285777, 0.23902, 0.374712, -0.000140486, 1.23157,

    //// Component 3 weights
    //341882,
    //// Component 3 means
    //1.99046, 0.474488, 0.99657, 1.70535, -0.0163528, 5.77626,
    //// Component 3 covariances
    //1.8794, 0.307235, 0.146055, -0.0676678, -0.00742074, 0.309588,
    //0.307235, 0.477411, 0.0138136, 0.0168707, -0.00562672, 0.200934,
    //0.146055, 0.0138136, 1.0498, 0.00846862, -0.00361105, 0.357167,
    //-0.0676678, 0.0168707, 0.00846862, 1.72611, -0.00360249, 0.797656,
    //-0.00742074, -0.00562672, -0.00361105, -0.00360249, 0.0462161, -0.0168335,
    //0.309588, 0.200934, 0.357167, 0.797656, -0.0168335, 4.68588
//};

//Longstanding
const std::array<int, (174*4)> eval_weights_gmm = {
    -348,  -313,  -168,  30,    // King Piece Square Table
    -90,   -134,  47,    127,   // King Piece Square Table
    -176,  -99,   -100,  11,    // King Piece Square Table
    -269,  -70,   -181,  -77,   // King Piece Square Table
    93,    -33,   207,   195,   // King Piece Square Table
    72,    42,    133,   -51,   // King Piece Square Table
    39,    71,    -61,   -259,  // King Piece Square Table
    189,   135,   220,   -152,  // King Piece Square Table
    201,   173,   57,    -101,  // King Piece Square Table
    320,   225,   -169,  303,   // King Piece Square Table
    2068,  2061,  1346,  1,     // Knight Piece Square Table
    2541,  2227,  1790,  361,   // Knight Piece Square Table
    2550,  2221,  1761,  316,   // Knight Piece Square Table
    2639,  2307,  1848,  392,   // Knight Piece Square Table
    2572,  2289,  1753,  223,   // Knight Piece Square Table
    2548,  2252,  1741,  337,   // Knight Piece Square Table
    2630,  2268,  1850,  413,   // Knight Piece Square Table
    2624,  2271,  1816,  433,   // Knight Piece Square Table
    2650,  2342,  1846,  464,   // Knight Piece Square Table
    2616,  2345,  1795,  421,   // Knight Piece Square Table
    496,   640,   334,   142,   // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    273,   286,   80,    13,    // Bishop Pair
    35,    36,    1,     2,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    59,    41,    41,    18,    // Abstract Piece Activity Bonus
    41,    34,    30,    13,    // Abstract Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    29,    0,     1,     1,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    17,    107,   0,     0,     // Passed Pawn Bonus
    235,   227,   177,   112,   // Passed Pawn Bonus
    507,   313,   460,   316,   // Passed Pawn Bonus
    141,   145,   2,     283,   // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    70,    105,   46,    22,    // Passed Pawn Bonus
    374,   369,   262,   105,   // Passed Pawn Bonus
    777,   674,   602,   390,   // Passed Pawn Bonus
    1369,  1043,  1017,  753,   // Passed Pawn Bonus
    -41,   -106,  -36,   -41,   // Double Pawn Penalty
    28,    4,     28,    34,    // Tempo Bonus
    0,     0,     0,     0,     // Direct Pawn Shield Bonus
    -29,   2,     -19,   4,     // King Vector Exposure
    -37,   -11,   -32,   -5,    // King Vector Exposure
    97,    94,    68,    76,    // Knight Outpost
    0,     0,     0,     0,     // Castling Right
    331,   213,   287,   163,   // Pawn Attack
    260,   142,   189,   135,   // Minor Attack
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    -89,   -1,    -83,   -13,   // King Attack Distance
    0,     0,     0,     0,     // King Attack Distance
    -93,   -24,   -60,   -15,   // King Attack Distance
    -67,   -101,  -20,   0,     // King Attack Distance
    22,    4,     11,    14,    // King Attack Danger
    13,    8,     4,     0,     // King Attack Danger
    23,    15,    28,    18,    // King Attack Danger
    8,     5,     1,     1,     // King Attack Danger
    -708,  -802,  -539,  -236,  // Safe Knight Square Count
    -323,  -345,  -245,  -121,  // Safe Knight Square Count
    -165,  -178,  -138,  -59,   // Safe Knight Square Count
    -71,   -79,   -60,   -24,   // Safe Knight Square Count
    0,     0,     0,     0,     // Safe Knight Square Count
    41,    42,    39,    5,     // Safe Knight Square Count
    69,    44,    68,    7,     // Safe Knight Square Count
    81,    44,    84,    11,    // Safe Knight Square Count
    82,    44,    99,    12,    // Safe Knight Square Count
    1312,  1654,  915,   238,   // Bishop Mobility
    1564,  1839,  1025,  259,   // Bishop Mobility
    1728,  1965,  1156,  341,   // Bishop Mobility
    1853,  2084,  1247,  379,   // Bishop Mobility
    1964,  2169,  1323,  430,   // Bishop Mobility
    2050,  2244,  1385,  461,   // Bishop Mobility
    2132,  2293,  1443,  485,   // Bishop Mobility
    2207,  2349,  1479,  513,   // Bishop Mobility
    2263,  2379,  1528,  517,   // Bishop Mobility
    2323,  2422,  1568,  536,   // Bishop Mobility
    2401,  2431,  1670,  650,   // Bishop Mobility
    2455,  2447,  1855,  665,   // Bishop Mobility
    2539,  2451,  1974,  858,   // Bishop Mobility
    2563,  2463,  2250,  946,   // Bishop Mobility
    2601,  3166,  2704,  2875,  // Rook Mobility
    2922,  3292,  2781,  2934,  // Rook Mobility
    3061,  3370,  2831,  2950,  // Rook Mobility
    3148,  3442,  2888,  3002,  // Rook Mobility
    3245,  3518,  2921,  3011,  // Rook Mobility
    3361,  3597,  2998,  3063,  // Rook Mobility
    3433,  3663,  3061,  3111,  // Rook Mobility
    3517,  3719,  3125,  3159,  // Rook Mobility
    3607,  3784,  3183,  3209,  // Rook Mobility
    3683,  3842,  3238,  3240,  // Rook Mobility
    3746,  3879,  3284,  3291,  // Rook Mobility
    3803,  3917,  3323,  3373,  // Rook Mobility
    3868,  3949,  3382,  3473,  // Rook Mobility
    3944,  3971,  3485,  3504,  // Rook Mobility
    4020,  3972,  3610,  3573,  // Rook Mobility
    6120,  6018,  6088,  6177,  // Queen Mobility
    6247,  6151,  6106,  6226,  // Queen Mobility
    6263,  6152,  6166,  6242,  // Queen Mobility
    6391,  6156,  6192,  6276,  // Queen Mobility
    6424,  6156,  6226,  6289,  // Queen Mobility
    6490,  6206,  6260,  6315,  // Queen Mobility
    6532,  6236,  6288,  6338,  // Queen Mobility
    6576,  6280,  6317,  6353,  // Queen Mobility
    6613,  6305,  6343,  6367,  // Queen Mobility
    6643,  6364,  6358,  6389,  // Queen Mobility
    6667,  6391,  6385,  6393,  // Queen Mobility
    6696,  6470,  6398,  6394,  // Queen Mobility
    6714,  6489,  6412,  6399,  // Queen Mobility
    6731,  6489,  6414,  6400,  // Queen Mobility
    6747,  6493,  6418,  6402,  // Queen Mobility
    6753,  6495,  6419,  6404,  // Queen Mobility
    6754,  6496,  6426,  6407,  // Queen Mobility
    6754,  6497,  6428,  6407,  // Queen Mobility
    6754,  6497,  6437,  6411,  // Queen Mobility
    6755,  6498,  6451,  6421,  // Queen Mobility
    6755,  6499,  6466,  6448,  // Queen Mobility
    6755,  6499,  6473,  6457,  // Queen Mobility
    6755,  6499,  6508,  6458,  // Queen Mobility
    6755,  6500,  6514,  6461,  // Queen Mobility
    6755,  6500,  6556,  6465,  // Queen Mobility
    6755,  6500,  6557,  6466,  // Queen Mobility
    6783,  6500,  6558,  6471,  // Queen Mobility
    6828,  6500,  6558,  6472,  // Queen Mobility
    105,   74,    126,   13,    // Safe Checks
    38,    41,    34,    9,     // Unsafe Checks
    -78,   -17,   -77,   -37,   // Unprotected Pieces
    168,   229,   5,     323,   // Bishop's Position Relative To King
    133,   44,    72,    12,    // Bishop's Position Relative To King
    89,    62,    27,    82,    // Bishop's Position Relative To King
    11,    50,    12,    -19,   // Bishop's Position Relative To King
    28,    112,   -19,   -48,   // Bishop's Position Relative To King
    7,     125,   -57,   -48,   // Bishop's Position Relative To King
    38,    83,    -29,   -53,   // Bishop's Position Relative To King
    -44,   85,    -71,   -135,  // Bishop's Position Relative To King
    118,   150,   96,    104,   // Bishop's Position Relative To King
    39,    47,    21,    -17,   // Bishop's Position Relative To King
    59,    61,    48,    92,    // Bishop's Position Relative To King
    -40,   41,    -71,   -102,  // Bishop's Position Relative To King
    16,    69,    13,    -9,    // Bishop's Position Relative To King
    -12,   43,    -40,   -75,   // Bishop's Position Relative To King
    30,    69,    27,    -9,    // Bishop's Position Relative To King
    133,   116,   94,    110,   // Bishop's Position Relative To King
    9,     39,    3,     -28,   // Bishop's Position Relative To King
    10,    37,    22,    24,    // Bishop's Position Relative To King
    -48,   24,    -44,   -18,   // Bishop's Position Relative To King
    32,    57,    8,     -23,   // Bishop's Position Relative To King
    -11,   24,    -48,   -56,   // Bishop's Position Relative To King
    67,    53,    25,    22,    // Bishop's Position Relative To King
    -20,   16,    -9,    -26,   // Bishop's Position Relative To King
    2,     38,    -13,   -27,   // Bishop's Position Relative To King
    -55,   8,     -94,   -103,  // Bishop's Position Relative To King
    75,    58,    22,    28,    // Bishop's Position Relative To King
    112,   82,    59,    -55,   // Bishop's Position Relative To King
    7,     -5,    4,     -131,  // Bishop's Position Relative To King
    65,    19,    53,    37,    // Bishop's Position Relative To King
    4,     0,     -13,   -87,   // Bishop's Position Relative To King
    681,   199,   547,   437,   // Bishop's Position Relative To King
    127,   -12,   240,   144,   // Bishop's Position Relative To King
    218,   13,    373,   271,   // Bishop's Position Relative To King
    0,     0,     0,     0,     // Bishop's Position Relative To King
    43,    -57,   34,    -58,   // Bishop's Position Relative To King
    0,     0,     0,     0      // Bishop's Position Relative To King
};

//// NCFM 181125, k=3
//const std::array<int, (174*3)> eval_weights = {
//    57,    -589,  -9,    // King Piece Square Table
//    -155,  -59,   47,    // King Piece Square Table
//    135,   -279,  -77,   // King Piece Square Table
//    260,   -384,  -151,  // King Piece Square Table
//    -49,   90,    140,   // King Piece Square Table
//    23,    232,   -38,   // King Piece Square Table
//    42,    332,   -191,  // King Piece Square Table
//    -23,   408,   367,   // King Piece Square Table
//    -42,   536,   403,   // King Piece Square Table
//    -78,   622,   741,   // King Piece Square Table
//    2572,  1882,  27,    // Knight Piece Square Table
//    2397,  2172,  671,   // Knight Piece Square Table
//    2273,  2304,  618,   // Knight Piece Square Table
//    2189,  2507,  691,   // Knight Piece Square Table
//    2381,  2354,  536,   // Knight Piece Square Table
//    2169,  2408,  631,   // Knight Piece Square Table
//    2067,  2519,  722,   // Knight Piece Square Table
//    2069,  2488,  743,   // Knight Piece Square Table
//    2034,  2650,  744,   // Knight Piece Square Table
//    2058,  2599,  707,   // Knight Piece Square Table
//    957,   391,   56,    // Piece Base Value
//    0,     0,     0,     // Piece Base Value
//    0,     0,     0,     // Piece Base Value
//    0,     0,     0,     // Piece Base Value
//    0,     0,     0,     // Piece Base Value
//    745,   112,   0,     // Bishop Pair
//    88,    0,     0,     // Piece Activity Bonus
//    0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     89,    11,    // Abstract Piece Activity Bonus
//    79,    18,    8,     // Abstract Piece Activity Bonus
//    0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     // Passed Pawn Bonus
//    161,   0,     0,     // Passed Pawn Bonus
//    197,   302,   27,    // Passed Pawn Bonus
//    62,    647,   211,   // Passed Pawn Bonus
//    168,   92,    0,     // Passed Pawn Bonus
//    0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     // Passed Pawn Bonus
//    200,   17,    5,     // Passed Pawn Bonus
//    473,   367,   1,     // Passed Pawn Bonus
//    741,   808,   109,   // Passed Pawn Bonus
//    1200,  1118,  430,   // Passed Pawn Bonus
//    -214,  0,     -3,    // Double Pawn Penalty
//    8,     35,    47,    // Tempo Bonus
//    0,     0,     0,     // Direct Pawn Shield Bonus
//    25,    -58,   4,     // King Vector Exposure
//    78,    -99,   -5,    // King Vector Exposure
//    78,    116,   56,    // Knight Outpost
//    0,     0,     0,     // Castling Right
//    2,     434,   173,   // Pawn Attack
//    0,     288,   134,   // Minor Attack
//    0,     0,     0,     // King Defence Distance
//    0,     0,     0,     // King Defence Distance
//    0,     0,     0,     // King Defence Distance
//    0,     0,     0,     // King Defence Distance
//    0,     -75,   -17,   // King Attack Distance
//    0,     0,     0,     // King Attack Distance
//    0,     -72,   -39,   // King Attack Distance
//    -32,   -64,   0,     // King Attack Distance
//    0,     19,    12,    // King Attack Danger
//    0,     10,    0,     // King Attack Danger
//    0,     30,    8,     // King Attack Danger
//    63,    4,     0,     // King Attack Danger
//    -766,  -727,  -178,  // Safe Knight Square Count
//    -291,  -373,  -92,   // Safe Knight Square Count
//    -140,  -217,  -45,   // Safe Knight Square Count
//    -57,   -90,   -13,   // Safe Knight Square Count
//    0,     0,     0,     // Safe Knight Square Count
//    5,     112,   0,     // Safe Knight Square Count
//    6,     174,   2,     // Safe Knight Square Count
//    6,     174,   4,     // Safe Knight Square Count
//    6,     174,   4,     // Safe Knight Square Count
//    1506,  995,   339,   // Bishop Mobility
//    1742,  1163,  354,   // Bishop Mobility
//    1872,  1343,  425,   // Bishop Mobility
//    1942,  1509,  458,   // Bishop Mobility
//    1967,  1665,  492,   // Bishop Mobility
//    1998,  1787,  517,   // Bishop Mobility
//    1998,  1905,  529,   // Bishop Mobility
//    2006,  2012,  531,   // Bishop Mobility
//    2006,  2093,  536,   // Bishop Mobility
//    2006,  2188,  538,   // Bishop Mobility
//    2006,  2192,  625,   // Bishop Mobility
//    2006,  2198,  749,   // Bishop Mobility
//    2006,  2198,  795,   // Bishop Mobility
//    2006,  2200,  832,   // Bishop Mobility
//    4133,  2373,  560,   // Rook Mobility
//    4265,  2467,  598,   // Rook Mobility
//    4356,  2500,  615,   // Rook Mobility
//    4526,  2513,  654,   // Rook Mobility
//    4707,  2527,  655,   // Rook Mobility
//    4774,  2633,  688,   // Rook Mobility
//    4848,  2700,  729,   // Rook Mobility
//    4848,  2803,  769,   // Rook Mobility
//    4909,  2891,  776,   // Rook Mobility
//    4982,  2961,  796,   // Rook Mobility
//    5017,  3019,  823,   // Rook Mobility
//    5059,  3054,  853,   // Rook Mobility
//    5088,  3075,  978,   // Rook Mobility
//    5090,  3092,  1115,  // Rook Mobility
//    5090,  3108,  1245,  // Rook Mobility
//    7227,  5207,  3159,  // Queen Mobility
//    7526,  5402,  3184,  // Queen Mobility
//    7565,  5416,  3208,  // Queen Mobility
//    7575,  5433,  3237,  // Queen Mobility
//    7605,  5487,  3253,  // Queen Mobility
//    7641,  5525,  3278,  // Queen Mobility
//    7677,  5564,  3303,  // Queen Mobility
//    7727,  5600,  3314,  // Queen Mobility
//    7759,  5646,  3319,  // Queen Mobility
//    7820,  5673,  3320,  // Queen Mobility
//    7876,  5692,  3320,  // Queen Mobility
//    7945,  5700,  3319,  // Queen Mobility
//    8000,  5706,  3319,  // Queen Mobility
//    8029,  5714,  3320,  // Queen Mobility
//    8053,  5722,  3320,  // Queen Mobility
//    8053,  5731,  3320,  // Queen Mobility
//    8055,  5732,  3320,  // Queen Mobility
//    8055,  5738,  3333,  // Queen Mobility
//    8055,  5738,  3336,  // Queen Mobility
//    8055,  5738,  3337,  // Queen Mobility
//    8056,  5738,  3337,  // Queen Mobility
//    8056,  5738,  3338,  // Queen Mobility
//    8056,  5738,  3339,  // Queen Mobility
//    8056,  5739,  3339,  // Queen Mobility
//    8056,  5739,  3340,  // Queen Mobility
//    8056,  5742,  3354,  // Queen Mobility
//    8056,  5746,  3507,  // Queen Mobility
//    8121,  5878,  3771,  // Queen Mobility
//    -181,  375,   3,     // Safe Checks
//    -35,   125,   -14,   // Unsafe Checks
//    142,   -162,  -35,   // Unprotected Pieces
//    188,   152,   297,   // Bishop's Position Relative To King
//    81,    361,   135,   // Bishop's Position Relative To King
//    262,   194,   186,   // Bishop's Position Relative To King
//    344,   106,   87,    // Bishop's Position Relative To King
//    426,   140,   15,    // Bishop's Position Relative To King
//    357,   145,   34,    // Bishop's Position Relative To King
//    402,   165,   22,    // Bishop's Position Relative To King
//    293,   212,   -40,   // Bishop's Position Relative To King
//    270,   287,   190,   // Bishop's Position Relative To King
//    251,   221,   80,    // Bishop's Position Relative To King
//    219,   206,   185,   // Bishop's Position Relative To King
//    413,   51,    -3,    // Bishop's Position Relative To King
//    338,   195,   55,    // Bishop's Position Relative To King
//    237,   182,   29,    // Bishop's Position Relative To King
//    264,   259,   52,    // Bishop's Position Relative To King
//    267,   283,   195,   // Bishop's Position Relative To King
//    248,   205,   69,    // Bishop's Position Relative To King
//    271,   172,   112,   // Bishop's Position Relative To King
//    298,   73,    99,    // Bishop's Position Relative To King
//    243,   243,   56,    // Bishop's Position Relative To King
//    196,   199,   43,    // Bishop's Position Relative To King
//    338,   193,   96,    // Bishop's Position Relative To King
//    171,   227,   49,    // Bishop's Position Relative To King
//    244,   205,   57,    // Bishop's Position Relative To King
//    273,   105,   2,     // Bishop's Position Relative To King
//    199,   292,   87,    // Bishop's Position Relative To King
//    125,   446,   16,    // Bishop's Position Relative To King
//    137,   288,   -38,   // Bishop's Position Relative To King
//    180,   261,   83,    // Bishop's Position Relative To King
//    209,   193,   17,    // Bishop's Position Relative To King
//    41,    615,   589,   // Bishop's Position Relative To King
//    -28,   353,   257,   // Bishop's Position Relative To King
//    -98,   402,   483,   // Bishop's Position Relative To King
//    0,     0,     0,     // Bishop's Position Relative To King
//    138,   217,   -59,   // Bishop's Position Relative To King
//    0,     0,     0      // Bishop's Position Relative To King
//};

//NFCM 181123
const std::array<int, (174*4)> eval_weights = {
    551,   -935,  -212,  -48,   // King Piece Square Table
    173,   -442,  159,   17,    // King Piece Square Table
    65,    -71,   -226,  -71,   // King Piece Square Table
    -97,   259,   -483,  -145,  // King Piece Square Table
    49,    -207,  440,   36,    // King Piece Square Table
    -50,   159,   213,   9,     // King Piece Square Table
    -150,  453,   -204,  180,   // King Piece Square Table
    -85,   286,   225,   517,   // King Piece Square Table
    -161,  446,   83,    778,   // King Piece Square Table
    -165,  364,   474,   953,   // King Piece Square Table
    2554,  2596,  1136,  1,     // Knight Piece Square Table
    2285,  2564,  1805,  557,   // Knight Piece Square Table
    2182,  2578,  1875,  508,   // Knight Piece Square Table
    2066,  2734,  2018,  590,   // Knight Piece Square Table
    2238,  2726,  1869,  425,   // Knight Piece Square Table
    1978,  2748,  1885,  545,   // Knight Piece Square Table
    1891,  2732,  2052,  622,   // Knight Piece Square Table
    1956,  2699,  2004,  653,   // Knight Piece Square Table
    1908,  2857,  2062,  651,   // Knight Piece Square Table
    1878,  2902,  1965,  620,   // Knight Piece Square Table
    857,   827,   138,   61,    // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    352,   595,   0,     0,     // Bishop Pair
    106,   11,    0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    0,     64,    65,    12,    // Abstract Piece Activity Bonus
    99,    15,    23,    8,     // Abstract Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    165,   48,    0,     0,     // Passed Pawn Bonus
    171,   363,   149,   25,    // Passed Pawn Bonus
    102,   373,   685,   111,   // Passed Pawn Bonus
    50,    309,   0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    126,   136,   0,     4,     // Passed Pawn Bonus
    453,   487,   215,   8,     // Passed Pawn Bonus
    961,   622,   731,   58,    // Passed Pawn Bonus
    1563,  761,   1242,  338,   // Passed Pawn Bonus
    -299,  -13,   0,     -5,    // Double Pawn Penalty
    10,    0,     53,    44,    // Tempo Bonus
    0,     0,     0,     0,     // Direct Pawn Shield Bonus
    -3,    15,    -61,   4,     // King Vector Exposure
    35,    -8,    -89,   -2,    // King Vector Exposure
    -27,   208,   67,    57,    // Knight Outpost
    0,     0,     0,     0,     // Castling Right
    5,     228,   446,   172,   // Pawn Attack
    1,     27,    392,   129,   // Minor Attack
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     -115,  -7,    // King Attack Distance
    0,     0,     0,     0,     // King Attack Distance
    0,     0,     -124,  -21,   // King Attack Distance
    -168,  -1,    -30,   0,     // King Attack Distance
    0,     0,     27,    10,    // King Attack Danger
    0,     7,     10,    0,     // King Attack Danger
    0,     0,     48,    4,     // King Attack Danger
    7,     61,    0,     0,     // King Attack Danger
    -801,  -839,  -461,  -187,  // Safe Knight Square Count
    -202,  -519,  -227,  -97,   // Safe Knight Square Count
    -154,  -242,  -125,  -48,   // Safe Knight Square Count
    -81,   -76,   -59,   -16,   // Safe Knight Square Count
    0,     0,     0,     0,     // Safe Knight Square Count
    4,     34,    101,   0,     // Safe Knight Square Count
    4,     34,    179,   0,     // Safe Knight Square Count
    4,     34,    208,   0,     // Safe Knight Square Count
    4,     34,    208,   0,     // Safe Knight Square Count
    1349,  1697,  656,   241,   // Bishop Mobility
    1612,  1849,  787,   258,   // Bishop Mobility
    1690,  2017,  938,   336,   // Bishop Mobility
    1712,  2215,  1041,  371,   // Bishop Mobility
    1717,  2369,  1147,  410,   // Bishop Mobility
    1724,  2496,  1222,  434,   // Bishop Mobility
    1725,  2580,  1310,  447,   // Bishop Mobility
    1726,  2689,  1362,  456,   // Bishop Mobility
    1727,  2718,  1444,  456,   // Bishop Mobility
    1727,  2718,  1563,  458,   // Bishop Mobility
    1727,  2718,  1663,  476,   // Bishop Mobility
    1727,  2718,  1731,  606,   // Bishop Mobility
    1727,  2718,  1739,  665,   // Bishop Mobility
    1727,  2719,  1747,  777,   // Bishop Mobility
    3805,  3484,  1850,  218,   // Rook Mobility
    3963,  3620,  1882,  254,   // Rook Mobility
    3979,  3741,  1906,  270,   // Rook Mobility
    4033,  3877,  1928,  307,   // Rook Mobility
    4093,  4024,  1928,  310,   // Rook Mobility
    4311,  4033,  2006,  346,   // Rook Mobility
    4381,  4114,  2056,  384,   // Rook Mobility
    4382,  4163,  2163,  423,   // Rook Mobility
    4529,  4193,  2233,  426,   // Rook Mobility
    4641,  4223,  2295,  454,   // Rook Mobility
    4709,  4245,  2349,  484,   // Rook Mobility
    4736,  4255,  2436,  508,   // Rook Mobility
    4753,  4267,  2469,  670,   // Rook Mobility
    4753,  4267,  2512,  849,   // Rook Mobility
    4753,  4270,  2555,  881,   // Rook Mobility
    7072,  7541,  3577,  2901,  // Queen Mobility
    7347,  7784,  3725,  2946,  // Queen Mobility
    7404,  7827,  3741,  2963,  // Queen Mobility
    7411,  7831,  3746,  2994,  // Queen Mobility
    7441,  7856,  3782,  3013,  // Queen Mobility
    7505,  7908,  3799,  3038,  // Queen Mobility
    7544,  7955,  3827,  3063,  // Queen Mobility
    7572,  8025,  3844,  3078,  // Queen Mobility
    7587,  8069,  3872,  3081,  // Queen Mobility
    7622,  8148,  3888,  3081,  // Queen Mobility
    7648,  8213,  3891,  3081,  // Queen Mobility
    7697,  8278,  3894,  3081,  // Queen Mobility
    7740,  8302,  3897,  3081,  // Queen Mobility
    7782,  8322,  3898,  3081,  // Queen Mobility
    7806,  8335,  3899,  3082,  // Queen Mobility
    7811,  8336,  3906,  3082,  // Queen Mobility
    7813,  8337,  3906,  3084,  // Queen Mobility
    7813,  8338,  3910,  3091,  // Queen Mobility
    7813,  8338,  3910,  3092,  // Queen Mobility
    7813,  8338,  3910,  3092,  // Queen Mobility
    7813,  8338,  3910,  3092,  // Queen Mobility
    7813,  8338,  3910,  3092,  // Queen Mobility
    7813,  8338,  3910,  3092,  // Queen Mobility
    7813,  8338,  3910,  3092,  // Queen Mobility
    7813,  8338,  3910,  3092,  // Queen Mobility
    7813,  8338,  3914,  3101,  // Queen Mobility
    7814,  8338,  3991,  3285,  // Queen Mobility
    7877,  8382,  4094,  3388,  // Queen Mobility
    -274,  200,   359,   3,     // Safe Checks
    -109,  159,   59,    -6,    // Unsafe Checks
    26,    49,    -168,  -28,   // Unprotected Pieces
    253,   239,   172,   304,   // Bishop's Position Relative To King
    98,    290,   294,   99,    // Bishop's Position Relative To King
    224,   294,   187,   177,   // Bishop's Position Relative To King
    176,   237,   171,   73,    // Bishop's Position Relative To King
    324,   352,   45,    26,    // Bishop's Position Relative To King
    367,   392,   -39,   29,    // Bishop's Position Relative To King
    229,   407,   87,    17,    // Bishop's Position Relative To King
    296,   385,   38,    -22,   // Bishop's Position Relative To King
    212,   265,   301,   164,   // Bishop's Position Relative To King
    96,    350,   191,   67,    // Bishop's Position Relative To King
    206,   281,   193,   169,   // Bishop's Position Relative To King
    234,   339,   3,     -9,    // Bishop's Position Relative To King
    276,   295,   159,   50,    // Bishop's Position Relative To King
    266,   279,   72,    30,    // Bishop's Position Relative To King
    270,   241,   217,   44,    // Bishop's Position Relative To King
    273,   318,   273,   174,   // Bishop's Position Relative To King
    163,   344,   130,   62,    // Bishop's Position Relative To King
    244,   284,   147,   93,    // Bishop's Position Relative To King
    202,   301,   33,    85,    // Bishop's Position Relative To King
    342,   217,   184,   51,    // Bishop's Position Relative To King
    275,   234,   91,    32,    // Bishop's Position Relative To King
    257,   348,   135,   85,    // Bishop's Position Relative To King
    120,   313,   143,   32,    // Bishop's Position Relative To King
    282,   239,   147,   44,    // Bishop's Position Relative To King
    222,   290,   2,     -3,    // Bishop's Position Relative To King
    279,   200,   250,   72,    // Bishop's Position Relative To King
    173,   330,   331,   26,    // Bishop's Position Relative To King
    103,   282,   210,   -31,   // Bishop's Position Relative To King
    265,   145,   303,   30,    // Bishop's Position Relative To King
    146,   310,   109,   11,    // Bishop's Position Relative To King
    18,    349,   843,   558,   // Bishop's Position Relative To King
    -64,   140,   621,   160,   // Bishop's Position Relative To King
    -15,   22,    817,   236,   // Bishop's Position Relative To King
    0,     0,     0,     0,     // Bishop's Position Relative To King
    19,    247,   290,   -166,  // Bishop's Position Relative To King
    0,     0,     0,     0      // Bishop's Position Relative To King
};


const std::array<int, 114> search_params = {
    2000, // Hash Move
    142,  // Killer Move
    112,  // Killer Move
    92,   // Counter Move
    83,   // Moving and Target Piece Type
    271,  // Moving and Target Piece Type
    265,  // Moving and Target Piece Type
    377,  // Moving and Target Piece Type
    673,  // Moving and Target Piece Type
    140,  // Moving and Target Piece Type
    170,  // Moving and Target Piece Type
    156,  // Moving and Target Piece Type
    116,  // Moving and Target Piece Type
    313,  // Moving and Target Piece Type
    546,  // Moving and Target Piece Type
    66,   // Moving and Target Piece Type
    155,  // Moving and Target Piece Type
    37,   // Moving and Target Piece Type
    209,  // Moving and Target Piece Type
    283,  // Moving and Target Piece Type
    556,  // Moving and Target Piece Type
    37,   // Moving and Target Piece Type
    110,  // Moving and Target Piece Type
    255,  // Moving and Target Piece Type
    267,  // Moving and Target Piece Type
    248,  // Moving and Target Piece Type
    605,  // Moving and Target Piece Type
    33,   // Moving and Target Piece Type
    4,    // Moving and Target Piece Type
    98,   // Moving and Target Piece Type
    145,  // Moving and Target Piece Type
    164,  // Moving and Target Piece Type
    312,  // Moving and Target Piece Type
    41,   // Moving and Target Piece Type
    74,   // Moving and Target Piece Type
    218,  // Moving and Target Piece Type
    238,  // Moving and Target Piece Type
    406,  // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    -318, // Move Type
    -185, // Move Type
    -352, // Move Type
    14,   // Move Type
    -74,  // Move Type
    -311, // Move Type
    -1000,// Move Type
    -1000,// Move Type
    0,    // Move Type
    57,   // Move Source
    32,   // Move Source
    44,   // Move Source
    31,   // Move Source
    3,    // Move Source
    9,    // Move Source
    13,   // Move Source
    8,    // Move Source
    3,    // Move Source
    0,    // Move Source
    -135, // Move Destination
    -86,  // Move Destination
    -92,  // Move Destination
    -78,  // Move Destination
    -35,  // Move Destination
    -37,  // Move Destination
    -37,  // Move Destination
    -25,  // Move Destination
    -18,  // Move Destination
    0,    // Move Destination
    175,  // Knight Move Source
    124,  // Knight Move Source
    85,   // Knight Move Source
    71,   // Knight Move Source
    76,   // Knight Move Source
    49,   // Knight Move Source
    42,   // Knight Move Source
    21,   // Knight Move Source
    -4,   // Knight Move Source
    0,    // Knight Move Source
    -369, // Knight Move Destination
    -259, // Knight Move Destination
    -191, // Knight Move Destination
    -149, // Knight Move Destination
    -150, // Knight Move Destination
    -90,  // Knight Move Destination
    -76,  // Knight Move Destination
    -57,  // Knight Move Destination
    -14,  // Knight Move Destination
    0,    // Knight Move Destination
    66,   // Move Captures Last Moved Piece
    -426, // Move SEE
    -232, // Move SEE
    82,   // Move Gives Check
    160,  // Move Gives Check
    -369, // Move Destination Taboo
    39,   // Changes Between Non-/Forcing States
    -39,  // Changes Between Non-/Forcing States
    0,    // Changes Between Non-/Forcing States
    0,    // Changes Between Non-/Forcing States
    -167, // Rank of pawn destination
    -143, // Rank of pawn destination
    -74,  // Rank of pawn destination
    11,   // Rank of pawn destination
    0,    // Rank of pawn destination
    0,    // Rank of pawn destination
    -186, // Rank of passed pawn destination
    -97,  // Rank of passed pawn destination
    -56,  // Rank of passed pawn destination
    -30,  // Rank of passed pawn destination
    0,    // Rank of passed pawn destination
    0,    // Rank of passed pawn destination
    128,  // Pawn move attacks a piece
    131,  // Piece under attack
    206   // Piece under attack
};

const std::array<int, 114> search_params_in_check = {
    2000, // Hash Move
    123,  // Killer Move
    116,  // Killer Move
    188,  // Counter Move
    -135, // Moving and Target Piece Type
    -55,  // Moving and Target Piece Type
    -2,   // Moving and Target Piece Type
    143,  // Moving and Target Piece Type
    262,  // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    -97,  // Moving and Target Piece Type
    -53,  // Moving and Target Piece Type
    4,    // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    160,  // Moving and Target Piece Type
    -16,  // Moving and Target Piece Type
    -68,  // Moving and Target Piece Type
    -22,  // Moving and Target Piece Type
    88,   // Moving and Target Piece Type
    35,   // Moving and Target Piece Type
    183,  // Moving and Target Piece Type
    -8,   // Moving and Target Piece Type
    -80,  // Moving and Target Piece Type
    17,   // Moving and Target Piece Type
    66,   // Moving and Target Piece Type
    112,  // Moving and Target Piece Type
    183,  // Moving and Target Piece Type
    2,    // Moving and Target Piece Type
    -128, // Moving and Target Piece Type
    -22,  // Moving and Target Piece Type
    38,   // Moving and Target Piece Type
    -28,  // Moving and Target Piece Type
    237,  // Moving and Target Piece Type
    6,    // Moving and Target Piece Type
    -198, // Moving and Target Piece Type
    -107, // Moving and Target Piece Type
    -49,  // Moving and Target Piece Type
    64,   // Moving and Target Piece Type
    192,  // Moving and Target Piece Type
    -70,  // Moving and Target Piece Type
    -75,  // Move Type
    0,    // Move Type
    -59,  // Move Type
    76,   // Move Type
    137,  // Move Type
    -15,  // Move Type
    -1000,// Move Type
    -1000,// Move Type
    0,    // Move Type
    53,   // Move Source
    21,   // Move Source
    27,   // Move Source
    40,   // Move Source
    -11,  // Move Source
    -3,   // Move Source
    0,    // Move Source
    -26,  // Move Source
    -35,  // Move Source
    -48,  // Move Source
    -99,  // Move Destination
    -37,  // Move Destination
    -70,  // Move Destination
    -88,  // Move Destination
    -1,   // Move Destination
    11,   // Move Destination
    0,    // Move Destination
    40,   // Move Destination
    48,   // Move Destination
    56,   // Move Destination
    12,   // Knight Move Source
    34,   // Knight Move Source
    17,   // Knight Move Source
    0,    // Knight Move Source
    0,    // Knight Move Source
    -1,   // Knight Move Source
    0,    // Knight Move Source
    -9,   // Knight Move Source
    -49,  // Knight Move Source
    -12,  // Knight Move Source
    0,    // Knight Move Destination
    -79,  // Knight Move Destination
    -79,  // Knight Move Destination
    -116, // Knight Move Destination
    7,    // Knight Move Destination
    0,    // Knight Move Destination
    -15,  // Knight Move Destination
    0,    // Knight Move Destination
    0,    // Knight Move Destination
    7,    // Knight Move Destination
    78,   // Move Captures Last Moved Piece
    -499, // Move SEE
    -419, // Move SEE
    59,   // Move Gives Check
    76,   // Move Gives Check
    -418, // Move Destination Taboo
    0,    // Changes Between Non-/Forcing States
    104,  // Changes Between Non-/Forcing States
    -208, // Changes Between Non-/Forcing States
    0,    // Changes Between Non-/Forcing States
    -19,  // Rank of pawn destination
    38,   // Rank of pawn destination
    54,   // Rank of pawn destination
    166,  // Rank of pawn destination
    0,    // Rank of pawn destination
    0,    // Rank of pawn destination
    -1,   // Rank of passed pawn destination
    35,   // Rank of passed pawn destination
    73,   // Rank of passed pawn destination
    249,  // Rank of passed pawn destination
    126,  // Rank of passed pawn destination
    0,    // Rank of passed pawn destination
    -15,  // Pawn move attacks a piece
    75,   // Piece under attack
    11    // Piece under attack
};

}



#endif /* SRC_GENERAL_HARDCODED_PARAMS_H_ */

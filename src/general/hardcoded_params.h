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


// 181227, mix 0.3, best
//const std::array<int, (270*4)> eval_weights = {
//    -371,  -604,  -18,   100,   // King Piece Square Table
//    -252,  -328,  204,   304,   // King Piece Square Table
//    -33,   -31,   -262,  5,     // King Piece Square Table
//    -59,   84,    -510,  -43,   // King Piece Square Table
//    -31,   -113,  473,   -69,   // King Piece Square Table
//    98,    149,   213,   -186,  // King Piece Square Table
//    109,   360,   -218,  -287,  // King Piece Square Table
//    176,   221,   279,   -40,   // King Piece Square Table
//    191,   307,   131,   -165,  // King Piece Square Table
//    273,   266,   0,     90,    // King Piece Square Table
//    2247,  2427,  1610,  3303,  // Knight Piece Square Table
//    2112,  2497,  2293,  3586,  // Knight Piece Square Table
//    2201,  2638,  2182,  3551,  // Knight Piece Square Table
//    2106,  2702,  2452,  3620,  // Knight Piece Square Table
//    2272,  2705,  2195,  3433,  // Knight Piece Square Table
//    2115,  2743,  2391,  3721,  // Knight Piece Square Table
//    2229,  2744,  2425,  3767,  // Knight Piece Square Table
//    2189,  2801,  2486,  3799,  // Knight Piece Square Table
//    2187,  2915,  2513,  3735,  // Knight Piece Square Table
//    2203,  2995,  2413,  3753,  // Knight Piece Square Table
//    941,   580,   102,   392,   // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    447,   189,   130,   962,   // Bishop Pair
//    137,   0,     0,     25,    // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     75,    53,    32,    // Abstract Piece Activity Bonus
//    51,    28,    21,    6,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    -60,   0,     0,     0,     // Double Pawn Penalty
//    100,   100,   100,   100,   // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    23,    32,    -112,  -37,   // King Vector Exposure
//    -86,   59,    -93,   -21,   // King Vector Exposure
//    -46,   294,   58,    171,   // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    0,     104,   701,   464,   // Pawn Attack
//    0,     15,    548,   361,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     -65,   0,     // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    0,     0,     -69,   0,     // King Attack Distance
//    -106,  -123,  -28,   0,     // King Attack Distance
//    0,     0,     16,    9,     // King Attack Danger
//    0,     0,     22,    6,     // King Attack Danger
//    0,     0,     61,    26,    // King Attack Danger
//    0,     142,   22,    0,     // King Attack Danger
//    -503,  -481,  -463,  -287,  // Safe Knight Square Count
//    -50,   -89,   -397,  -160,  // Safe Knight Square Count
//    -7,    -84,   -172,  -97,   // Safe Knight Square Count
//    0,     -18,   -100,  -58,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    20,    4,     103,   38,    // Safe Knight Square Count
//    20,    4,     155,   71,    // Safe Knight Square Count
//    20,    4,     172,   92,    // Safe Knight Square Count
//    20,    4,     172,   92,    // Safe Knight Square Count
//    1491,  1980,  1329,  1998,  // Bishop Mobility
//    1779,  2055,  1585,  2147,  // Bishop Mobility
//    1952,  2119,  1803,  2232,  // Bishop Mobility
//    2000,  2211,  1931,  2314,  // Bishop Mobility
//    2091,  2296,  1992,  2375,  // Bishop Mobility
//    2091,  2383,  2099,  2422,  // Bishop Mobility
//    2091,  2465,  2202,  2466,  // Bishop Mobility
//    2094,  2570,  2209,  2512,  // Bishop Mobility
//    2106,  2623,  2300,  2530,  // Bishop Mobility
//    2117,  2635,  2404,  2548,  // Bishop Mobility
//    2119,  2636,  2404,  2667,  // Bishop Mobility
//    2121,  2650,  2520,  2794,  // Bishop Mobility
//    2256,  2651,  2520,  2818,  // Bishop Mobility
//    2279,  2674,  2548,  2843,  // Bishop Mobility
//    4133,  4073,  2546,  3835,  // Rook Mobility
//    4336,  4279,  2688,  3931,  // Rook Mobility
//    4482,  4495,  2728,  3971,  // Rook Mobility
//    4651,  4571,  2747,  4013,  // Rook Mobility
//    4769,  4634,  2749,  4026,  // Rook Mobility
//    4939,  4640,  2852,  4087,  // Rook Mobility
//    4994,  4707,  2927,  4142,  // Rook Mobility
//    5004,  4707,  3034,  4177,  // Rook Mobility
//    5033,  4715,  3169,  4253,  // Rook Mobility
//    5068,  4759,  3242,  4260,  // Rook Mobility
//    5083,  4787,  3289,  4401,  // Rook Mobility
//    5162,  4789,  3370,  4417,  // Rook Mobility
//    5271,  4791,  3431,  4455,  // Rook Mobility
//    5271,  4795,  3449,  4584,  // Rook Mobility
//    5271,  4846,  3451,  4586,  // Rook Mobility
//    8820,  8534,  7346,  6586,  // Queen Mobility
//    8986,  8716,  7406,  6659,  // Queen Mobility
//    9022,  8750,  7417,  6738,  // Queen Mobility
//    9025,  8752,  7417,  6738,  // Queen Mobility
//    9107,  8829,  7431,  6776,  // Queen Mobility
//    9143,  8915,  7436,  6794,  // Queen Mobility
//    9162,  8935,  7448,  6794,  // Queen Mobility
//    9194,  8976,  7451,  6794,  // Queen Mobility
//    9242,  9012,  7451,  6794,  // Queen Mobility
//    9282,  9059,  7453,  6794,  // Queen Mobility
//    9331,  9082,  7454,  6794,  // Queen Mobility
//    9366,  9094,  7455,  6794,  // Queen Mobility
//    9366,  9098,  7455,  6794,  // Queen Mobility
//    9455,  9169,  7455,  6794,  // Queen Mobility
//    9458,  9172,  7455,  6794,  // Queen Mobility
//    9472,  9204,  7455,  6794,  // Queen Mobility
//    9510,  9257,  7472,  6794,  // Queen Mobility
//    9510,  9257,  7483,  6800,  // Queen Mobility
//    9513,  9259,  7509,  6841,  // Queen Mobility
//    9513,  9259,  7534,  6991,  // Queen Mobility
//    9513,  9259,  7537,  6994,  // Queen Mobility
//    9513,  9259,  7544,  7041,  // Queen Mobility
//    9513,  9259,  7544,  7041,  // Queen Mobility
//    9513,  9259,  7544,  7042,  // Queen Mobility
//    9513,  9259,  7544,  7042,  // Queen Mobility
//    9513,  9259,  7544,  7042,  // Queen Mobility
//    9513,  9259,  7548,  7059,  // Queen Mobility
//    9513,  9259,  7548,  7060,  // Queen Mobility
//    -333,  315,   599,   59,    // Safe Checks
//    -151,  71,    236,   -2,    // Unsafe Checks
//    195,   -9,    -297,  -53,   // Unprotected Pieces
//    352,   349,   293,   304,   // Bishop's Position Relative To King
//    356,   335,   220,   209,   // Bishop's Position Relative To King
//    355,   512,   304,   155,   // Bishop's Position Relative To King
//    307,   384,   251,   174,   // Bishop's Position Relative To King
//    583,   364,   115,   260,   // Bishop's Position Relative To King
//    473,   385,   73,    141,   // Bishop's Position Relative To King
//    723,   583,   -79,   80,    // Bishop's Position Relative To King
//    489,   407,   43,    16,    // Bishop's Position Relative To King
//    464,   507,   462,   358,   // Bishop's Position Relative To King
//    485,   279,   257,   255,   // Bishop's Position Relative To King
//    503,   313,   304,   307,   // Bishop's Position Relative To King
//    505,   324,   104,   185,   // Bishop's Position Relative To King
//    588,   366,   205,   267,   // Bishop's Position Relative To King
//    454,   311,   154,   189,   // Bishop's Position Relative To King
//    447,   396,   352,   266,   // Bishop's Position Relative To King
//    569,   580,   335,   368,   // Bishop's Position Relative To King
//    428,   376,   218,   186,   // Bishop's Position Relative To King
//    467,   462,   249,   208,   // Bishop's Position Relative To King
//    542,   260,   172,   162,   // Bishop's Position Relative To King
//    531,   336,   360,   177,   // Bishop's Position Relative To King
//    466,   195,   390,   133,   // Bishop's Position Relative To King
//    645,   588,   366,   169,   // Bishop's Position Relative To King
//    304,   410,   273,   123,   // Bishop's Position Relative To King
//    490,   375,   311,   134,   // Bishop's Position Relative To King
//    417,   352,   113,   84,    // Bishop's Position Relative To King
//    412,   487,   270,   147,   // Bishop's Position Relative To King
//    523,   759,   249,   165,   // Bishop's Position Relative To King
//    233,   409,   317,   72,    // Bishop's Position Relative To King
//    435,   315,   386,   54,    // Bishop's Position Relative To King
//    195,   328,   313,   114,   // Bishop's Position Relative To King
//    398,   651,   994,   402,   // Bishop's Position Relative To King
//    10,    299,   597,   199,   // Bishop's Position Relative To King
//    298,   367,   388,   253,   // Bishop's Position Relative To King
//    722,   690,   868,   809,   // Bishop's Position Relative To King
//    13,    271,   462,   -211,  // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -74,   56,    174,   -1,    // Opposed Pawns
//    -7,    7,     39,    -70,   // Opposed Pawns
//    -66,   38,    94,    17,    // Opposed Pawns
//    -11,   177,   21,    98,    // Opposed Pawns
//    -180,  397,   447,   244,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    49,    162,   189,   105,   // Opposed Pawns
//    7,     173,   157,   151,   // Opposed Pawns
//    150,   252,   107,   303,   // Opposed Pawns
//    323,   789,   464,   206,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -28,   -16,   242,   59,    // Opposed Pawns
//    -36,   27,    98,    52,    // Opposed Pawns
//    12,    75,    130,   168,   // Opposed Pawns
//    135,   113,   316,   191,   // Opposed Pawns
//    569,   606,   587,   578,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    110,   234,   236,   125,   // Opposed Pawns
//    -20,   201,   312,   288,   // Opposed Pawns
//    323,   322,   492,   336,   // Opposed Pawns
//    466,   463,   460,   458,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -131,  35,    167,   126,   // Unopposed Pawns
//    -143,  87,    -1,    -25,   // Unopposed Pawns
//    -116,  77,    42,    317,   // Unopposed Pawns
//    -45,   171,   328,   282,   // Unopposed Pawns
//    436,   532,   746,   615,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -80,   270,   264,   325,   // Unopposed Pawns
//    63,    240,   265,   584,   // Unopposed Pawns
//    324,   435,   452,   588,   // Unopposed Pawns
//    1065,  905,   842,   835,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -76,   46,    250,   247,   // Unopposed Pawns
//    44,    145,   58,    242,   // Unopposed Pawns
//    213,   160,   204,   455,   // Unopposed Pawns
//    549,   390,   611,   475,   // Unopposed Pawns
//    729,   816,   803,   787,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    107,   332,   352,   369,   // Unopposed Pawns
//    390,   539,   306,   567,   // Unopposed Pawns
//    859,   603,   596,   608,   // Unopposed Pawns
//    188,   188,   187,   187,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -42,   126,   276,   62,    // Covered Passed Pawns
//    -37,   221,   -28,   118,   // Covered Passed Pawns
//    175,   370,   -45,   185,   // Covered Passed Pawns
//    168,   570,   218,   400,   // Covered Passed Pawns
//    -46,   778,   1202,  430,   // Covered Passed Pawns
//    -335,  368,   1387,  1466,  // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    -86,   316,   553,   238,   // Covered Passed Pawns
//    296,   655,   66,    432,   // Covered Passed Pawns
//    466,   947,   296,   466,   // Covered Passed Pawns
//    750,   1360,  1248,  1163,  // Covered Passed Pawns
//    1004,  984,   929,   918,   // Covered Passed Pawns
//    -120,  125,   394,   166,   // Covered Passed Pawns
//    91,    222,   135,   212,   // Covered Passed Pawns
//    320,   679,   53,    174,   // Covered Passed Pawns
//    563,   1016,  487,   571,   // Covered Passed Pawns
//    1329,  1365,  1278,  1280,  // Covered Passed Pawns
//    84,    84,    84,    84,    // Covered Passed Pawns
//    0,     125,   394,   166,   // Covered Passed Pawns
//    91,    316,   553,   238,   // Covered Passed Pawns
//    320,   679,   66,    432,   // Covered Passed Pawns
//    563,   1016,  487,   571,   // Covered Passed Pawns
//    1329,  1365,  1278,  1280,  // Covered Passed Pawns
//    1004,  984,   929,   918,   // Covered Passed Pawns
//    278,   104,   -220,  -103,  // Uncovered Passed Pawns
//    369,   88,    -33,   -46,   // Uncovered Passed Pawns
//    1077,  244,   -283,  85,    // Uncovered Passed Pawns
//    1869,  497,   -191,  116,   // Uncovered Passed Pawns
//    3282,  772,   -27,   96,    // Uncovered Passed Pawns
//    3711,  1523,  320,   732,   // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    124,   328,   88,    47,    // Uncovered Passed Pawns
//    822,   436,   72,    354,   // Uncovered Passed Pawns
//    1623,  971,   77,    80,    // Uncovered Passed Pawns
//    2369,  1647,  865,   976,   // Uncovered Passed Pawns
//    2284,  2508,  2382,  2363,  // Uncovered Passed Pawns
//    251,   138,   -87,   -157,  // Uncovered Passed Pawns
//    4,     138,   121,   120,   // Uncovered Passed Pawns
//    809,   493,   156,   332,   // Uncovered Passed Pawns
//    1303,  1004,  765,   877,   // Uncovered Passed Pawns
//    2108,  1898,  1749,  1794,  // Uncovered Passed Pawns
//    2454,  2386,  2386,  2383,  // Uncovered Passed Pawns
//    251,   138,   0,     0,     // Uncovered Passed Pawns
//    124,   328,   121,   120,   // Uncovered Passed Pawns
//    822,   493,   156,   354,   // Uncovered Passed Pawns
//    1623,  1004,  765,   877,   // Uncovered Passed Pawns
//    2369,  1898,  1749,  1794,  // Uncovered Passed Pawns
//    2454,  2508,  2386,  2383   // Uncovered Passed Pawns
//};

// 181228, mix 0.5
const std::array<int, (270*4)> eval_weights = {
    -441,  -671,  -53,   88,    // King Piece Square Table
    -291,  -391,  206,   296,   // King Piece Square Table
    -54,   -44,   -282,  -56,   // King Piece Square Table
    -77,   63,    -524,  -81,   // King Piece Square Table
    -55,   -132,  464,   -128,  // King Piece Square Table
    98,    155,   228,   -339,  // King Piece Square Table
    110,   397,   -214,  -461,  // King Piece Square Table
    186,   242,   238,   -82,   // King Piece Square Table
    214,   310,   153,   -197,  // King Piece Square Table
    329,   249,   8,     102,   // King Piece Square Table
    2242,  2411,  1840,  3918,  // Knight Piece Square Table
    2075,  2533,  2673,  4249,  // Knight Piece Square Table
    2215,  2649,  2540,  4213,  // Knight Piece Square Table
    2127,  2723,  2825,  4286,  // Knight Piece Square Table
    2294,  2750,  2549,  4057,  // Knight Piece Square Table
    2143,  2780,  2742,  4396,  // Knight Piece Square Table
    2298,  2772,  2778,  4454,  // Knight Piece Square Table
    2268,  2834,  2843,  4482,  // Knight Piece Square Table
    2263,  2940,  2903,  4425,  // Knight Piece Square Table
    2264,  3041,  2797,  4438,  // Knight Piece Square Table
    960,   634,   183,   465,   // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    353,   188,   139,   1200,  // Bishop Pair
    139,   3,     0,     30,    // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    0,     78,    61,    33,    // Abstract Piece Activity Bonus
    58,    30,    25,    6,     // Abstract Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    0,     0,     0,     0,     // Passed Pawn Bonus
    -94,   0,     0,     0,     // Double Pawn Penalty
    100,   100,   100,   100,   // Tempo Bonus
    0,     0,     0,     0,     // Direct Pawn Shield Bonus
    22,    44,    -127,  -43,   // King Vector Exposure
    -46,   47,    -111,  -24,   // King Vector Exposure
    -22,   336,   48,    192,   // Knight Outpost
    0,     0,     0,     0,     // Castling Right
    0,     74,    789,   519,   // Pawn Attack
    0,     2,     572,   400,   // Minor Attack
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     -63,   0,     // King Attack Distance
    0,     0,     0,     0,     // King Attack Distance
    0,     0,     -78,   0,     // King Attack Distance
    -162,  -114,  -4,    0,     // King Attack Distance
    0,     0,     17,    6,     // King Attack Danger
    0,     0,     20,    8,     // King Attack Danger
    0,     0,     58,    36,    // King Attack Danger
    0,     115,   48,    0,     // King Attack Danger
    -606,  -604,  -532,  -323,  // Safe Knight Square Count
    -64,   -79,   -465,  -186,  // Safe Knight Square Count
    -1,    -73,   -220,  -114,  // Safe Knight Square Count
    0,     -7,    -129,  -69,   // Safe Knight Square Count
    0,     0,     0,     0,     // Safe Knight Square Count
    3,     0,     122,   42,    // Safe Knight Square Count
    3,     0,     172,   86,    // Safe Knight Square Count
    3,     0,     172,   91,    // Safe Knight Square Count
    3,     0,     172,   91,    // Safe Knight Square Count
    1579,  2035,  1573,  2390,  // Bishop Mobility
    1907,  2115,  1843,  2551,  // Bishop Mobility
    2095,  2188,  2086,  2648,  // Bishop Mobility
    2142,  2290,  2239,  2739,  // Bishop Mobility
    2249,  2389,  2298,  2808,  // Bishop Mobility
    2249,  2487,  2421,  2861,  // Bishop Mobility
    2249,  2583,  2540,  2908,  // Bishop Mobility
    2258,  2700,  2545,  2962,  // Bishop Mobility
    2280,  2750,  2651,  2976,  // Bishop Mobility
    2290,  2758,  2764,  2999,  // Bishop Mobility
    2290,  2759,  2764,  3128,  // Bishop Mobility
    2292,  2767,  2881,  3270,  // Bishop Mobility
    2486,  2767,  2881,  3278,  // Bishop Mobility
    2497,  2782,  2894,  3293,  // Bishop Mobility
    4135,  4216,  2973,  4563,  // Rook Mobility
    4297,  4430,  3178,  4676,  // Rook Mobility
    4494,  4680,  3215,  4718,  // Rook Mobility
    4666,  4760,  3247,  4770,  // Rook Mobility
    4789,  4848,  3252,  4784,  // Rook Mobility
    4987,  4862,  3353,  4856,  // Rook Mobility
    5035,  4946,  3438,  4914,  // Rook Mobility
    5048,  4947,  3561,  4956,  // Rook Mobility
    5075,  4955,  3730,  5035,  // Rook Mobility
    5095,  5010,  3825,  5038,  // Rook Mobility
    5108,  5045,  3872,  5214,  // Rook Mobility
    5191,  5049,  3982,  5229,  // Rook Mobility
    5301,  5057,  4042,  5293,  // Rook Mobility
    5301,  5066,  4065,  5435,  // Rook Mobility
    5301,  5128,  4065,  5435,  // Rook Mobility
    8932,  9003,  8220,  7832,  // Queen Mobility
    9116,  9195,  8304,  7912,  // Queen Mobility
    9158,  9235,  8330,  8009,  // Queen Mobility
    9163,  9237,  8330,  8009,  // Queen Mobility
    9246,  9317,  8348,  8060,  // Queen Mobility
    9282,  9423,  8370,  8074,  // Queen Mobility
    9298,  9441,  8382,  8074,  // Queen Mobility
    9331,  9476,  8384,  8074,  // Queen Mobility
    9380,  9514,  8384,  8074,  // Queen Mobility
    9418,  9571,  8388,  8074,  // Queen Mobility
    9481,  9608,  8391,  8074,  // Queen Mobility
    9509,  9620,  8392,  8074,  // Queen Mobility
    9509,  9623,  8392,  8074,  // Queen Mobility
    9609,  9708,  8392,  8074,  // Queen Mobility
    9613,  9711,  8392,  8074,  // Queen Mobility
    9637,  9751,  8394,  8074,  // Queen Mobility
    9673,  9805,  8422,  8074,  // Queen Mobility
    9673,  9805,  8437,  8084,  // Queen Mobility
    9677,  9807,  8465,  8141,  // Queen Mobility
    9677,  9807,  8513,  8296,  // Queen Mobility
    9677,  9807,  8518,  8302,  // Queen Mobility
    9677,  9807,  8526,  8367,  // Queen Mobility
    9677,  9807,  8526,  8367,  // Queen Mobility
    9677,  9807,  8527,  8374,  // Queen Mobility
    9677,  9808,  8527,  8374,  // Queen Mobility
    9677,  9808,  8527,  8374,  // Queen Mobility
    9678,  9808,  8531,  8387,  // Queen Mobility
    9678,  9808,  8531,  8387,  // Queen Mobility
    -405,  318,   708,   61,    // Safe Checks
    -201,  87,    279,   -3,    // Unsafe Checks
    259,   -22,   -342,  -54,   // Unprotected Pieces
    294,   296,   256,   256,   // Bishop's Position Relative To King
    368,   333,   164,   231,   // Bishop's Position Relative To King
    383,   553,   316,   143,   // Bishop's Position Relative To King
    282,   376,   312,   188,   // Bishop's Position Relative To King
    566,   350,   151,   328,   // Bishop's Position Relative To King
    498,   406,   68,    157,   // Bishop's Position Relative To King
    719,   603,   -50,   103,   // Bishop's Position Relative To King
    437,   412,   101,   22,    // Bishop's Position Relative To King
    485,   515,   479,   430,   // Bishop's Position Relative To King
    512,   270,   258,   277,   // Bishop's Position Relative To King
    494,   299,   344,   349,   // Bishop's Position Relative To King
    504,   325,   138,   210,   // Bishop's Position Relative To King
    593,   361,   236,   302,   // Bishop's Position Relative To King
    389,   315,   202,   216,   // Bishop's Position Relative To King
    447,   390,   384,   293,   // Bishop's Position Relative To King
    560,   587,   325,   437,   // Bishop's Position Relative To King
    418,   358,   269,   199,   // Bishop's Position Relative To King
    447,   487,   275,   228,   // Bishop's Position Relative To King
    528,   266,   188,   193,   // Bishop's Position Relative To King
    542,   339,   382,   200,   // Bishop's Position Relative To King
    460,   160,   453,   159,   // Bishop's Position Relative To King
    633,   592,   440,   173,   // Bishop's Position Relative To King
    264,   409,   311,   162,   // Bishop's Position Relative To King
    501,   371,   348,   158,   // Bishop's Position Relative To King
    437,   334,   119,   109,   // Bishop's Position Relative To King
    452,   494,   291,   168,   // Bishop's Position Relative To King
    538,   784,   286,   183,   // Bishop's Position Relative To King
    210,   408,   375,   72,    // Bishop's Position Relative To King
    458,   299,   448,   56,    // Bishop's Position Relative To King
    155,   321,   386,   125,   // Bishop's Position Relative To King
    392,   682,   1112,  543,   // Bishop's Position Relative To King
    -23,   285,   671,   243,   // Bishop's Position Relative To King
    348,   373,   382,   270,   // Bishop's Position Relative To King
    879,   858,   966,   1088,  // Bishop's Position Relative To King
    -17,   272,   493,   -198,  // Bishop's Position Relative To King
    0,     0,     0,     0,     // Bishop's Position Relative To King
    -79,   24,    157,   -22,   // Opposed Pawns
    -18,   -14,   20,    -91,   // Opposed Pawns
    -84,   21,    84,    -5,    // Opposed Pawns
    -34,   150,   16,    81,    // Opposed Pawns
    -203,  381,   434,   231,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    21,    134,   182,   97,    // Opposed Pawns
    -19,   158,   145,   141,   // Opposed Pawns
    127,   232,   89,    319,   // Opposed Pawns
    260,   815,   490,   194,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    -33,   -39,   222,   44,    // Opposed Pawns
    -60,   13,    79,    42,    // Opposed Pawns
    -11,   54,    124,   167,   // Opposed Pawns
    108,   64,    307,   167,   // Opposed Pawns
    658,   707,   681,   697,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    113,   190,   227,   120,   // Opposed Pawns
    -92,   206,   324,   291,   // Opposed Pawns
    267,   338,   557,   398,   // Opposed Pawns
    495,   494,   492,   491,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    -131,  3,     149,   119,   // Unopposed Pawns
    -151,  58,    -27,   -45,   // Unopposed Pawns
    -131,  41,    16,    346,   // Unopposed Pawns
    -68,   120,   362,   318,   // Unopposed Pawns
    447,   538,   682,   869,   // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    -77,   243,   259,   354,   // Unopposed Pawns
    50,    197,   271,   647,   // Unopposed Pawns
    288,   388,   487,   658,   // Unopposed Pawns
    1129,  1016,  969,   998,   // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    -53,   15,    241,   262,   // Unopposed Pawns
    45,    123,   44,    268,   // Unopposed Pawns
    236,   103,   216,   495,   // Unopposed Pawns
    498,   361,   737,   586,   // Unopposed Pawns
    800,   883,   869,   868,   // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    133,   292,   371,   419,   // Unopposed Pawns
    411,   568,   312,   590,   // Unopposed Pawns
    866,   654,   659,   696,   // Unopposed Pawns
    160,   160,   159,   159,   // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    -3,    100,   235,   73,    // Covered Passed Pawns
    23,    186,   -87,   120,   // Covered Passed Pawns
    211,   336,   -82,   178,   // Covered Passed Pawns
    183,   535,   170,   436,   // Covered Passed Pawns
    -21,   737,   1168,  533,   // Covered Passed Pawns
    -288,  382,   1413,  1544,  // Covered Passed Pawns
    0,     0,     0,     0,     // Covered Passed Pawns
    -53,   313,   553,   232,   // Covered Passed Pawns
    296,   666,   73,    421,   // Covered Passed Pawns
    428,   948,   298,   481,   // Covered Passed Pawns
    692,   1380,  1338,  1241,  // Covered Passed Pawns
    1022,  1077,  1011,  995,   // Covered Passed Pawns
    -111,  118,   385,   182,   // Covered Passed Pawns
    102,   211,   135,   207,   // Covered Passed Pawns
    351,   686,   15,    182,   // Covered Passed Pawns
    537,   1040,  527,   636,   // Covered Passed Pawns
    1525,  1517,  1436,  1430,  // Covered Passed Pawns
    84,    84,    84,    84,    // Covered Passed Pawns
    0,     118,   385,   182,   // Covered Passed Pawns
    102,   313,   553,   232,   // Covered Passed Pawns
    351,   686,   73,    421,   // Covered Passed Pawns
    537,   1040,  527,   636,   // Covered Passed Pawns
    1525,  1517,  1436,  1430,  // Covered Passed Pawns
    1022,  1077,  1011,  995,   // Covered Passed Pawns
    299,   85,    -253,  -120,  // Uncovered Passed Pawns
    411,   59,    -65,   -54,   // Uncovered Passed Pawns
    1090,  250,   -315,  95,    // Uncovered Passed Pawns
    1911,  496,   -204,  106,   // Uncovered Passed Pawns
    3309,  829,   -2,    100,   // Uncovered Passed Pawns
    3683,  1606,  388,   750,   // Uncovered Passed Pawns
    0,     0,     0,     0,     // Uncovered Passed Pawns
    132,   358,   59,    36,    // Uncovered Passed Pawns
    766,   482,   100,   413,   // Uncovered Passed Pawns
    1598,  1068,  42,    52,    // Uncovered Passed Pawns
    2334,  1818,  951,   1047,  // Uncovered Passed Pawns
    2501,  2805,  2728,  2688,  // Uncovered Passed Pawns
    280,   134,   -73,   -195,  // Uncovered Passed Pawns
    -19,   116,   101,   103,   // Uncovered Passed Pawns
    743,   549,   216,   387,   // Uncovered Passed Pawns
    1313,  1097,  890,   992,   // Uncovered Passed Pawns
    2357,  2269,  2112,  2160,  // Uncovered Passed Pawns
    2535,  2475,  2465,  2465,  // Uncovered Passed Pawns
    280,   134,   0,     0,     // Uncovered Passed Pawns
    132,   358,   101,   103,   // Uncovered Passed Pawns
    766,   549,   216,   413,   // Uncovered Passed Pawns
    1598,  1097,  890,   992,   // Uncovered Passed Pawns
    2357,  2269,  2112,  2160,  // Uncovered Passed Pawns
    2535,  2805,  2728,  2688   // Uncovered Passed Pawns
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

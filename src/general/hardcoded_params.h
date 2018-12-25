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

////NFCM 181123
//const std::array<int, (174*4)> eval_weights = {
//    551,   -935,  -212,  -48,   // King Piece Square Table
//    173,   -442,  159,   17,    // King Piece Square Table
//    65,    -71,   -226,  -71,   // King Piece Square Table
//    -97,   259,   -483,  -145,  // King Piece Square Table
//    49,    -207,  440,   36,    // King Piece Square Table
//    -50,   159,   213,   9,     // King Piece Square Table
//    -150,  453,   -204,  180,   // King Piece Square Table
//    -85,   286,   225,   517,   // King Piece Square Table
//    -161,  446,   83,    778,   // King Piece Square Table
//    -165,  364,   474,   953,   // King Piece Square Table
//    2554,  2596,  1136,  1,     // Knight Piece Square Table
//    2285,  2564,  1805,  557,   // Knight Piece Square Table
//    2182,  2578,  1875,  508,   // Knight Piece Square Table
//    2066,  2734,  2018,  590,   // Knight Piece Square Table
//    2238,  2726,  1869,  425,   // Knight Piece Square Table
//    1978,  2748,  1885,  545,   // Knight Piece Square Table
//    1891,  2732,  2052,  622,   // Knight Piece Square Table
//    1956,  2699,  2004,  653,   // Knight Piece Square Table
//    1908,  2857,  2062,  651,   // Knight Piece Square Table
//    1878,  2902,  1965,  620,   // Knight Piece Square Table
//    857,   827,   138,   61,    // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    352,   595,   0,     0,     // Bishop Pair
//    106,   11,    0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     64,    65,    12,    // Abstract Piece Activity Bonus
//    99,    15,    23,    8,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    165,   48,    0,     0,     // Passed Pawn Bonus
//    171,   363,   149,   25,    // Passed Pawn Bonus
//    102,   373,   685,   111,   // Passed Pawn Bonus
//    50,    309,   0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    126,   136,   0,     4,     // Passed Pawn Bonus
//    453,   487,   215,   8,     // Passed Pawn Bonus
//    961,   622,   731,   58,    // Passed Pawn Bonus
//    1563,  761,   1242,  338,   // Passed Pawn Bonus
//    -299,  -13,   0,     -5,    // Double Pawn Penalty
//    10,    0,     53,    44,    // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    -3,    15,    -61,   4,     // King Vector Exposure
//    35,    -8,    -89,   -2,    // King Vector Exposure
//    -27,   208,   67,    57,    // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    5,     228,   446,   172,   // Pawn Attack
//    1,     27,    392,   129,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     -115,  -7,    // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    0,     0,     -124,  -21,   // King Attack Distance
//    -168,  -1,    -30,   0,     // King Attack Distance
//    0,     0,     27,    10,    // King Attack Danger
//    0,     7,     10,    0,     // King Attack Danger
//    0,     0,     48,    4,     // King Attack Danger
//    7,     61,    0,     0,     // King Attack Danger
//    -801,  -839,  -461,  -187,  // Safe Knight Square Count
//    -202,  -519,  -227,  -97,   // Safe Knight Square Count
//    -154,  -242,  -125,  -48,   // Safe Knight Square Count
//    -81,   -76,   -59,   -16,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    4,     34,    101,   0,     // Safe Knight Square Count
//    4,     34,    179,   0,     // Safe Knight Square Count
//    4,     34,    208,   0,     // Safe Knight Square Count
//    4,     34,    208,   0,     // Safe Knight Square Count
//    1349,  1697,  656,   241,   // Bishop Mobility
//    1612,  1849,  787,   258,   // Bishop Mobility
//    1690,  2017,  938,   336,   // Bishop Mobility
//    1712,  2215,  1041,  371,   // Bishop Mobility
//    1717,  2369,  1147,  410,   // Bishop Mobility
//    1724,  2496,  1222,  434,   // Bishop Mobility
//    1725,  2580,  1310,  447,   // Bishop Mobility
//    1726,  2689,  1362,  456,   // Bishop Mobility
//    1727,  2718,  1444,  456,   // Bishop Mobility
//    1727,  2718,  1563,  458,   // Bishop Mobility
//    1727,  2718,  1663,  476,   // Bishop Mobility
//    1727,  2718,  1731,  606,   // Bishop Mobility
//    1727,  2718,  1739,  665,   // Bishop Mobility
//    1727,  2719,  1747,  777,   // Bishop Mobility
//    3805,  3484,  1850,  218,   // Rook Mobility
//    3963,  3620,  1882,  254,   // Rook Mobility
//    3979,  3741,  1906,  270,   // Rook Mobility
//    4033,  3877,  1928,  307,   // Rook Mobility
//    4093,  4024,  1928,  310,   // Rook Mobility
//    4311,  4033,  2006,  346,   // Rook Mobility
//    4381,  4114,  2056,  384,   // Rook Mobility
//    4382,  4163,  2163,  423,   // Rook Mobility
//    4529,  4193,  2233,  426,   // Rook Mobility
//    4641,  4223,  2295,  454,   // Rook Mobility
//    4709,  4245,  2349,  484,   // Rook Mobility
//    4736,  4255,  2436,  508,   // Rook Mobility
//    4753,  4267,  2469,  670,   // Rook Mobility
//    4753,  4267,  2512,  849,   // Rook Mobility
//    4753,  4270,  2555,  881,   // Rook Mobility
//    7072,  7541,  3577,  2901,  // Queen Mobility
//    7347,  7784,  3725,  2946,  // Queen Mobility
//    7404,  7827,  3741,  2963,  // Queen Mobility
//    7411,  7831,  3746,  2994,  // Queen Mobility
//    7441,  7856,  3782,  3013,  // Queen Mobility
//    7505,  7908,  3799,  3038,  // Queen Mobility
//    7544,  7955,  3827,  3063,  // Queen Mobility
//    7572,  8025,  3844,  3078,  // Queen Mobility
//    7587,  8069,  3872,  3081,  // Queen Mobility
//    7622,  8148,  3888,  3081,  // Queen Mobility
//    7648,  8213,  3891,  3081,  // Queen Mobility
//    7697,  8278,  3894,  3081,  // Queen Mobility
//    7740,  8302,  3897,  3081,  // Queen Mobility
//    7782,  8322,  3898,  3081,  // Queen Mobility
//    7806,  8335,  3899,  3082,  // Queen Mobility
//    7811,  8336,  3906,  3082,  // Queen Mobility
//    7813,  8337,  3906,  3084,  // Queen Mobility
//    7813,  8338,  3910,  3091,  // Queen Mobility
//    7813,  8338,  3910,  3092,  // Queen Mobility
//    7813,  8338,  3910,  3092,  // Queen Mobility
//    7813,  8338,  3910,  3092,  // Queen Mobility
//    7813,  8338,  3910,  3092,  // Queen Mobility
//    7813,  8338,  3910,  3092,  // Queen Mobility
//    7813,  8338,  3910,  3092,  // Queen Mobility
//    7813,  8338,  3910,  3092,  // Queen Mobility
//    7813,  8338,  3914,  3101,  // Queen Mobility
//    7814,  8338,  3991,  3285,  // Queen Mobility
//    7877,  8382,  4094,  3388,  // Queen Mobility
//    -274,  200,   359,   3,     // Safe Checks
//    -109,  159,   59,    -6,    // Unsafe Checks
//    26,    49,    -168,  -28,   // Unprotected Pieces
//    253,   239,   172,   304,   // Bishop's Position Relative To King
//    98,    290,   294,   99,    // Bishop's Position Relative To King
//    224,   294,   187,   177,   // Bishop's Position Relative To King
//    176,   237,   171,   73,    // Bishop's Position Relative To King
//    324,   352,   45,    26,    // Bishop's Position Relative To King
//    367,   392,   -39,   29,    // Bishop's Position Relative To King
//    229,   407,   87,    17,    // Bishop's Position Relative To King
//    296,   385,   38,    -22,   // Bishop's Position Relative To King
//    212,   265,   301,   164,   // Bishop's Position Relative To King
//    96,    350,   191,   67,    // Bishop's Position Relative To King
//    206,   281,   193,   169,   // Bishop's Position Relative To King
//    234,   339,   3,     -9,    // Bishop's Position Relative To King
//    276,   295,   159,   50,    // Bishop's Position Relative To King
//    266,   279,   72,    30,    // Bishop's Position Relative To King
//    270,   241,   217,   44,    // Bishop's Position Relative To King
//    273,   318,   273,   174,   // Bishop's Position Relative To King
//    163,   344,   130,   62,    // Bishop's Position Relative To King
//    244,   284,   147,   93,    // Bishop's Position Relative To King
//    202,   301,   33,    85,    // Bishop's Position Relative To King
//    342,   217,   184,   51,    // Bishop's Position Relative To King
//    275,   234,   91,    32,    // Bishop's Position Relative To King
//    257,   348,   135,   85,    // Bishop's Position Relative To King
//    120,   313,   143,   32,    // Bishop's Position Relative To King
//    282,   239,   147,   44,    // Bishop's Position Relative To King
//    222,   290,   2,     -3,    // Bishop's Position Relative To King
//    279,   200,   250,   72,    // Bishop's Position Relative To King
//    173,   330,   331,   26,    // Bishop's Position Relative To King
//    103,   282,   210,   -31,   // Bishop's Position Relative To King
//    265,   145,   303,   30,    // Bishop's Position Relative To King
//    146,   310,   109,   11,    // Bishop's Position Relative To King
//    18,    349,   843,   558,   // Bishop's Position Relative To King
//    -64,   140,   621,   160,   // Bishop's Position Relative To King
//    -15,   22,    817,   236,   // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    19,    247,   290,   -166,  // Bishop's Position Relative To King
//    0,     0,     0,     0      // Bishop's Position Relative To King
//};

// NFCM 181222
//const std::array<int, (270*4)> eval_weights = {
//    -245,  -324,  -200,  -109,  // King Piece Square Table
//    -164,  -234,  50,    24,    // King Piece Square Table
//    -72,   -140,  -130,  -51,   // King Piece Square Table
//    -25,   -46,   -254,  -95,   // King Piece Square Table
//    -56,   -85,   144,   99,    // King Piece Square Table
//    59,    75,    52,    47,    // King Piece Square Table
//    73,    154,   25,    49,    // King Piece Square Table
//    120,   204,   165,   174,   // King Piece Square Table
//    110,   266,   242,   256,   // King Piece Square Table
//    218,   346,   372,   398,   // King Piece Square Table
//    563,   420,   211,   193,   // Knight Piece Square Table
//    734,   506,   210,   314,   // Knight Piece Square Table
//    732,   546,   268,   337,   // Knight Piece Square Table
//    845,   665,   404,   367,   // Knight Piece Square Table
//    798,   644,   376,   353,   // Knight Piece Square Table
//    775,   624,   378,   330,   // Knight Piece Square Table
//    810,   649,   416,   414,   // Knight Piece Square Table
//    760,   630,   499,   414,   // Knight Piece Square Table
//    832,   720,   553,   448,   // Knight Piece Square Table
//    785,   683,   467,   432,   // Knight Piece Square Table
//    545,   408,   120,   82,    // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    345,   309,   109,   36,    // Bishop Pair
//    122,   9,     5,     10,    // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    10,    28,    44,    16,    // Abstract Piece Activity Bonus
//    87,    13,    9,     12,    // Abstract Piece Activity Bonus
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
//    -51,   -14,   -10,   -27,   // Double Pawn Penalty
//    4,     6,     52,    46,    // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    -58,   -9,    -18,   -14,   // King Vector Exposure
//    -194,  58,    -13,   -7,    // King Vector Exposure
//    95,    110,   110,   27,    // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    111,   127,   104,   101,   // Pawn Attack
//    75,    71,    154,   92,    // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     -3,    -7,    // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    -8,    -9,    -40,   -23,   // King Attack Distance
//    0,     0,     -4,    -5,    // King Attack Distance
//    6,     10,    41,    14,    // King Attack Danger
//    21,    10,    17,    8,     // King Attack Danger
//    43,    0,     28,    12,    // King Attack Danger
//    43,    69,    8,     14,    // King Attack Danger
//    -283,  -288,  -311,  -133,  // Safe Knight Square Count
//    -166,  -172,  -177,  -68,   // Safe Knight Square Count
//    -66,   -115,  -136,  -39,   // Safe Knight Square Count
//    2,     -6,    -22,   -32,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    99,    86,    53,    8,     // Safe Knight Square Count
//    115,   104,   80,    6,     // Safe Knight Square Count
//    118,   103,   85,    21,    // Safe Knight Square Count
//    118,   99,    83,    16,    // Safe Knight Square Count
//    111,   87,    28,    89,    // Bishop Mobility
//    220,   185,   122,   125,   // Bishop Mobility
//    309,   299,   222,   168,   // Bishop Mobility
//    388,   357,   287,   226,   // Bishop Mobility
//    450,   433,   365,   226,   // Bishop Mobility
//    490,   478,   402,   259,   // Bishop Mobility
//    548,   551,   448,   277,   // Bishop Mobility
//    608,   601,   460,   276,   // Bishop Mobility
//    622,   623,   495,   297,   // Bishop Mobility
//    651,   651,   536,   361,   // Bishop Mobility
//    646,   647,   531,   374,   // Bishop Mobility
//    654,   651,   533,   386,   // Bishop Mobility
//    652,   648,   531,   384,   // Bishop Mobility
//    666,   659,   546,   399,   // Bishop Mobility
//    669,   585,   439,   595,   // Rook Mobility
//    744,   654,   526,   596,   // Rook Mobility
//    768,   678,   544,   638,   // Rook Mobility
//    846,   753,   590,   646,   // Rook Mobility
//    863,   785,   593,   644,   // Rook Mobility
//    964,   872,   669,   663,   // Rook Mobility
//    1007,  900,   691,   695,   // Rook Mobility
//    1034,  922,   738,   714,   // Rook Mobility
//    1106,  1008,  822,   733,   // Rook Mobility
//    1139,  1036,  853,   830,   // Rook Mobility
//    1163,  1055,  877,   849,   // Rook Mobility
//    1192,  1091,  921,   910,   // Rook Mobility
//    1219,  1108,  931,   950,   // Rook Mobility
//    1238,  1126,  959,   970,   // Rook Mobility
//    1234,  1133,  956,   971,   // Rook Mobility
//    1414,  1349,  1230,  1207,  // Queen Mobility
//    1414,  1348,  1230,  1277,  // Queen Mobility
//    1424,  1358,  1236,  1314,  // Queen Mobility
//    1425,  1360,  1241,  1317,  // Queen Mobility
//    1496,  1429,  1303,  1328,  // Queen Mobility
//    1490,  1423,  1298,  1326,  // Queen Mobility
//    1510,  1442,  1318,  1341,  // Queen Mobility
//    1557,  1491,  1364,  1340,  // Queen Mobility
//    1593,  1532,  1385,  1339,  // Queen Mobility
//    1606,  1538,  1383,  1350,  // Queen Mobility
//    1642,  1580,  1409,  1359,  // Queen Mobility
//    1653,  1590,  1411,  1354,  // Queen Mobility
//    1668,  1607,  1420,  1353,  // Queen Mobility
//    1678,  1615,  1428,  1369,  // Queen Mobility
//    1676,  1613,  1425,  1367,  // Queen Mobility
//    1681,  1619,  1429,  1380,  // Queen Mobility
//    1679,  1617,  1428,  1379,  // Queen Mobility
//    1681,  1618,  1434,  1396,  // Queen Mobility
//    1684,  1624,  1444,  1405,  // Queen Mobility
//    1686,  1624,  1444,  1407,  // Queen Mobility
//    1687,  1624,  1449,  1421,  // Queen Mobility
//    1685,  1621,  1447,  1419,  // Queen Mobility
//    1683,  1618,  1446,  1418,  // Queen Mobility
//    1678,  1613,  1442,  1414,  // Queen Mobility
//    1678,  1612,  1445,  1419,  // Queen Mobility
//    1676,  1610,  1444,  1418,  // Queen Mobility
//    1675,  1609,  1444,  1418,  // Queen Mobility
//    1675,  1608,  1445,  1420,  // Queen Mobility
//    120,   138,   109,   5,     // Safe Checks
//    95,    116,   29,    -16,   // Unsafe Checks
//    142,   39,    -144,  -8,    // Unprotected Pieces
//    54,    53,    49,    28,    // Bishop's Position Relative To King
//    11,    14,    12,    41,    // Bishop's Position Relative To King
//    22,    47,    36,    89,    // Bishop's Position Relative To King
//    30,    27,    3,     -29,   // Bishop's Position Relative To King
//    23,    34,    -4,    -48,   // Bishop's Position Relative To King
//    8,     10,    -10,   -16,   // Bishop's Position Relative To King
//    48,    57,    35,    -54,   // Bishop's Position Relative To King
//    -15,   -10,   -35,   -21,   // Bishop's Position Relative To King
//    50,    38,    60,    76,    // Bishop's Position Relative To King
//    67,    79,    17,    41,    // Bishop's Position Relative To King
//    90,    75,    25,    105,   // Bishop's Position Relative To King
//    44,    48,    -37,   -39,   // Bishop's Position Relative To King
//    98,    108,   40,    3,     // Bishop's Position Relative To King
//    61,    14,    11,    -1,    // Bishop's Position Relative To King
//    54,    56,    40,    18,    // Bishop's Position Relative To King
//    92,    90,    117,   27,    // Bishop's Position Relative To King
//    51,    48,    -24,   22,    // Bishop's Position Relative To King
//    46,    44,    33,    29,    // Bishop's Position Relative To King
//    48,    50,    -31,   28,    // Bishop's Position Relative To King
//    61,    55,    59,    7,     // Bishop's Position Relative To King
//    31,    55,    -4,    46,    // Bishop's Position Relative To King
//    72,    85,    33,    64,    // Bishop's Position Relative To King
//    38,    48,    48,    19,    // Bishop's Position Relative To King
//    43,    54,    46,    2,     // Bishop's Position Relative To King
//    54,    39,    -39,   35,    // Bishop's Position Relative To King
//    57,    68,    69,    41,    // Bishop's Position Relative To King
//    76,    71,    84,    -17,   // Bishop's Position Relative To King
//    77,    54,    112,   -16,   // Bishop's Position Relative To King
//    -3,    -16,   129,   81,    // Bishop's Position Relative To King
//    90,    80,    47,    -58,   // Bishop's Position Relative To King
//    126,   131,   137,   91,    // Bishop's Position Relative To King
//    36,    35,    207,   105,   // Bishop's Position Relative To King
//    -4,    13,    119,   59,    // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    69,    72,    145,   144,   // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -32,   -13,   -17,   -55,   // Opposed Pawns
//    -45,   -59,   -92,   -99,   // Opposed Pawns
//    -33,   -24,   -32,   6,     // Opposed Pawns
//    38,    44,    68,    82,    // Opposed Pawns
//    39,    72,    161,   134,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -18,   -5,    47,    -4,    // Opposed Pawns
//    16,    8,     -46,   10,    // Opposed Pawns
//    48,    54,    22,    144,   // Opposed Pawns
//    165,   223,   220,   148,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -14,   -22,   -23,   -58,   // Opposed Pawns
//    -21,   -29,   -35,   -2,    // Opposed Pawns
//    -13,   12,    40,    55,    // Opposed Pawns
//    143,   143,   174,   125,   // Opposed Pawns
//    80,    80,    71,    76,    // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    68,    57,    29,    -8,    // Opposed Pawns
//    40,    16,    19,    54,    // Opposed Pawns
//    171,   198,   206,   198,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -75,   -64,   -115,  -123,  // Unopposed Pawns
//    -63,   -110,  -250,  -129,  // Unopposed Pawns
//    -94,   -72,   -57,   38,    // Unopposed Pawns
//    -1,    53,    129,   102,   // Unopposed Pawns
//    147,   167,   174,   113,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -63,   -59,   -13,   -22,   // Unopposed Pawns
//    34,    -8,    -20,   27,    // Unopposed Pawns
//    132,   139,   235,   167,   // Unopposed Pawns
//    206,   224,   233,   223,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -46,   -78,   -11,   -76,   // Unopposed Pawns
//    -42,   -90,   -147,  -39,   // Unopposed Pawns
//    49,    33,    29,    92,    // Unopposed Pawns
//    184,   213,   248,   142,   // Unopposed Pawns
//    63,    63,    53,    60,    // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    44,    30,    -19,   -64,   // Unopposed Pawns
//    97,    74,    80,    48,    // Unopposed Pawns
//    188,   184,   212,   228,   // Unopposed Pawns
//    25,    25,    25,    25,    // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -6,    -4,    -81,   -71,   // Covered Passed Pawns
//    79,    -66,   -186,  -101,  // Covered Passed Pawns
//    147,   4,     -176,  -123,  // Covered Passed Pawns
//    93,    97,    67,    34,    // Covered Passed Pawns
//    186,   174,   290,   268,   // Covered Passed Pawns
//    55,    90,    31,    12,    // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    -55,   -61,   -131,  -122,  // Covered Passed Pawns
//    68,    -33,   -125,  -87,   // Covered Passed Pawns
//    161,   100,   68,    91,    // Covered Passed Pawns
//    195,   246,   279,   267,   // Covered Passed Pawns
//    45,    50,    45,    44,    // Covered Passed Pawns
//    -107,  -91,   -50,   -51,   // Covered Passed Pawns
//    -87,   -65,   -137,  -117,  // Covered Passed Pawns
//    77,    66,    6,     -8,    // Covered Passed Pawns
//    156,   208,   205,   190,   // Covered Passed Pawns
//    216,   207,   202,   201,   // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    -55,   -61,   -131,  -117,  // Covered Passed Pawns
//    77,    66,    6,     -8,    // Covered Passed Pawns
//    161,   208,   205,   190,   // Covered Passed Pawns
//    216,   246,   279,   267,   // Covered Passed Pawns
//    45,    50,    45,    44,    // Covered Passed Pawns
//    -106,  -110,  -165,  -127,  // Uncovered Passed Pawns
//    -86,   -65,   -84,   -100,  // Uncovered Passed Pawns
//    86,    109,   58,    45,    // Uncovered Passed Pawns
//    261,   258,   202,   116,   // Uncovered Passed Pawns
//    426,   435,   419,   323,   // Uncovered Passed Pawns
//    619,   592,   602,   584,   // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    -83,   -76,   -79,   -81,   // Uncovered Passed Pawns
//    -21,   28,    24,    11,    // Uncovered Passed Pawns
//    153,   159,   119,   144,   // Uncovered Passed Pawns
//    338,   380,   395,   379,   // Uncovered Passed Pawns
//    227,   252,   258,   257,   // Uncovered Passed Pawns
//    -138,  -125,  -199,  -147,  // Uncovered Passed Pawns
//    -40,   -31,   -34,   -17,   // Uncovered Passed Pawns
//    22,    17,    28,    25,    // Uncovered Passed Pawns
//    197,   204,   216,   206,   // Uncovered Passed Pawns
//    234,   257,   255,   254,   // Uncovered Passed Pawns
//    72,    70,    73,    72,    // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    -40,   -31,   -34,   -17,   // Uncovered Passed Pawns
//    22,    28,    28,    25,    // Uncovered Passed Pawns
//    197,   204,   216,   206,   // Uncovered Passed Pawns
//    338,   380,   395,   379,   // Uncovered Passed Pawns
//    227,   252,   258,   257    // Uncovered Passed Pawns
//};

// 181225 Experimental
const std::array<int, (270*4)> eval_weights = {
    -420,  -608,  -83,   48,    // King Piece Square Table
    -234,  -344,  178,   309,   // King Piece Square Table
    -62,   -43,   -271,  -33,   // King Piece Square Table
    -52,   98,    -574,  -73,   // King Piece Square Table
    -32,   -115,  446,   -41,   // King Piece Square Table
    112,   143,   230,   -183,  // King Piece Square Table
    102,   361,   -211,  -334,  // King Piece Square Table
    168,   219,   278,   -6,    // King Piece Square Table
    195,   278,   144,   -115,  // King Piece Square Table
    264,   223,   53,    136,   // King Piece Square Table
    2205,  2318,  1764,  3452,  // Knight Piece Square Table
    2220,  2432,  2274,  3673,  // Knight Piece Square Table
    2263,  2549,  2263,  3660,  // Knight Piece Square Table
    2178,  2608,  2524,  3750,  // Knight Piece Square Table
    2325,  2612,  2224,  3531,  // Knight Piece Square Table
    2172,  2648,  2476,  3830,  // Knight Piece Square Table
    2277,  2618,  2532,  3880,  // Knight Piece Square Table
    2262,  2701,  2593,  3919,  // Knight Piece Square Table
    2224,  2837,  2674,  3894,  // Knight Piece Square Table
    2231,  2865,  2605,  3885,  // Knight Piece Square Table
    1022,  621,   213,   515,   // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    450,   208,   188,   966,   // Bishop Pair
    145,   3,     3,     26,    // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    6,     70,    70,    40,    // Abstract Piece Activity Bonus
    68,    34,    18,    13,    // Abstract Piece Activity Bonus
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
    -51,   -1,    -2,    -1,    // Double Pawn Penalty
    287,   54,    530,   248,   // Tempo Bonus
    0,     0,     0,     0,     // Direct Pawn Shield Bonus
    25,    32,    -118,  -36,   // King Vector Exposure
    -102,  72,    -99,   -24,   // King Vector Exposure
    -13,   273,   42,    184,   // Knight Outpost
    0,     0,     0,     0,     // Castling Right
    8,     137,   938,   570,   // Pawn Attack
    9,     22,    853,   444,   // Minor Attack
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    -2,    -3,    -41,   -3,    // King Attack Distance
    0,     0,     0,     0,     // King Attack Distance
    -1,    -3,    -72,   -2,    // King Attack Distance
    -90,   -69,   -97,   -3,    // King Attack Distance
    0,     1,     10,    14,    // King Attack Danger
    3,     3,     21,    9,     // King Attack Danger
    0,     0,     63,    49,    // King Attack Danger
    21,    154,   42,    12,    // King Attack Danger
    -471,  -440,  -440,  -329,  // Safe Knight Square Count
    -85,   -114,  -416,  -172,  // Safe Knight Square Count
    -22,   -70,   -203,  -94,   // Safe Knight Square Count
    -5,    -20,   -117,  -59,   // Safe Knight Square Count
    0,     0,     0,     0,     // Safe Knight Square Count
    8,     6,     108,   37,    // Safe Knight Square Count
    10,    7,     160,   80,    // Safe Knight Square Count
    10,    7,     160,   81,    // Safe Knight Square Count
    9,     6,     160,   88,    // Safe Knight Square Count
    1513,  1849,  1450,  2048,  // Bishop Mobility
    1824,  1967,  1748,  2201,  // Bishop Mobility
    2020,  2073,  2016,  2300,  // Bishop Mobility
    2033,  2126,  2155,  2398,  // Bishop Mobility
    2123,  2216,  2237,  2470,  // Bishop Mobility
    2135,  2286,  2341,  2513,  // Bishop Mobility
    2146,  2385,  2410,  2569,  // Bishop Mobility
    2157,  2456,  2432,  2601,  // Bishop Mobility
    2189,  2523,  2540,  2612,  // Bishop Mobility
    2198,  2540,  2617,  2677,  // Bishop Mobility
    2202,  2545,  2619,  2755,  // Bishop Mobility
    2231,  2588,  2694,  2894,  // Bishop Mobility
    2316,  2607,  2705,  2911,  // Bishop Mobility
    2359,  2616,  2718,  2922,  // Bishop Mobility
    3941,  3984,  2816,  3965,  // Rook Mobility
    4141,  4170,  2941,  4064,  // Rook Mobility
    4297,  4382,  2974,  4101,  // Rook Mobility
    4499,  4472,  2996,  4155,  // Rook Mobility
    4615,  4549,  3013,  4176,  // Rook Mobility
    4723,  4554,  3116,  4226,  // Rook Mobility
    4794,  4617,  3192,  4290,  // Rook Mobility
    4827,  4640,  3329,  4362,  // Rook Mobility
    4837,  4663,  3459,  4424,  // Rook Mobility
    4888,  4687,  3529,  4443,  // Rook Mobility
    4914,  4729,  3568,  4619,  // Rook Mobility
    4980,  4730,  3611,  4620,  // Rook Mobility
    5110,  4745,  3643,  4634,  // Rook Mobility
    5116,  4754,  3670,  4724,  // Rook Mobility
    5126,  4808,  3690,  4737,  // Rook Mobility
    8421,  8319,  7450,  6610,  // Queen Mobility
    8635,  8539,  7545,  6689,  // Queen Mobility
    8692,  8597,  7593,  6767,  // Queen Mobility
    8702,  8607,  7605,  6777,  // Queen Mobility
    8788,  8693,  7635,  6834,  // Queen Mobility
    8820,  8767,  7666,  6847,  // Queen Mobility
    8867,  8823,  7666,  6848,  // Queen Mobility
    8894,  8850,  7677,  6848,  // Queen Mobility
    8961,  8912,  7683,  6852,  // Queen Mobility
    9006,  8966,  7683,  6852,  // Queen Mobility
    9021,  8968,  7683,  6852,  // Queen Mobility
    9056,  8973,  7686,  6851,  // Queen Mobility
    9058,  8974,  7685,  6850,  // Queen Mobility
    9136,  9064,  7698,  6854,  // Queen Mobility
    9139,  9066,  7700,  6856,  // Queen Mobility
    9150,  9080,  7709,  6863,  // Queen Mobility
    9176,  9108,  7716,  6862,  // Queen Mobility
    9176,  9109,  7722,  6875,  // Queen Mobility
    9181,  9114,  7748,  6970,  // Queen Mobility
    9183,  9118,  7750,  6985,  // Queen Mobility
    9185,  9119,  7755,  6993,  // Queen Mobility
    9200,  9138,  7790,  7049,  // Queen Mobility
    9200,  9138,  7790,  7050,  // Queen Mobility
    9199,  9138,  7789,  7049,  // Queen Mobility
    9200,  9138,  7790,  7049,  // Queen Mobility
    9201,  9139,  7791,  7051,  // Queen Mobility
    9221,  9155,  7838,  7121,  // Queen Mobility
    9222,  9156,  7839,  7122,  // Queen Mobility
    -309,  322,   622,   69,    // Safe Checks
    -148,  43,    247,   6,     // Unsafe Checks
    214,   -17,   -307,  -63,   // Unprotected Pieces
    284,   284,   245,   252,   // Bishop's Position Relative To King
    342,   307,   187,   222,   // Bishop's Position Relative To King
    333,   455,   278,   77,    // Bishop's Position Relative To King
    275,   324,   235,   178,   // Bishop's Position Relative To King
    522,   350,   146,   252,   // Bishop's Position Relative To King
    423,   355,   92,    130,   // Bishop's Position Relative To King
    688,   541,   -55,   69,    // Bishop's Position Relative To King
    448,   384,   55,    5,     // Bishop's Position Relative To King
    445,   469,   396,   311,   // Bishop's Position Relative To King
    447,   263,   234,   251,   // Bishop's Position Relative To King
    445,   279,   267,   270,   // Bishop's Position Relative To King
    489,   314,   66,    171,   // Bishop's Position Relative To King
    522,   348,   217,   261,   // Bishop's Position Relative To King
    424,   282,   202,   197,   // Bishop's Position Relative To King
    409,   350,   365,   262,   // Bishop's Position Relative To King
    544,   545,   342,   390,   // Bishop's Position Relative To King
    351,   324,   228,   216,   // Bishop's Position Relative To King
    418,   433,   226,   187,   // Bishop's Position Relative To King
    501,   243,   107,   173,   // Bishop's Position Relative To King
    497,   317,   410,   139,   // Bishop's Position Relative To King
    434,   145,   390,   131,   // Bishop's Position Relative To King
    609,   527,   363,   162,   // Bishop's Position Relative To King
    256,   333,   246,   106,   // Bishop's Position Relative To King
    462,   331,   298,   106,   // Bishop's Position Relative To King
    409,   301,   128,   58,    // Bishop's Position Relative To King
    383,   452,   266,   143,   // Bishop's Position Relative To King
    506,   705,   310,   144,   // Bishop's Position Relative To King
    193,   347,   321,   60,    // Bishop's Position Relative To King
    427,   275,   374,   61,    // Bishop's Position Relative To King
    184,   270,   298,   147,   // Bishop's Position Relative To King
    383,   615,   912,   419,   // Bishop's Position Relative To King
    -26,   249,   606,   224,   // Bishop's Position Relative To King
    248,   314,   383,   216,   // Bishop's Position Relative To King
    720,   694,   850,   816,   // Bishop's Position Relative To King
    -92,   235,   329,   -251,  // Bishop's Position Relative To King
    0,     0,     0,     0,     // Bishop's Position Relative To King
    -103,  14,    126,   -20,   // Opposed Pawns
    -56,   -51,   4,     -84,   // Opposed Pawns
    -66,   -23,   56,    1,     // Opposed Pawns
    -27,   112,   9,     86,    // Opposed Pawns
    -228,  321,   395,   293,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    19,    96,    169,   101,   // Opposed Pawns
    -38,   120,   122,   141,   // Opposed Pawns
    116,   176,   61,    280,   // Opposed Pawns
    339,   718,   437,   217,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    -89,   -72,   197,   45,    // Opposed Pawns
    -90,   -31,   25,    32,    // Opposed Pawns
    -13,   18,    84,    157,   // Opposed Pawns
    105,   54,    246,   162,   // Opposed Pawns
    513,   561,   543,   537,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    92,    192,   190,   134,   // Opposed Pawns
    -95,   140,   317,   282,   // Opposed Pawns
    249,   253,   431,   295,   // Opposed Pawns
    349,   349,   346,   345,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    -186,  -30,   100,   37,    // Unopposed Pawns
    -180,  15,    -63,   -79,   // Unopposed Pawns
    -183,  10,    5,     230,   // Unopposed Pawns
    -102,  98,    310,   210,   // Unopposed Pawns
    415,   486,   697,   509,   // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    -123,  205,   191,   247,   // Unopposed Pawns
    21,    171,   224,   512,   // Unopposed Pawns
    259,   384,   331,   514,   // Unopposed Pawns
    1060,  911,   849,   853,   // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    -146,  -2,    189,   165,   // Unopposed Pawns
    -5,    81,    -30,   169,   // Unopposed Pawns
    145,   103,   115,   413,   // Unopposed Pawns
    541,   385,   589,   461,   // Unopposed Pawns
    652,   754,   738,   727,   // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    56,    251,   244,   278,   // Unopposed Pawns
    330,   451,   253,   495,   // Unopposed Pawns
    761,   550,   578,   582,   // Unopposed Pawns
    136,   136,   136,   136,   // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    -138,  89,    218,   24,    // Covered Passed Pawns
    -122,  159,   -24,   79,    // Covered Passed Pawns
    84,    300,   -97,   124,   // Covered Passed Pawns
    90,    487,   191,   321,   // Covered Passed Pawns
    -121,  732,   1141,  396,   // Covered Passed Pawns
    -375,  353,   1245,  1295,  // Covered Passed Pawns
    0,     0,     0,     0,     // Covered Passed Pawns
    -119,  232,   433,   199,   // Covered Passed Pawns
    173,   553,   29,    364,   // Covered Passed Pawns
    413,   893,   302,   411,   // Covered Passed Pawns
    721,   1203,  1139,  1057,  // Covered Passed Pawns
    766,   794,   736,   726,   // Covered Passed Pawns
    -199,  56,    292,   114,   // Covered Passed Pawns
    35,    128,   110,   176,   // Covered Passed Pawns
    258,   561,   92,    230,   // Covered Passed Pawns
    481,   875,   465,   535,   // Covered Passed Pawns
    1128,  1170,  1106,  1110,  // Covered Passed Pawns
    54,    54,    54,    54,    // Covered Passed Pawns
    0,     56,    292,   114,   // Covered Passed Pawns
    35,    232,   433,   199,   // Covered Passed Pawns
    258,   561,   92,    364,   // Covered Passed Pawns
    481,   893,   465,   535,   // Covered Passed Pawns
    1128,  1203,  1139,  1110,  // Covered Passed Pawns
    766,   794,   736,   726,   // Covered Passed Pawns
    121,   49,    -236,  -187,  // Uncovered Passed Pawns
    250,   9,     -80,   -113,  // Uncovered Passed Pawns
    926,   176,   -285,  18,    // Uncovered Passed Pawns
    1777,  436,   -226,  47,    // Uncovered Passed Pawns
    3160,  706,   -4,    123,   // Uncovered Passed Pawns
    3610,  1502,  419,   716,   // Uncovered Passed Pawns
    0,     0,     0,     0,     // Uncovered Passed Pawns
    75,    258,   47,    -13,   // Uncovered Passed Pawns
    723,   365,   46,    266,   // Uncovered Passed Pawns
    1465,  869,   112,   122,   // Uncovered Passed Pawns
    2158,  1527,  822,   937,   // Uncovered Passed Pawns
    2080,  2257,  2145,  2130,  // Uncovered Passed Pawns
    100,   2,     -139,  -201,  // Uncovered Passed Pawns
    -68,   31,    34,    49,    // Uncovered Passed Pawns
    678,   398,   153,   283,   // Uncovered Passed Pawns
    1131,  916,   717,   827,   // Uncovered Passed Pawns
    1927,  1752,  1630,  1664,  // Uncovered Passed Pawns
    2031,  1972,  1973,  1970,  // Uncovered Passed Pawns
    100,   2,     0,     0,     // Uncovered Passed Pawns
    75,    258,   47,    49,    // Uncovered Passed Pawns
    723,   398,   153,   283,   // Uncovered Passed Pawns
    1465,  916,   717,   827,   // Uncovered Passed Pawns
    2158,  1752,  1630,  1664,  // Uncovered Passed Pawns
    2080,  2257,  2145,  2130   // Uncovered Passed Pawns
};


// Mixed 181225
//const std::array<int, (270*4)> eval_weights = {
//    510,   -911,  -217,  -88,   // King Piece Square Table
//    137,   -440,  168,   4,     // King Piece Square Table
//    42,    -45,   -268,  -52,   // King Piece Square Table
//    -108,  263,   -514,  -129,  // King Piece Square Table
//    21,    -213,  446,   74,    // King Piece Square Table
//    -63,   144,   230,   -8,    // King Piece Square Table
//    -163,  420,   -193,  244,   // King Piece Square Table
//    -86,   270,   241,   472,   // King Piece Square Table
//    -153,  425,   119,   694,   // King Piece Square Table
//    -148,  358,   463,   843,   // King Piece Square Table
//    2354,  2448,  1273,  190,   // Knight Piece Square Table
//    2364,  2429,  1815,  634,   // Knight Piece Square Table
//    2181,  2464,  1904,  651,   // Knight Piece Square Table
//    2105,  2602,  2039,  714,   // Knight Piece Square Table
//    2234,  2657,  1843,  587,   // Knight Piece Square Table
//    2003,  2664,  1910,  688,   // Knight Piece Square Table
//    1946,  2656,  2044,  734,   // Knight Piece Square Table
//    2006,  2624,  2028,  719,   // Knight Piece Square Table
//    1937,  2759,  2107,  770,   // Knight Piece Square Table
//    1915,  2818,  2009,  716,   // Knight Piece Square Table
//    717,   707,   178,   78,    // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    409,   616,   0,     0,     // Bishop Pair
//    95,    35,    0,     9,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     58,    68,    11,    // Abstract Piece Activity Bonus
//    99,    8,     21,    6,     // Abstract Piece Activity Bonus
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
//    -221,  -1,    0,     -87,   // Double Pawn Penalty
//    21,    0,     75,    55,    // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    4,     13,    -67,   -16,   // King Vector Exposure
//    28,    -2,    -91,   -10,   // King Vector Exposure
//    -33,   193,   78,    97,    // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    5,     208,   427,   145,   // Pawn Attack
//    0,     33,    410,   140,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     -115,  -9,    // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    0,     0,     -122,  -26,   // King Attack Distance
//    -127,  -1,    -45,   0,     // King Attack Distance
//    0,     0,     32,    10,    // King Attack Danger
//    0,     6,     13,    1,     // King Attack Danger
//    0,     0,     46,    8,     // King Attack Danger
//    2,     66,    0,     0,     // King Attack Danger
//    -636,  -664,  -489,  -108,  // Safe Knight Square Count
//    -198,  -435,  -204,  -58,   // Safe Knight Square Count
//    -100,  -215,  -125,  -37,   // Safe Knight Square Count
//    -66,   -74,   -50,   -23,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    3,     18,    103,   6,     // Safe Knight Square Count
//    3,     18,    182,   6,     // Safe Knight Square Count
//    3,     18,    200,   7,     // Safe Knight Square Count
//    3,     18,    200,   7,     // Safe Knight Square Count
//    1358,  1727,  714,   366,   // Bishop Mobility
//    1568,  1838,  859,   388,   // Bishop Mobility
//    1716,  1947,  1003,  426,   // Bishop Mobility
//    1741,  2133,  1091,  454,   // Bishop Mobility
//    1746,  2279,  1197,  469,   // Bishop Mobility
//    1750,  2408,  1270,  486,   // Bishop Mobility
//    1751,  2501,  1353,  491,   // Bishop Mobility
//    1751,  2603,  1423,  494,   // Bishop Mobility
//    1751,  2626,  1507,  498,   // Bishop Mobility
//    1751,  2626,  1636,  499,   // Bishop Mobility
//    1751,  2626,  1729,  523,   // Bishop Mobility
//    1751,  2627,  1773,  634,   // Bishop Mobility
//    1751,  2627,  1773,  777,   // Bishop Mobility
//    1752,  2627,  1774,  782,   // Bishop Mobility
//    3738,  3450,  2012,  552,   // Rook Mobility
//    3895,  3594,  2024,  564,   // Rook Mobility
//    3919,  3690,  2071,  571,   // Rook Mobility
//    4000,  3828,  2075,  589,   // Rook Mobility
//    4043,  3945,  2075,  589,   // Rook Mobility
//    4272,  3970,  2120,  608,   // Rook Mobility
//    4321,  4051,  2164,  629,   // Rook Mobility
//    4326,  4098,  2255,  659,   // Rook Mobility
//    4477,  4132,  2312,  680,   // Rook Mobility
//    4560,  4150,  2381,  701,   // Rook Mobility
//    4647,  4169,  2418,  733,   // Rook Mobility
//    4668,  4175,  2476,  803,   // Rook Mobility
//    4670,  4176,  2536,  961,   // Rook Mobility
//    4670,  4178,  2554,  1145,  // Rook Mobility
//    4670,  4178,  2594,  1197,  // Rook Mobility
//    6824,  7427,  3994,  3313,  // Queen Mobility
//    7042,  7606,  4055,  3386,  // Queen Mobility
//    7075,  7636,  4082,  3387,  // Queen Mobility
//    7077,  7638,  4084,  3387,  // Queen Mobility
//    7107,  7672,  4097,  3387,  // Queen Mobility
//    7149,  7708,  4115,  3387,  // Queen Mobility
//    7188,  7751,  4130,  3389,  // Queen Mobility
//    7218,  7810,  4135,  3389,  // Queen Mobility
//    7249,  7866,  4143,  3389,  // Queen Mobility
//    7269,  7913,  4144,  3389,  // Queen Mobility
//    7299,  7957,  4149,  3389,  // Queen Mobility
//    7336,  8001,  4148,  3389,  // Queen Mobility
//    7365,  8023,  4149,  3389,  // Queen Mobility
//    7403,  8045,  4149,  3389,  // Queen Mobility
//    7422,  8063,  4149,  3389,  // Queen Mobility
//    7428,  8067,  4153,  3389,  // Queen Mobility
//    7430,  8069,  4164,  3390,  // Queen Mobility
//    7430,  8069,  4169,  3412,  // Queen Mobility
//    7430,  8069,  4171,  3417,  // Queen Mobility
//    7430,  8069,  4172,  3418,  // Queen Mobility
//    7430,  8069,  4172,  3418,  // Queen Mobility
//    7430,  8069,  4172,  3418,  // Queen Mobility
//    7430,  8069,  4173,  3419,  // Queen Mobility
//    7430,  8069,  4173,  3419,  // Queen Mobility
//    7431,  8069,  4174,  3421,  // Queen Mobility
//    7432,  8069,  4181,  3431,  // Queen Mobility
//    7432,  8070,  4307,  3669,  // Queen Mobility
//    7472,  8092,  4377,  3777,  // Queen Mobility
//    -266,  200,   359,   -1,    // Safe Checks
//    -109,  157,   65,    12,    // Unsafe Checks
//    44,    34,    -160,  -30,   // Unprotected Pieces
//    236,   239,   213,   322,   // Bishop's Position Relative To King
//    50,    276,   298,   137,   // Bishop's Position Relative To King
//    183,   258,   193,   169,   // Bishop's Position Relative To King
//    242,   274,   132,   5,     // Bishop's Position Relative To King
//    305,   386,   70,    9,     // Bishop's Position Relative To King
//    348,   415,   -58,   6,     // Bishop's Position Relative To King
//    224,   415,   75,    -3,    // Bishop's Position Relative To King
//    274,   387,   -20,   -80,   // Bishop's Position Relative To King
//    209,   222,   304,   231,   // Bishop's Position Relative To King
//    160,   325,   170,   64,    // Bishop's Position Relative To King
//    178,   276,   184,   210,   // Bishop's Position Relative To King
//    226,   345,   6,     1,     // Bishop's Position Relative To King
//    283,   278,   166,   63,    // Bishop's Position Relative To King
//    233,   284,   66,    37,    // Bishop's Position Relative To King
//    171,   270,   228,   72,    // Bishop's Position Relative To King
//    242,   299,   275,   197,   // Bishop's Position Relative To King
//    175,   330,   97,    60,    // Bishop's Position Relative To King
//    267,   260,   137,   111,   // Bishop's Position Relative To King
//    216,   301,   19,    72,    // Bishop's Position Relative To King
//    312,   231,   186,   44,    // Bishop's Position Relative To King
//    256,   211,   119,   55,    // Bishop's Position Relative To King
//    263,   343,   117,   103,   // Bishop's Position Relative To King
//    138,   299,   140,   65,    // Bishop's Position Relative To King
//    277,   222,   162,   45,    // Bishop's Position Relative To King
//    222,   270,   16,    35,    // Bishop's Position Relative To King
//    253,   207,   251,   129,   // Bishop's Position Relative To King
//    188,   336,   314,   44,    // Bishop's Position Relative To King
//    136,   225,   242,   0,     // Bishop's Position Relative To King
//    276,   104,   284,   125,   // Bishop's Position Relative To King
//    162,   298,   105,   30,    // Bishop's Position Relative To King
//    83,    298,   827,   542,   // Bishop's Position Relative To King
//    -27,   102,   618,   203,   // Bishop's Position Relative To King
//    34,    -30,   793,   286,   // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    61,    234,   246,   -114,  // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    94,    67,    53,    -23,   // Opposed Pawns
//    86,    36,    -36,   -47,   // Opposed Pawns
//    74,    21,    52,    106,   // Opposed Pawns
//    60,    98,    151,   234,   // Opposed Pawns
//    -38,   304,   466,   200,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    49,    145,   147,   36,    // Opposed Pawns
//    44,    161,   117,   104,   // Opposed Pawns
//    65,    134,   256,   320,   // Opposed Pawns
//    -18,   577,   684,   373,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    103,   78,    46,    -25,   // Opposed Pawns
//    64,    64,    35,    49,    // Opposed Pawns
//    72,    58,    232,   160,   // Opposed Pawns
//    100,   100,   477,   276,   // Opposed Pawns
//    647,   653,   652,   642,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    156,   207,   182,   48,    // Opposed Pawns
//    82,    177,   253,   170,   // Opposed Pawns
//    86,    285,   656,   458,   // Opposed Pawns
//    647,   653,   652,   642,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    60,    29,    -114,  -103,  // Unopposed Pawns
//    42,    1,     -351,  -102,  // Unopposed Pawns
//    -15,   -42,   -60,   115,   // Unopposed Pawns
//    -53,   76,    304,   164,   // Unopposed Pawns
//    56,    332,   811,   354,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    75,    145,   37,    -18,   // Unopposed Pawns
//    90,    151,   108,   121,   // Unopposed Pawns
//    95,    270,   620,   309,   // Unopposed Pawns
//    844,   853,   993,   874,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    197,   -25,   -9,    -66,   // Unopposed Pawns
//    151,   115,   -131,  -14,   // Unopposed Pawns
//    216,   122,   190,   195,   // Unopposed Pawns
//    280,   408,   750,   243,   // Unopposed Pawns
//    633,   638,   662,   676,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    176,   297,   55,    -103,  // Unopposed Pawns
//    201,   268,   322,   155,   // Unopposed Pawns
//    360,   542,   1046,  844,   // Unopposed Pawns
//    633,   638,   662,   676,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    290,   -17,   -59,   -182,  // Covered Passed Pawns
//    279,   100,   -454,  -127,  // Covered Passed Pawns
//    310,   263,   -300,  -85,   // Covered Passed Pawns
//    246,   312,   148,   133,   // Covered Passed Pawns
//    133,   380,   627,   250,   // Covered Passed Pawns
//    -31,   562,   -146,  -91,   // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    289,   69,    -48,   -147,  // Covered Passed Pawns
//    375,   342,   -103,  -115,  // Covered Passed Pawns
//    429,   459,   293,   319,   // Covered Passed Pawns
//    347,   532,   1092,  728,   // Covered Passed Pawns
//    688,   392,   321,   258,   // Covered Passed Pawns
//    203,   -87,   31,    -152,  // Covered Passed Pawns
//    314,   126,   -268,  -178,  // Covered Passed Pawns
//    403,   448,   -80,   -44,   // Covered Passed Pawns
//    326,   610,   613,   466,   // Covered Passed Pawns
//    759,   838,   903,   879,   // Covered Passed Pawns
//    78,    77,    76,    76,    // Covered Passed Pawns
//    203,   0,     31,    0,     // Covered Passed Pawns
//    314,   126,   -48,   -147,  // Covered Passed Pawns
//    403,   448,   -80,   -44,   // Covered Passed Pawns
//    429,   610,   613,   466,   // Covered Passed Pawns
//    759,   838,   1092,  879,   // Covered Passed Pawns
//    759,   838,   1092,  879,   // Covered Passed Pawns
//    -149,  41,    -258,  -130,  // Uncovered Passed Pawns
//    43,    -36,   -213,  -174,  // Uncovered Passed Pawns
//    262,   207,   -24,   108,   // Uncovered Passed Pawns
//    615,   466,   249,   91,    // Uncovered Passed Pawns
//    1083,  644,   691,   175,   // Uncovered Passed Pawns
//    1608,  823,   1215,  628,   // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    -75,   46,    -126,  -152,  // Uncovered Passed Pawns
//    80,    273,   181,   267,   // Uncovered Passed Pawns
//    289,   763,   381,   303,   // Uncovered Passed Pawns
//    338,   1109,  1178,  956,   // Uncovered Passed Pawns
//    1160,  1301,  1715,  1574,  // Uncovered Passed Pawns
//    -7,    -75,   -544,  -72,   // Uncovered Passed Pawns
//    -186,  -145,  -179,  -86,   // Uncovered Passed Pawns
//    230,   410,   72,    159,   // Uncovered Passed Pawns
//    539,   715,   629,   631,   // Uncovered Passed Pawns
//    1145,  1185,  1202,  1214,  // Uncovered Passed Pawns
//    1750,  1627,  1617,  1621,  // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    -75,   46,    -126,  -86,   // Uncovered Passed Pawns
//    230,   410,   181,   267,   // Uncovered Passed Pawns
//    539,   763,   629,   631,   // Uncovered Passed Pawns
//    1145,  1185,  1202,  1214,  // Uncovered Passed Pawns
//    1750,  1627,  1715,  1621   // Uncovered Passed Pawns
//};


//SL
//const std::array<int, (270*4)> eval_weights = {
//    1038,  -1401, -304,  -621,  // King Piece Square Table
//    262,   -618,  280,   -557,  // King Piece Square Table
//    29,    -13,   -338,  -614,  // King Piece Square Table
//    -153,  379,   -593,  -727,  // King Piece Square Table
//    46,    -273,  706,   -550,  // King Piece Square Table
//    -223,  234,   412,   -607,  // King Piece Square Table
//    -280,  585,   -162,  -200,  // King Piece Square Table
//    -275,  344,   -37,   962,   // King Piece Square Table
//    -254,  488,   -392,  1542,  // King Piece Square Table
//    -190,  274,   429,   1372,  // King Piece Square Table
//    1176,  3208,  2930,  2,     // Knight Piece Square Table
//    1234,  3349,  3922,  7,     // Knight Piece Square Table
//    1043,  3412,  3937,  22,    // Knight Piece Square Table
//    959,   3586,  4073,  112,   // Knight Piece Square Table
//    1019,  3631,  3798,  1,     // Knight Piece Square Table
//    705,   3695,  3871,  52,    // Knight Piece Square Table
//    642,   3643,  4147,  121,   // Knight Piece Square Table
//    649,   3666,  4027,  111,   // Knight Piece Square Table
//    614,   3783,  4159,  172,   // Knight Piece Square Table
//    575,   3898,  4034,  106,   // Knight Piece Square Table
//    738,   1400,  770,   260,   // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     821,   129,   2,     // Bishop Pair
//    91,    77,    3,     7,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    5,     72,    103,   11,    // Abstract Piece Activity Bonus
//    123,   14,    39,    5,     // Abstract Piece Activity Bonus
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
//    -285,  -1,    -1,    -38,   // Double Pawn Penalty
//    2,     17,    182,   64,    // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    -24,   58,    -104,  -20,   // King Vector Exposure
//    123,   -38,   -140,  -10,   // King Vector Exposure
//    68,    254,   62,    139,   // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    0,     349,   540,   76,    // Pawn Attack
//    3,     5,     477,   121,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     -1,    -187,  -4,    // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    -3,    -2,    -181,  -4,    // King Attack Distance
//    -145,  -120,  -1,    -2,    // King Attack Distance
//    0,     1,     32,    8,     // King Attack Danger
//    1,     7,     16,    7,     // King Attack Danger
//    0,     1,     66,    13,    // King Attack Danger
//    2,     105,   1,     4,     // King Attack Danger
//    -879,  -976,  -726,  -138,  // Safe Knight Square Count
//    -121,  -686,  -350,  -78,   // Safe Knight Square Count
//    -102,  -286,  -232,  -40,   // Safe Knight Square Count
//    -70,   -84,   -97,   -24,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    78,    1,     157,   0,     // Safe Knight Square Count
//    78,    2,     270,   1,     // Safe Knight Square Count
//    78,    2,     276,   10,    // Safe Knight Square Count
//    78,    2,     277,   12,    // Safe Knight Square Count
//    128,   2888,  2083,  -132,  // Bishop Mobility
//    420,   2989,  2276,  -114,  // Bishop Mobility
//    425,   3264,  2514,  -48,   // Bishop Mobility
//    427,   3510,  2674,  -24,   // Bishop Mobility
//    428,   3731,  2823,  -18,   // Bishop Mobility
//    428,   3930,  2927,  6,     // Bishop Mobility
//    429,   4063,  3055,  6,     // Bishop Mobility
//    429,   4229,  3146,  10,    // Bishop Mobility
//    429,   4231,  3303,  12,    // Bishop Mobility
//    429,   4231,  3527,  13,    // Bishop Mobility
//    429,   4232,  3663,  14,    // Bishop Mobility
//    429,   4232,  3743,  14,    // Bishop Mobility
//    429,   4232,  3743,  14,    // Bishop Mobility
//    429,   4232,  3743,  14,    // Bishop Mobility
//    2590,  4714,  4323,  -1005, // Rook Mobility
//    2592,  4963,  4366,  -972,  // Rook Mobility
//    2596,  5136,  4418,  -958,  // Rook Mobility
//    2602,  5390,  4422,  -944,  // Rook Mobility
//    2604,  5555,  4425,  -944,  // Rook Mobility
//    2845,  5665,  4475,  -929,  // Rook Mobility
//    2848,  5811,  4560,  -902,  // Rook Mobility
//    2850,  5912,  4673,  -876,  // Rook Mobility
//    3146,  5929,  4764,  -865,  // Rook Mobility
//    3241,  5998,  4859,  -860,  // Rook Mobility
//    3282,  6041,  4951,  -827,  // Rook Mobility
//    3283,  6043,  5142,  -807,  // Rook Mobility
//    3283,  6045,  5242,  -543,  // Rook Mobility
//    3283,  6045,  5337,  -469,  // Rook Mobility
//    3283,  6045,  5338,  -469,  // Rook Mobility
//    3140,  13267, 5774,  2222,  // Queen Mobility
//    3182,  13282, 5846,  2329,  // Queen Mobility
//    3182,  13282, 5851,  2331,  // Queen Mobility
//    3182,  13282, 5851,  2331,  // Queen Mobility
//    3182,  13282, 5859,  2332,  // Queen Mobility
//    3186,  13356, 5890,  2332,  // Queen Mobility
//    3192,  13394, 5953,  2338,  // Queen Mobility
//    3192,  13489, 5953,  2338,  // Queen Mobility
//    3197,  13578, 5968,  2343,  // Queen Mobility
//    3201,  13677, 5984,  2347,  // Queen Mobility
//    3206,  13753, 5991,  2347,  // Queen Mobility
//    3223,  13861, 5991,  2348,  // Queen Mobility
//    3261,  13896, 5997,  2348,  // Queen Mobility
//    3289,  13933, 6006,  2349,  // Queen Mobility
//    3300,  13948, 6019,  2350,  // Queen Mobility
//    3300,  13948, 6022,  2350,  // Queen Mobility
//    3300,  13948, 6037,  2350,  // Queen Mobility
//    3300,  13948, 6048,  2351,  // Queen Mobility
//    3300,  13948, 6048,  2351,  // Queen Mobility
//    3300,  13948, 6049,  2351,  // Queen Mobility
//    3300,  13948, 6049,  2351,  // Queen Mobility
//    3300,  13948, 6049,  2351,  // Queen Mobility
//    3300,  13948, 6049,  2351,  // Queen Mobility
//    3300,  13948, 6049,  2351,  // Queen Mobility
//    3300,  13948, 6049,  2351,  // Queen Mobility
//    3300,  13948, 6049,  2351,  // Queen Mobility
//    3300,  13948, 6049,  2352,  // Queen Mobility
//    3300,  13949, 6049,  2352,  // Queen Mobility
//    -489,  162,   692,   -11,   // Safe Checks
//    -241,  233,   157,   9,     // Unsafe Checks
//    73,    50,    -262,  -22,   // Unprotected Pieces
//    108,   38,    -10,   515,   // Bishop's Position Relative To King
//    -662,  162,   410,   60,    // Bishop's Position Relative To King
//    -172,  173,   65,    103,   // Bishop's Position Relative To King
//    -55,   231,   -11,   -132,  // Bishop's Position Relative To King
//    234,   284,   -120,  -130,  // Bishop's Position Relative To King
//    142,   427,   -326,  -136,  // Bishop's Position Relative To King
//    -89,   340,   2,     -156,  // Bishop's Position Relative To King
//    16,    421,   -269,  -222,  // Bishop's Position Relative To King
//    52,    49,    172,   164,   // Bishop's Position Relative To King
//    -425,  389,   18,    -48,   // Bishop's Position Relative To King
//    -61,   105,   71,    135,   // Bishop's Position Relative To King
//    -90,   319,   -238,  -141,  // Bishop's Position Relative To King
//    218,   100,   16,    -65,   // Bishop's Position Relative To King
//    -54,   153,   -96,   -96,   // Bishop's Position Relative To King
//    0,     57,    101,   -61,   // Bishop's Position Relative To King
//    24,    158,   132,   150,   // Bishop's Position Relative To King
//    -193,  299,   -108,  -51,   // Bishop's Position Relative To King
//    163,   74,    -77,   30,    // Bishop's Position Relative To King
//    -31,   197,   -192,  -43,   // Bishop's Position Relative To King
//    254,   6,     10,    -83,   // Bishop's Position Relative To King
//    5,     75,    -56,   -61,   // Bishop's Position Relative To King
//    130,   155,   -101,  39,    // Bishop's Position Relative To King
//    -112,  170,   -35,   -26,   // Bishop's Position Relative To King
//    273,   7,     -55,   -83,   // Bishop's Position Relative To King
//    20,    111,   -238,  -59,   // Bishop's Position Relative To King
//    333,   -87,   105,   45,    // Bishop's Position Relative To King
//    53,    95,    227,   -39,   // Bishop's Position Relative To King
//    2,     -21,   162,   -131,  // Bishop's Position Relative To King
//    418,   -200,  104,   21,    // Bishop's Position Relative To King
//    59,    54,    -41,   -84,   // Bishop's Position Relative To King
//    -265,  82,    1227,  660,   // Bishop's Position Relative To King
//    -105,  -264,  786,   79,    // Bishop's Position Relative To King
//    197,   -467,  933,   116,   // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -10,   -90,   310,   -283,  // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -3,    -513,  -231,  -327,  // Opposed Pawns
//    -47,   -459,  -324,  -365,  // Opposed Pawns
//    -53,   -437,  -287,  -197,  // Opposed Pawns
//    -53,   -387,  -174,  -60,   // Opposed Pawns
//    -141,  -217,  273,   -152,  // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -143,  -362,  -105,  -254,  // Opposed Pawns
//    -116,  -339,  -187,  -197,  // Opposed Pawns
//    -122,  -394,  -61,   65,    // Opposed Pawns
//    -282,  272,   460,   -75,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -34,   -461,  -245,  -329,  // Opposed Pawns
//    -90,   -450,  -274,  -224,  // Opposed Pawns
//    -80,   -438,  -13,   -135,  // Opposed Pawns
//    -60,   -344,  351,   4,     // Opposed Pawns
//    504,   692,   760,   544,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -45,   -268,  -19,   -274,  // Opposed Pawns
//    -118,  -283,  -38,   -125,  // Opposed Pawns
//    -101,  -132,  456,   85,    // Opposed Pawns
//    67,    98,    145,   97,    // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -68,   -556,  -507,  -311,  // Unopposed Pawns
//    -122,  -509,  -798,  -303,  // Unopposed Pawns
//    -146,  -592,  -521,  -26,   // Unopposed Pawns
//    -197,  -461,  -30,   -2,    // Unopposed Pawns
//    -788,  -208,  924,   -111,  // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -133,  -386,  -287,  -173,  // Unopposed Pawns
//    -143,  -364,  -271,  -34,   // Unopposed Pawns
//    -124,  -278,  349,   178,   // Unopposed Pawns
//    474,   445,   1317,  543,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    20,    -566,  -382,  -257,  // Unopposed Pawns
//    -119,  -334,  -534,  -137,  // Unopposed Pawns
//    122,   -403,  -199,  50,    // Unopposed Pawns
//    241,   -46,   519,   34,    // Unopposed Pawns
//    292,   422,   568,   420,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -22,   -192,  -292,  -307,  // Unopposed Pawns
//    -92,   -265,  44,    0,     // Unopposed Pawns
//    114,   47,    911,   403,   // Unopposed Pawns
//    235,   347,   339,   219,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    261,   -593,  -488,  -438,  // Covered Passed Pawns
//    211,   -389,  -975,  -354,  // Covered Passed Pawns
//    239,   -170,  -849,  -286,  // Covered Passed Pawns
//    112,   -97,   -290,  -26,   // Covered Passed Pawns
//    -41,   23,    367,   -81,   // Covered Passed Pawns
//    -821,  971,   -1053, -231,  // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    12,    -448,  -347,  -366,  // Covered Passed Pawns
//    128,   -126,  -515,  -336,  // Covered Passed Pawns
//    128,   67,    -72,   148,   // Covered Passed Pawns
//    -84,   258,   1018,  245,   // Covered Passed Pawns
//    251,   390,   -67,   -122,  // Covered Passed Pawns
//    77,    -712,  -280,  -418,  // Covered Passed Pawns
//    8,     -319,  -740,  -277,  // Covered Passed Pawns
//    78,    213,   -556,  -217,  // Covered Passed Pawns
//    -36,   464,   440,   32,    // Covered Passed Pawns
//    251,   713,   1014,  647,   // Covered Passed Pawns
//    29,    38,    27,    19,    // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    -435,  -519,  93,    -421,  // Covered Passed Pawns
//    -291,  -7,    -116,  131,   // Covered Passed Pawns
//    -102,  479,   440,   472,   // Covered Passed Pawns
//    464,   857,   618,   438,   // Covered Passed Pawns
//    2,     4,     2,     2,     // Covered Passed Pawns
//    -248,  -555,  -800,  -194,  // Uncovered Passed Pawns
//    26,    -620,  -740,  -324,  // Uncovered Passed Pawns
//    273,   -226,  -588,  0,     // Uncovered Passed Pawns
//    479,   274,   -186,  -67,   // Uncovered Passed Pawns
//    581,   771,   492,   -96,   // Uncovered Passed Pawns
//    119,   1099,  1652,  206,   // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    -841,  -323,  -400,  -274,  // Uncovered Passed Pawns
//    -324,  -55,   -269,  318,   // Uncovered Passed Pawns
//    -370,  891,   -108,  104,   // Uncovered Passed Pawns
//    -567,  1772,  783,   427,   // Uncovered Passed Pawns
//    425,   1514,  1526,  1074,  // Uncovered Passed Pawns
//    -519,  -446,  -1038, -149,  // Uncovered Passed Pawns
//    -639,  -535,  -608,  -147,  // Uncovered Passed Pawns
//    -73,   281,   -593,  -1,    // Uncovered Passed Pawns
//    281,   1000,  -81,   239,   // Uncovered Passed Pawns
//    1234,  1455,  840,   930,   // Uncovered Passed Pawns
//    1632,  1776,  1329,  1089,  // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    -344,  -337,  -258,  -177,  // Uncovered Passed Pawns
//    -208,  -227,  -326,  -180,  // Uncovered Passed Pawns
//    279,   427,   350,   305,   // Uncovered Passed Pawns
//    418,   615,   441,   322,   // Uncovered Passed Pawns
//    57,    69,    47,    30     // Uncovered Passed Pawns
//};



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

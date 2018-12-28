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


// 181227, mix 0.3
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
//const std::array<int, (270*4)> eval_weights = {
//    -441,  -671,  -53,   88,    // King Piece Square Table
//    -291,  -391,  206,   296,   // King Piece Square Table
//    -54,   -44,   -282,  -56,   // King Piece Square Table
//    -77,   63,    -524,  -81,   // King Piece Square Table
//    -55,   -132,  464,   -128,  // King Piece Square Table
//    98,    155,   228,   -339,  // King Piece Square Table
//    110,   397,   -214,  -461,  // King Piece Square Table
//    186,   242,   238,   -82,   // King Piece Square Table
//    214,   310,   153,   -197,  // King Piece Square Table
//    329,   249,   8,     102,   // King Piece Square Table
//    2242,  2411,  1840,  3918,  // Knight Piece Square Table
//    2075,  2533,  2673,  4249,  // Knight Piece Square Table
//    2215,  2649,  2540,  4213,  // Knight Piece Square Table
//    2127,  2723,  2825,  4286,  // Knight Piece Square Table
//    2294,  2750,  2549,  4057,  // Knight Piece Square Table
//    2143,  2780,  2742,  4396,  // Knight Piece Square Table
//    2298,  2772,  2778,  4454,  // Knight Piece Square Table
//    2268,  2834,  2843,  4482,  // Knight Piece Square Table
//    2263,  2940,  2903,  4425,  // Knight Piece Square Table
//    2264,  3041,  2797,  4438,  // Knight Piece Square Table
//    960,   634,   183,   465,   // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    353,   188,   139,   1200,  // Bishop Pair
//    139,   3,     0,     30,    // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     78,    61,    33,    // Abstract Piece Activity Bonus
//    58,    30,    25,    6,     // Abstract Piece Activity Bonus
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
//    -94,   0,     0,     0,     // Double Pawn Penalty
//    100,   100,   100,   100,   // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    22,    44,    -127,  -43,   // King Vector Exposure
//    -46,   47,    -111,  -24,   // King Vector Exposure
//    -22,   336,   48,    192,   // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    0,     74,    789,   519,   // Pawn Attack
//    0,     2,     572,   400,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     -63,   0,     // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    0,     0,     -78,   0,     // King Attack Distance
//    -162,  -114,  -4,    0,     // King Attack Distance
//    0,     0,     17,    6,     // King Attack Danger
//    0,     0,     20,    8,     // King Attack Danger
//    0,     0,     58,    36,    // King Attack Danger
//    0,     115,   48,    0,     // King Attack Danger
//    -606,  -604,  -532,  -323,  // Safe Knight Square Count
//    -64,   -79,   -465,  -186,  // Safe Knight Square Count
//    -1,    -73,   -220,  -114,  // Safe Knight Square Count
//    0,     -7,    -129,  -69,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    3,     0,     122,   42,    // Safe Knight Square Count
//    3,     0,     172,   86,    // Safe Knight Square Count
//    3,     0,     172,   91,    // Safe Knight Square Count
//    3,     0,     172,   91,    // Safe Knight Square Count
//    1579,  2035,  1573,  2390,  // Bishop Mobility
//    1907,  2115,  1843,  2551,  // Bishop Mobility
//    2095,  2188,  2086,  2648,  // Bishop Mobility
//    2142,  2290,  2239,  2739,  // Bishop Mobility
//    2249,  2389,  2298,  2808,  // Bishop Mobility
//    2249,  2487,  2421,  2861,  // Bishop Mobility
//    2249,  2583,  2540,  2908,  // Bishop Mobility
//    2258,  2700,  2545,  2962,  // Bishop Mobility
//    2280,  2750,  2651,  2976,  // Bishop Mobility
//    2290,  2758,  2764,  2999,  // Bishop Mobility
//    2290,  2759,  2764,  3128,  // Bishop Mobility
//    2292,  2767,  2881,  3270,  // Bishop Mobility
//    2486,  2767,  2881,  3278,  // Bishop Mobility
//    2497,  2782,  2894,  3293,  // Bishop Mobility
//    4135,  4216,  2973,  4563,  // Rook Mobility
//    4297,  4430,  3178,  4676,  // Rook Mobility
//    4494,  4680,  3215,  4718,  // Rook Mobility
//    4666,  4760,  3247,  4770,  // Rook Mobility
//    4789,  4848,  3252,  4784,  // Rook Mobility
//    4987,  4862,  3353,  4856,  // Rook Mobility
//    5035,  4946,  3438,  4914,  // Rook Mobility
//    5048,  4947,  3561,  4956,  // Rook Mobility
//    5075,  4955,  3730,  5035,  // Rook Mobility
//    5095,  5010,  3825,  5038,  // Rook Mobility
//    5108,  5045,  3872,  5214,  // Rook Mobility
//    5191,  5049,  3982,  5229,  // Rook Mobility
//    5301,  5057,  4042,  5293,  // Rook Mobility
//    5301,  5066,  4065,  5435,  // Rook Mobility
//    5301,  5128,  4065,  5435,  // Rook Mobility
//    8932,  9003,  8220,  7832,  // Queen Mobility
//    9116,  9195,  8304,  7912,  // Queen Mobility
//    9158,  9235,  8330,  8009,  // Queen Mobility
//    9163,  9237,  8330,  8009,  // Queen Mobility
//    9246,  9317,  8348,  8060,  // Queen Mobility
//    9282,  9423,  8370,  8074,  // Queen Mobility
//    9298,  9441,  8382,  8074,  // Queen Mobility
//    9331,  9476,  8384,  8074,  // Queen Mobility
//    9380,  9514,  8384,  8074,  // Queen Mobility
//    9418,  9571,  8388,  8074,  // Queen Mobility
//    9481,  9608,  8391,  8074,  // Queen Mobility
//    9509,  9620,  8392,  8074,  // Queen Mobility
//    9509,  9623,  8392,  8074,  // Queen Mobility
//    9609,  9708,  8392,  8074,  // Queen Mobility
//    9613,  9711,  8392,  8074,  // Queen Mobility
//    9637,  9751,  8394,  8074,  // Queen Mobility
//    9673,  9805,  8422,  8074,  // Queen Mobility
//    9673,  9805,  8437,  8084,  // Queen Mobility
//    9677,  9807,  8465,  8141,  // Queen Mobility
//    9677,  9807,  8513,  8296,  // Queen Mobility
//    9677,  9807,  8518,  8302,  // Queen Mobility
//    9677,  9807,  8526,  8367,  // Queen Mobility
//    9677,  9807,  8526,  8367,  // Queen Mobility
//    9677,  9807,  8527,  8374,  // Queen Mobility
//    9677,  9808,  8527,  8374,  // Queen Mobility
//    9677,  9808,  8527,  8374,  // Queen Mobility
//    9678,  9808,  8531,  8387,  // Queen Mobility
//    9678,  9808,  8531,  8387,  // Queen Mobility
//    -405,  318,   708,   61,    // Safe Checks
//    -201,  87,    279,   -3,    // Unsafe Checks
//    259,   -22,   -342,  -54,   // Unprotected Pieces
//    294,   296,   256,   256,   // Bishop's Position Relative To King
//    368,   333,   164,   231,   // Bishop's Position Relative To King
//    383,   553,   316,   143,   // Bishop's Position Relative To King
//    282,   376,   312,   188,   // Bishop's Position Relative To King
//    566,   350,   151,   328,   // Bishop's Position Relative To King
//    498,   406,   68,    157,   // Bishop's Position Relative To King
//    719,   603,   -50,   103,   // Bishop's Position Relative To King
//    437,   412,   101,   22,    // Bishop's Position Relative To King
//    485,   515,   479,   430,   // Bishop's Position Relative To King
//    512,   270,   258,   277,   // Bishop's Position Relative To King
//    494,   299,   344,   349,   // Bishop's Position Relative To King
//    504,   325,   138,   210,   // Bishop's Position Relative To King
//    593,   361,   236,   302,   // Bishop's Position Relative To King
//    389,   315,   202,   216,   // Bishop's Position Relative To King
//    447,   390,   384,   293,   // Bishop's Position Relative To King
//    560,   587,   325,   437,   // Bishop's Position Relative To King
//    418,   358,   269,   199,   // Bishop's Position Relative To King
//    447,   487,   275,   228,   // Bishop's Position Relative To King
//    528,   266,   188,   193,   // Bishop's Position Relative To King
//    542,   339,   382,   200,   // Bishop's Position Relative To King
//    460,   160,   453,   159,   // Bishop's Position Relative To King
//    633,   592,   440,   173,   // Bishop's Position Relative To King
//    264,   409,   311,   162,   // Bishop's Position Relative To King
//    501,   371,   348,   158,   // Bishop's Position Relative To King
//    437,   334,   119,   109,   // Bishop's Position Relative To King
//    452,   494,   291,   168,   // Bishop's Position Relative To King
//    538,   784,   286,   183,   // Bishop's Position Relative To King
//    210,   408,   375,   72,    // Bishop's Position Relative To King
//    458,   299,   448,   56,    // Bishop's Position Relative To King
//    155,   321,   386,   125,   // Bishop's Position Relative To King
//    392,   682,   1112,  543,   // Bishop's Position Relative To King
//    -23,   285,   671,   243,   // Bishop's Position Relative To King
//    348,   373,   382,   270,   // Bishop's Position Relative To King
//    879,   858,   966,   1088,  // Bishop's Position Relative To King
//    -17,   272,   493,   -198,  // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -79,   24,    157,   -22,   // Opposed Pawns
//    -18,   -14,   20,    -91,   // Opposed Pawns
//    -84,   21,    84,    -5,    // Opposed Pawns
//    -34,   150,   16,    81,    // Opposed Pawns
//    -203,  381,   434,   231,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    21,    134,   182,   97,    // Opposed Pawns
//    -19,   158,   145,   141,   // Opposed Pawns
//    127,   232,   89,    319,   // Opposed Pawns
//    260,   815,   490,   194,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -33,   -39,   222,   44,    // Opposed Pawns
//    -60,   13,    79,    42,    // Opposed Pawns
//    -11,   54,    124,   167,   // Opposed Pawns
//    108,   64,    307,   167,   // Opposed Pawns
//    658,   707,   681,   697,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    113,   190,   227,   120,   // Opposed Pawns
//    -92,   206,   324,   291,   // Opposed Pawns
//    267,   338,   557,   398,   // Opposed Pawns
//    495,   494,   492,   491,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -131,  3,     149,   119,   // Unopposed Pawns
//    -151,  58,    -27,   -45,   // Unopposed Pawns
//    -131,  41,    16,    346,   // Unopposed Pawns
//    -68,   120,   362,   318,   // Unopposed Pawns
//    447,   538,   682,   869,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -77,   243,   259,   354,   // Unopposed Pawns
//    50,    197,   271,   647,   // Unopposed Pawns
//    288,   388,   487,   658,   // Unopposed Pawns
//    1129,  1016,  969,   998,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -53,   15,    241,   262,   // Unopposed Pawns
//    45,    123,   44,    268,   // Unopposed Pawns
//    236,   103,   216,   495,   // Unopposed Pawns
//    498,   361,   737,   586,   // Unopposed Pawns
//    800,   883,   869,   868,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    133,   292,   371,   419,   // Unopposed Pawns
//    411,   568,   312,   590,   // Unopposed Pawns
//    866,   654,   659,   696,   // Unopposed Pawns
//    160,   160,   159,   159,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -3,    100,   235,   73,    // Covered Passed Pawns
//    23,    186,   -87,   120,   // Covered Passed Pawns
//    211,   336,   -82,   178,   // Covered Passed Pawns
//    183,   535,   170,   436,   // Covered Passed Pawns
//    -21,   737,   1168,  533,   // Covered Passed Pawns
//    -288,  382,   1413,  1544,  // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    -53,   313,   553,   232,   // Covered Passed Pawns
//    296,   666,   73,    421,   // Covered Passed Pawns
//    428,   948,   298,   481,   // Covered Passed Pawns
//    692,   1380,  1338,  1241,  // Covered Passed Pawns
//    1022,  1077,  1011,  995,   // Covered Passed Pawns
//    -111,  118,   385,   182,   // Covered Passed Pawns
//    102,   211,   135,   207,   // Covered Passed Pawns
//    351,   686,   15,    182,   // Covered Passed Pawns
//    537,   1040,  527,   636,   // Covered Passed Pawns
//    1525,  1517,  1436,  1430,  // Covered Passed Pawns
//    84,    84,    84,    84,    // Covered Passed Pawns
//    0,     118,   385,   182,   // Covered Passed Pawns
//    102,   313,   553,   232,   // Covered Passed Pawns
//    351,   686,   73,    421,   // Covered Passed Pawns
//    537,   1040,  527,   636,   // Covered Passed Pawns
//    1525,  1517,  1436,  1430,  // Covered Passed Pawns
//    1022,  1077,  1011,  995,   // Covered Passed Pawns
//    299,   85,    -253,  -120,  // Uncovered Passed Pawns
//    411,   59,    -65,   -54,   // Uncovered Passed Pawns
//    1090,  250,   -315,  95,    // Uncovered Passed Pawns
//    1911,  496,   -204,  106,   // Uncovered Passed Pawns
//    3309,  829,   -2,    100,   // Uncovered Passed Pawns
//    3683,  1606,  388,   750,   // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    132,   358,   59,    36,    // Uncovered Passed Pawns
//    766,   482,   100,   413,   // Uncovered Passed Pawns
//    1598,  1068,  42,    52,    // Uncovered Passed Pawns
//    2334,  1818,  951,   1047,  // Uncovered Passed Pawns
//    2501,  2805,  2728,  2688,  // Uncovered Passed Pawns
//    280,   134,   -73,   -195,  // Uncovered Passed Pawns
//    -19,   116,   101,   103,   // Uncovered Passed Pawns
//    743,   549,   216,   387,   // Uncovered Passed Pawns
//    1313,  1097,  890,   992,   // Uncovered Passed Pawns
//    2357,  2269,  2112,  2160,  // Uncovered Passed Pawns
//    2535,  2475,  2465,  2465,  // Uncovered Passed Pawns
//    280,   134,   0,     0,     // Uncovered Passed Pawns
//    132,   358,   101,   103,   // Uncovered Passed Pawns
//    766,   549,   216,   413,   // Uncovered Passed Pawns
//    1598,  1097,  890,   992,   // Uncovered Passed Pawns
//    2357,  2269,  2112,  2160,  // Uncovered Passed Pawns
//    2535,  2805,  2728,  2688   // Uncovered Passed Pawns
//};

// 181228 Pure SL
//const std::array<int, (270*4)> eval_weights = {
//    -556,  -731,  -132,  113,   // King Piece Square Table
//    -344,  -461,  185,   305,   // King Piece Square Table
//    -76,   -61,   -309,  -136,  // King Piece Square Table
//    -103,  25,    -530,  -108,  // King Piece Square Table
//    -95,   -141,  422,   -182,  // King Piece Square Table
//    100,   164,   243,   -549,  // King Piece Square Table
//    107,   446,   -163,  -761,  // King Piece Square Table
//    193,   267,   170,   -125,  // King Piece Square Table
//    245,   298,   195,   -208,  // King Piece Square Table
//    430,   202,   18,    112,   // King Piece Square Table
//    2126,  2281,  2259,  4732,  // Knight Piece Square Table
//    1958,  2535,  3273,  5042,  // Knight Piece Square Table
//    2167,  2601,  3122,  5014,  // Knight Piece Square Table
//    2115,  2682,  3411,  5091,  // Knight Piece Square Table
//    2287,  2749,  3100,  4793,  // Knight Piece Square Table
//    2134,  2748,  3311,  5208,  // Knight Piece Square Table
//    2362,  2733,  3330,  5281,  // Knight Piece Square Table
//    2344,  2794,  3412,  5299,  // Knight Piece Square Table
//    2324,  2882,  3534,  5260,  // Knight Piece Square Table
//    2303,  3019,  3421,  5262,  // Knight Piece Square Table
//    987,   680,   304,   576,   // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    219,   183,   176,   1484,  // Bishop Pair
//    138,   10,    0,     38,    // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     84,    73,    33,    // Abstract Piece Activity Bonus
//    77,    29,    28,    5,     // Abstract Piece Activity Bonus
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
//    -134,  0,     0,     0,     // Double Pawn Penalty
//    100,   100,   100,   100,   // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    18,    62,    -148,  -50,   // King Vector Exposure
//    6,     29,    -132,  -25,   // King Vector Exposure
//    50,    384,   28,    212,   // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    0,     17,    911,   581,   // Pawn Attack
//    0,     0,     583,   455,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     -56,   0,     // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    0,     0,     -91,   0,     // King Attack Distance
//    -242,  -50,   0,     0,     // King Attack Distance
//    0,     0,     17,    1,     // King Attack Danger
//    0,     0,     18,    13,    // King Attack Danger
//    0,     0,     53,    48,    // King Attack Danger
//    0,     74,    90,    0,     // King Attack Danger
//    -760,  -765,  -640,  -362,  // Safe Knight Square Count
//    -84,   -67,   -553,  -226,  // Safe Knight Square Count
//    0,     -51,   -297,  -142,  // Safe Knight Square Count
//    0,     0,     -174,  -87,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    0,     0,     133,   41,    // Safe Knight Square Count
//    0,     0,     159,   91,    // Safe Knight Square Count
//    0,     0,     159,   91,    // Safe Knight Square Count
//    0,     0,     159,   91,    // Safe Knight Square Count
//    1649,  2054,  1983,  2881,  // Bishop Mobility
//    2001,  2141,  2271,  3058,  // Bishop Mobility
//    2194,  2233,  2548,  3170,  // Bishop Mobility
//    2233,  2350,  2741,  3267,  // Bishop Mobility
//    2377,  2469,  2785,  3345,  // Bishop Mobility
//    2377,  2585,  2930,  3404,  // Bishop Mobility
//    2377,  2704,  3071,  3454,  // Bishop Mobility
//    2410,  2827,  3076,  3517,  // Bishop Mobility
//    2454,  2867,  3204,  3522,  // Bishop Mobility
//    2462,  2871,  3321,  3552,  // Bishop Mobility
//    2462,  2871,  3321,  3687,  // Bishop Mobility
//    2465,  2873,  3416,  3829,  // Bishop Mobility
//    2736,  2876,  3416,  3829,  // Bishop Mobility
//    2738,  2878,  3416,  3829,  // Bishop Mobility
//    4071,  4315,  3653,  5486,  // Rook Mobility
//    4165,  4511,  3963,  5616,  // Rook Mobility
//    4438,  4812,  3993,  5653,  // Rook Mobility
//    4611,  4888,  4045,  5725,  // Rook Mobility
//    4720,  5018,  4051,  5739,  // Rook Mobility
//    4976,  5052,  4133,  5833,  // Rook Mobility
//    5027,  5159,  4227,  5887,  // Rook Mobility
//    5041,  5159,  4374,  5941,  // Rook Mobility
//    5065,  5166,  4607,  6015,  // Rook Mobility
//    5081,  5227,  4729,  6015,  // Rook Mobility
//    5099,  5269,  4782,  6245,  // Rook Mobility
//    5191,  5273,  4939,  6261,  // Rook Mobility
//    5294,  5299,  4985,  6388,  // Rook Mobility
//    5294,  5321,  5019,  6529,  // Rook Mobility
//    5294,  5397,  5020,  6529,  // Rook Mobility
//    8832,  9436,  9365,  9882,  // Queen Mobility
//    9054,  9651,  9480,  9961,  // Queen Mobility
//    9090,  9695,  9526,  10084, // Queen Mobility
//    9106,  9707,  9526,  10084, // Queen Mobility
//    9156,  9763,  9565,  10162, // Queen Mobility
//    9212,  9904,  9612,  10166, // Queen Mobility
//    9221,  9914,  9620,  10166, // Queen Mobility
//    9241,  9932,  9621,  10166, // Queen Mobility
//    9285,  9964,  9621,  10166, // Queen Mobility
//    9329,  10032, 9631,  10166, // Queen Mobility
//    9413,  10099, 9634,  10166, // Queen Mobility
//    9424,  10110, 9637,  10166, // Queen Mobility
//    9424,  10110, 9637,  10166, // Queen Mobility
//    9539,  10208, 9637,  10166, // Queen Mobility
//    9543,  10211, 9637,  10166, // Queen Mobility
//    9591,  10272, 9641,  10166, // Queen Mobility
//    9623,  10321, 9688,  10167, // Queen Mobility
//    9624,  10321, 9703,  10186, // Queen Mobility
//    9636,  10324, 9750,  10257, // Queen Mobility
//    9636,  10324, 9814,  10453, // Queen Mobility
//    9636,  10325, 9821,  10461, // Queen Mobility
//    9638,  10326, 9841,  10525, // Queen Mobility
//    9638,  10326, 9841,  10525, // Queen Mobility
//    9638,  10326, 9852,  10548, // Queen Mobility
//    9638,  10326, 9852,  10548, // Queen Mobility
//    9638,  10326, 9852,  10548, // Queen Mobility
//    9642,  10327, 9868,  10575, // Queen Mobility
//    9642,  10327, 9868,  10575, // Queen Mobility
//    -529,  306,   907,   53,    // Safe Checks
//    -268,  113,   344,   -8,    // Unsafe Checks
//    386,   -51,   -416,  -48,   // Unprotected Pieces
//    227,   234,   190,   192,   // Bishop's Position Relative To King
//    387,   338,   68,    227,   // Bishop's Position Relative To King
//    417,   600,   320,   119,   // Bishop's Position Relative To King
//    229,   347,   415,   201,   // Bishop's Position Relative To King
//    529,   341,   185,   416,   // Bishop's Position Relative To King
//    530,   440,   55,    173,   // Bishop's Position Relative To King
//    682,   621,   1,     130,   // Bishop's Position Relative To King
//    356,   401,   182,   23,    // Bishop's Position Relative To King
//    498,   533,   508,   536,   // Bishop's Position Relative To King
//    513,   261,   256,   303,   // Bishop's Position Relative To King
//    440,   274,   412,   397,   // Bishop's Position Relative To King
//    479,   330,   190,   238,   // Bishop's Position Relative To King
//    575,   348,   276,   343,   // Bishop's Position Relative To King
//    244,   325,   273,   246,   // Bishop's Position Relative To King
//    425,   374,   415,   320,   // Bishop's Position Relative To King
//    517,   587,   299,   515,   // Bishop's Position Relative To King
//    388,   314,   352,   202,   // Bishop's Position Relative To King
//    382,   524,   304,   241,   // Bishop's Position Relative To King
//    466,   292,   195,   237,   // Bishop's Position Relative To King
//    517,   350,   399,   231,   // Bishop's Position Relative To King
//    401,   117,   545,   192,   // Bishop's Position Relative To King
//    592,   579,   548,   163,   // Bishop's Position Relative To King
//    183,   400,   346,   235,   // Bishop's Position Relative To King
//    458,   370,   393,   202,   // Bishop's Position Relative To King
//    454,   297,   118,   145,   // Bishop's Position Relative To King
//    496,   502,   314,   179,   // Bishop's Position Relative To King
//    531,   808,   338,   208,   // Bishop's Position Relative To King
//    156,   398,   460,   63,    // Bishop's Position Relative To King
//    432,   268,   561,   60,    // Bishop's Position Relative To King
//    55,    317,   500,   142,   // Bishop's Position Relative To King
//    408,   775,   1193,  771,   // Bishop's Position Relative To King
//    -82,   256,   759,   337,   // Bishop's Position Relative To King
//    390,   388,   371,   267,   // Bishop's Position Relative To King
//    1140,  1131,  1180,  1391,  // Bishop's Position Relative To King
//    -57,   273,   513,   -148,  // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -88,   -22,   122,   -46,   // Opposed Pawns
//    -38,   -45,   -18,   -111,  // Opposed Pawns
//    -105,  -3,    60,    -36,   // Opposed Pawns
//    -59,   104,   21,    49,    // Opposed Pawns
//    -215,  345,   395,   209,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -16,   93,    166,   89,    // Opposed Pawns
//    -52,   132,   120,   125,   // Opposed Pawns
//    97,    199,   59,    331,   // Opposed Pawns
//    202,   808,   508,   210,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -40,   -74,   182,   26,    // Opposed Pawns
//    -91,   -13,   40,    31,    // Opposed Pawns
//    -35,   12,    112,   165,   // Opposed Pawns
//    17,    13,    312,   83,    // Opposed Pawns
//    970,   1021,  985,   1015,  // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    115,   117,   215,   115,   // Opposed Pawns
//    -190,  205,   334,   281,   // Opposed Pawns
//    296,   378,   595,   429,   // Opposed Pawns
//    469,   467,   469,   466,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -137,  -39,   128,   89,    // Unopposed Pawns
//    -170,  25,    -64,   -83,   // Unopposed Pawns
//    -154,  -2,    -17,   363,   // Unopposed Pawns
//    -103,  52,    429,   344,   // Unopposed Pawns
//    350,   530,   583,   1461,  // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -73,   199,   250,   375,   // Unopposed Pawns
//    19,    144,   279,   704,   // Unopposed Pawns
//    232,   324,   530,   717,   // Unopposed Pawns
//    1280,  1143,  1116,  1146,  // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -25,   -21,   225,   263,   // Unopposed Pawns
//    40,    99,    18,    288,   // Unopposed Pawns
//    266,   32,    230,   516,   // Unopposed Pawns
//    441,   322,   894,   726,   // Unopposed Pawns
//    1018,  1085,  1054,  1063,  // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    160,   229,   393,   478,   // Unopposed Pawns
//    465,   609,   292,   577,   // Unopposed Pawns
//    875,   740,   751,   815,   // Unopposed Pawns
//    199,   196,   196,   197,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    66,    54,    173,   73,    // Covered Passed Pawns
//    124,   124,   -169,  104,   // Covered Passed Pawns
//    270,   287,   -136,  157,   // Covered Passed Pawns
//    201,   481,   117,   458,   // Covered Passed Pawns
//    5,     677,   1100,  679,   // Covered Passed Pawns
//    -153,  402,   1366,  1578,  // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    4,     310,   530,   202,   // Covered Passed Pawns
//    296,   670,   94,    383,   // Covered Passed Pawns
//    386,   924,   314,   477,   // Covered Passed Pawns
//    580,   1389,  1430,  1302,  // Covered Passed Pawns
//    1108,  1178,  1099,  1076,  // Covered Passed Pawns
//    -86,   107,   363,   183,   // Covered Passed Pawns
//    103,   197,   144,   197,   // Covered Passed Pawns
//    403,   673,   5,     210,   // Covered Passed Pawns
//    522,   1043,  586,   729,   // Covered Passed Pawns
//    1792,  1806,  1740,  1733,  // Covered Passed Pawns
//    61,    60,    60,    60,    // Covered Passed Pawns
//    0,     107,   363,   183,   // Covered Passed Pawns
//    103,   310,   530,   202,   // Covered Passed Pawns
//    403,   673,   94,    383,   // Covered Passed Pawns
//    522,   1043,  586,   729,   // Covered Passed Pawns
//    1792,  1806,  1740,  1733,  // Covered Passed Pawns
//    1108,  1178,  1099,  1076,  // Covered Passed Pawns
//    329,   46,    -285,  -148,  // Uncovered Passed Pawns
//    467,   11,    -114,  -51,   // Uncovered Passed Pawns
//    1094,  267,   -356,  110,   // Uncovered Passed Pawns
//    2012,  458,   -213,  63,    // Uncovered Passed Pawns
//    3345,  905,   34,    114,   // Uncovered Passed Pawns
//    3580,  1719,  511,   696,   // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    139,   394,   24,    27,    // Uncovered Passed Pawns
//    699,   546,   128,   470,   // Uncovered Passed Pawns
//    1583,  1152,  -9,    26,    // Uncovered Passed Pawns
//    2243,  1967,  1070,  1116,  // Uncovered Passed Pawns
//    3097,  3315,  3187,  3158,  // Uncovered Passed Pawns
//    322,   117,   -62,   -241,  // Uncovered Passed Pawns
//    -67,   82,    65,    81,    // Uncovered Passed Pawns
//    689,   621,   296,   437,   // Uncovered Passed Pawns
//    1377,  1244,  1077,  1175,  // Uncovered Passed Pawns
//    3073,  3054,  2912,  2954,  // Uncovered Passed Pawns
//    2628,  2573,  2562,  2559,  // Uncovered Passed Pawns
//    322,   117,   0,     0,     // Uncovered Passed Pawns
//    139,   394,   65,    81,    // Uncovered Passed Pawns
//    699,   621,   296,   470,   // Uncovered Passed Pawns
//    1583,  1244,  1077,  1175,  // Uncovered Passed Pawns
//    3073,  3054,  2912,  2954,  // Uncovered Passed Pawns
//    3097,  3315,  3187,  3158   // Uncovered Passed Pawns
//};

// 0.5 mix 181231, best
//const std::array<int, (271*4)> eval_weights = {
//    -441,  -667,  -56,   84,    // King Piece Square Table
//    -290,  -389,  205,   291,   // King Piece Square Table
//    -53,   -44,   -279,  -62,   // King Piece Square Table
//    -76,   62,    -522,  -77,   // King Piece Square Table
//    -55,   -127,  462,   -118,  // King Piece Square Table
//    99,    156,   229,   -343,  // King Piece Square Table
//    110,   395,   -211,  -452,  // King Piece Square Table
//    184,   239,   242,   -80,   // King Piece Square Table
//    214,   309,   152,   -191,  // King Piece Square Table
//    329,   248,   11,    107,   // King Piece Square Table
//    2241,  2419,  1857,  3908,  // Knight Piece Square Table
//    2081,  2538,  2686,  4226,  // Knight Piece Square Table
//    2218,  2656,  2558,  4190,  // Knight Piece Square Table
//    2131,  2724,  2841,  4266,  // Knight Piece Square Table
//    2301,  2753,  2566,  4020,  // Knight Piece Square Table
//    2144,  2788,  2756,  4382,  // Knight Piece Square Table
//    2293,  2782,  2793,  4439,  // Knight Piece Square Table
//    2270,  2837,  2854,  4459,  // Knight Piece Square Table
//    2264,  2943,  2916,  4408,  // Knight Piece Square Table
//    2261,  3050,  2808,  4428,  // Knight Piece Square Table
//    947,   648,   183,   473,   // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    346,   190,   137,   1215,  // Bishop Pair
//    138,   1,     0,     28,    // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     79,    61,    34,    // Abstract Piece Activity Bonus
//    58,    30,    24,    6,     // Abstract Piece Activity Bonus
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
//    -13,   0,     0,     0,     // Double Pawn Penalty
//    -17,   -94,   34,    -124,  // Isolated Pawn
//    100,   100,   100,   100,   // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    23,    44,    -127,  -44,   // King Vector Exposure
//    -46,   48,    -110,  -24,   // King Vector Exposure
//    -13,   340,   51,    147,   // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    0,     73,    786,   514,   // Pawn Attack
//    0,     2,     572,   399,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     -64,   0,     // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    0,     0,     -78,   0,     // King Attack Distance
//    -164,  -112,  -4,    0,     // King Attack Distance
//    0,     0,     16,    6,     // King Attack Danger
//    0,     0,     20,    9,     // King Attack Danger
//    0,     0,     58,    36,    // King Attack Danger
//    0,     116,   47,    0,     // King Attack Danger
//    -596,  -596,  -531,  -307,  // Safe Knight Square Count
//    -56,   -77,   -467,  -183,  // Safe Knight Square Count
//    -1,    -72,   -219,  -114,  // Safe Knight Square Count
//    0,     -7,    -129,  -70,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    2,     0,     123,   43,    // Safe Knight Square Count
//    2,     0,     175,   88,    // Safe Knight Square Count
//    2,     0,     175,   94,    // Safe Knight Square Count
//    2,     0,     175,   94,    // Safe Knight Square Count
//    1583,  2046,  1566,  2389,  // Bishop Mobility
//    1904,  2123,  1844,  2558,  // Bishop Mobility
//    2092,  2199,  2084,  2653,  // Bishop Mobility
//    2142,  2300,  2239,  2744,  // Bishop Mobility
//    2252,  2400,  2295,  2813,  // Bishop Mobility
//    2252,  2495,  2421,  2865,  // Bishop Mobility
//    2252,  2591,  2538,  2914,  // Bishop Mobility
//    2260,  2707,  2544,  2966,  // Bishop Mobility
//    2282,  2758,  2651,  2979,  // Bishop Mobility
//    2291,  2766,  2760,  3004,  // Bishop Mobility
//    2292,  2766,  2760,  3140,  // Bishop Mobility
//    2294,  2773,  2879,  3277,  // Bishop Mobility
//    2487,  2773,  2879,  3286,  // Bishop Mobility
//    2498,  2788,  2892,  3301,  // Bishop Mobility
//    4148,  4236,  2965,  4588,  // Rook Mobility
//    4307,  4445,  3174,  4694,  // Rook Mobility
//    4498,  4693,  3212,  4734,  // Rook Mobility
//    4671,  4776,  3244,  4781,  // Rook Mobility
//    4797,  4862,  3249,  4794,  // Rook Mobility
//    4996,  4877,  3353,  4860,  // Rook Mobility
//    5047,  4960,  3440,  4913,  // Rook Mobility
//    5061,  4960,  3565,  4949,  // Rook Mobility
//    5091,  4969,  3732,  5029,  // Rook Mobility
//    5108,  5025,  3827,  5033,  // Rook Mobility
//    5120,  5060,  3875,  5211,  // Rook Mobility
//    5200,  5064,  3986,  5228,  // Rook Mobility
//    5304,  5072,  4043,  5303,  // Rook Mobility
//    5304,  5080,  4067,  5445,  // Rook Mobility
//    5304,  5148,  4068,  5445,  // Rook Mobility
//    8951,  9030,  8243,  7845,  // Queen Mobility
//    9132,  9218,  8322,  7925,  // Queen Mobility
//    9172,  9255,  8345,  8018,  // Queen Mobility
//    9174,  9257,  8345,  8018,  // Queen Mobility
//    9258,  9335,  8362,  8068,  // Queen Mobility
//    9295,  9445,  8384,  8079,  // Queen Mobility
//    9311,  9461,  8395,  8079,  // Queen Mobility
//    9341,  9495,  8398,  8079,  // Queen Mobility
//    9391,  9533,  8399,  8079,  // Queen Mobility
//    9429,  9591,  8402,  8079,  // Queen Mobility
//    9489,  9627,  8404,  8079,  // Queen Mobility
//    9522,  9639,  8405,  8079,  // Queen Mobility
//    9522,  9641,  8405,  8079,  // Queen Mobility
//    9620,  9727,  8406,  8079,  // Queen Mobility
//    9623,  9731,  8406,  8079,  // Queen Mobility
//    9648,  9769,  8407,  8079,  // Queen Mobility
//    9683,  9823,  8436,  8079,  // Queen Mobility
//    9683,  9823,  8450,  8091,  // Queen Mobility
//    9687,  9825,  8476,  8154,  // Queen Mobility
//    9687,  9825,  8525,  8307,  // Queen Mobility
//    9687,  9825,  8530,  8313,  // Queen Mobility
//    9687,  9825,  8538,  8378,  // Queen Mobility
//    9687,  9825,  8539,  8378,  // Queen Mobility
//    9687,  9825,  8539,  8385,  // Queen Mobility
//    9687,  9825,  8539,  8385,  // Queen Mobility
//    9687,  9825,  8539,  8385,  // Queen Mobility
//    9688,  9825,  8543,  8397,  // Queen Mobility
//    9688,  9825,  8543,  8397,  // Queen Mobility
//    -409,  321,   709,   62,    // Safe Checks
//    -202,  88,    279,   -3,    // Unsafe Checks
//    260,   -21,   -343,  -56,   // Unprotected Pieces
//    299,   299,   259,   259,   // Bishop's Position Relative To King
//    362,   332,   163,   247,   // Bishop's Position Relative To King
//    384,   552,   318,   143,   // Bishop's Position Relative To King
//    279,   378,   311,   185,   // Bishop's Position Relative To King
//    569,   349,   150,   324,   // Bishop's Position Relative To King
//    498,   405,   71,    156,   // Bishop's Position Relative To King
//    718,   604,   -48,   101,   // Bishop's Position Relative To King
//    439,   413,   102,   17,    // Bishop's Position Relative To King
//    477,   509,   482,   449,   // Bishop's Position Relative To King
//    510,   273,   257,   283,   // Bishop's Position Relative To King
//    489,   299,   342,   356,   // Bishop's Position Relative To King
//    505,   325,   137,   210,   // Bishop's Position Relative To King
//    595,   362,   232,   304,   // Bishop's Position Relative To King
//    389,   316,   202,   216,   // Bishop's Position Relative To King
//    449,   394,   380,   290,   // Bishop's Position Relative To King
//    558,   593,   321,   443,   // Bishop's Position Relative To King
//    422,   358,   273,   191,   // Bishop's Position Relative To King
//    445,   488,   272,   229,   // Bishop's Position Relative To King
//    525,   268,   186,   193,   // Bishop's Position Relative To King
//    545,   340,   377,   196,   // Bishop's Position Relative To King
//    462,   159,   454,   155,   // Bishop's Position Relative To King
//    633,   594,   436,   174,   // Bishop's Position Relative To King
//    265,   410,   313,   156,   // Bishop's Position Relative To King
//    500,   371,   350,   157,   // Bishop's Position Relative To King
//    440,   332,   115,   109,   // Bishop's Position Relative To King
//    454,   490,   291,   171,   // Bishop's Position Relative To King
//    536,   787,   284,   186,   // Bishop's Position Relative To King
//    207,   408,   380,   76,    // Bishop's Position Relative To King
//    459,   298,   445,   60,    // Bishop's Position Relative To King
//    156,   317,   392,   129,   // Bishop's Position Relative To King
//    392,   682,   1118,  542,   // Bishop's Position Relative To King
//    -25,   285,   677,   242,   // Bishop's Position Relative To King
//    347,   372,   383,   274,   // Bishop's Position Relative To King
//    886,   866,   968,   1077,  // Bishop's Position Relative To King
//    -21,   276,   489,   -193,  // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -94,   65,    157,   -7,    // Opposed Pawns
//    -34,   43,    27,    -62,   // Opposed Pawns
//    -91,   74,    74,    3,     // Opposed Pawns
//    -40,   197,   5,     87,    // Opposed Pawns
//    -208,  409,   430,   250,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    6,     107,   197,   98,    // Opposed Pawns
//    -37,   114,   150,   134,   // Opposed Pawns
//    107,   179,   97,    322,   // Opposed Pawns
//    246,   776,   477,   180,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -45,   -33,   227,   52,    // Opposed Pawns
//    -74,   15,    86,    50,    // Opposed Pawns
//    -22,   48,    130,   171,   // Opposed Pawns
//    103,   45,    313,   179,   // Opposed Pawns
//    636,   688,   664,   687,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    105,   158,   231,   116,   // Opposed Pawns
//    -103,  169,   320,   289,   // Opposed Pawns
//    258,   323,   545,   393,   // Opposed Pawns
//    494,   494,   490,   489,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -141,  8,     149,   107,   // Unopposed Pawns
//    -158,  84,    -25,   -24,   // Unopposed Pawns
//    -128,  77,    16,    342,   // Unopposed Pawns
//    -68,   138,   356,   327,   // Unopposed Pawns
//    437,   527,   675,   895,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -84,   167,   283,   305,   // Unopposed Pawns
//    41,    119,   288,   603,   // Unopposed Pawns
//    273,   316,   488,   622,   // Unopposed Pawns
//    1091,  975,   931,   955,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -61,   -35,   254,   202,   // Unopposed Pawns
//    35,    83,    58,    233,   // Unopposed Pawns
//    226,   67,    230,   461,   // Unopposed Pawns
//    483,   325,   717,   575,   // Unopposed Pawns
//    777,   860,   846,   855,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    108,   207,   369,   366,   // Unopposed Pawns
//    374,   498,   311,   567,   // Unopposed Pawns
//    831,   619,   631,   668,   // Unopposed Pawns
//    163,   163,   162,   162,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    15,    137,   228,   127,   // Covered Passed Pawns
//    41,    240,   -85,   181,   // Covered Passed Pawns
//    231,   396,   -87,   239,   // Covered Passed Pawns
//    202,   592,   145,   499,   // Covered Passed Pawns
//    -8,    779,   1162,  546,   // Covered Passed Pawns
//    -267,  406,   1404,  1539,  // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    -48,   267,   553,   226,   // Covered Passed Pawns
//    296,   610,   61,    426,   // Covered Passed Pawns
//    419,   888,   287,   469,   // Covered Passed Pawns
//    684,   1331,  1298,  1202,  // Covered Passed Pawns
//    1000,  1050,  984,   968,   // Covered Passed Pawns
//    -100,  109,   387,   196,   // Covered Passed Pawns
//    109,   207,   137,   210,   // Covered Passed Pawns
//    357,   678,   17,    177,   // Covered Passed Pawns
//    543,   1028,  520,   637,   // Covered Passed Pawns
//    1517,  1506,  1426,  1421,  // Covered Passed Pawns
//    84,    84,    84,    84,    // Covered Passed Pawns
//    0,     109,   387,   196,   // Covered Passed Pawns
//    109,   267,   553,   226,   // Covered Passed Pawns
//    357,   678,   61,    426,   // Covered Passed Pawns
//    543,   1028,  520,   637,   // Covered Passed Pawns
//    1517,  1506,  1426,  1421,  // Covered Passed Pawns
//    1000,  1050,  984,   968,   // Covered Passed Pawns
//    310,   129,   -238,  -63,   // Uncovered Passed Pawns
//    423,   115,   -43,   3,     // Uncovered Passed Pawns
//    1105,  310,   -307,  137,   // Uncovered Passed Pawns
//    1927,  551,   -218,  163,   // Uncovered Passed Pawns
//    3325,  880,   -10,   113,   // Uncovered Passed Pawns
//    3697,  1651,  389,   749,   // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    120,   328,   48,    25,    // Uncovered Passed Pawns
//    772,   442,   59,    367,   // Uncovered Passed Pawns
//    1597,  1017,  18,    7,     // Uncovered Passed Pawns
//    2324,  1773,  924,   1022,  // Uncovered Passed Pawns
//    2476,  2776,  2699,  2657,  // Uncovered Passed Pawns
//    288,   140,   -71,   -181,  // Uncovered Passed Pawns
//    -17,   115,   100,   104,   // Uncovered Passed Pawns
//    746,   546,   212,   380,   // Uncovered Passed Pawns
//    1313,  1093,  887,   985,   // Uncovered Passed Pawns
//    2356,  2264,  2106,  2153,  // Uncovered Passed Pawns
//    2533,  2472,  2461,  2462,  // Uncovered Passed Pawns
//    288,   140,   0,     0,     // Uncovered Passed Pawns
//    120,   328,   100,   104,   // Uncovered Passed Pawns
//    772,   546,   212,   380,   // Uncovered Passed Pawns
//    1597,  1093,  887,   985,   // Uncovered Passed Pawns
//    2356,  2264,  2106,  2153,  // Uncovered Passed Pawns
//    2533,  2776,  2699,  2657   // Uncovered Passed Pawns
//};

// 181229 SL v3
//const std::array<int, (271*4)> eval_weights = {
//    -534,  -733,  -199,  -83,   // King Piece Square Table
//    -330,  -461,  147,   69,    // King Piece Square Table
//    -65,   -58,   -365,  -350,  // King Piece Square Table
//    -93,   28,    -577,  -333,  // King Piece Square Table
//    -69,   -153,  478,   -536,  // King Piece Square Table
//    113,   143,   272,   -831,  // King Piece Square Table
//    121,   433,   -171,  -995,  // King Piece Square Table
//    219,   233,   294,   -554,  // King Piece Square Table
//    269,   266,   333,   -651,  // King Piece Square Table
//    464,   214,   -174,  44,    // King Piece Square Table
//    2445,  2332,  1856,  4896,  // Knight Piece Square Table
//    1888,  2512,  3442,  5137,  // Knight Piece Square Table
//    2151,  2578,  3249,  5105,  // Knight Piece Square Table
//    2096,  2644,  3555,  5187,  // Knight Piece Square Table
//    2272,  2708,  3317,  4768,  // Knight Piece Square Table
//    2138,  2736,  3389,  5315,  // Knight Piece Square Table
//    2373,  2707,  3425,  5388,  // Knight Piece Square Table
//    2365,  2762,  3490,  5398,  // Knight Piece Square Table
//    2354,  2847,  3604,  5367,  // Knight Piece Square Table
//    2323,  3000,  3481,  5374,  // Knight Piece Square Table
//    379,   213,   29,    142,   // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    148,   220,   159,   1497,  // Bishop Pair
//    131,   9,     0,     36,    // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     80,    77,    34,    // Abstract Piece Activity Bonus
//    60,    31,    38,    3,     // Abstract Piece Activity Bonus
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
//    0,     0,     0,     0,     // Double Pawn Penalty
//    -23,   -105,  40,    -123,  // Isolated Pawn
//    100,   100,   100,   100,   // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    2,     74,    -148,  -51,   // King Vector Exposure
//    16,    27,    -132,  -27,   // King Vector Exposure
//    66,    397,   8,     166,   // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    0,     9,     926,   571,   // Pawn Attack
//    0,     0,     566,   458,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     -71,   0,     // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    0,     0,     -93,   0,     // King Attack Distance
//    -221,  -73,   0,     0,     // King Attack Distance
//    0,     0,     12,    1,     // King Attack Danger
//    0,     0,     15,    14,    // King Attack Danger
//    0,     0,     50,    51,    // King Attack Danger
//    0,     63,    95,    0,     // King Attack Danger
//    -728,  -741,  -720,  -339,  // Safe Knight Square Count
//    -31,   -30,   -634,  -218,  // Safe Knight Square Count
//    -1,    -24,   -345,  -138,  // Safe Knight Square Count
//    0,     0,     -196,  -88,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    0,     0,     122,   43,    // Safe Knight Square Count
//    0,     0,     150,   95,    // Safe Knight Square Count
//    0,     0,     150,   95,    // Safe Knight Square Count
//    0,     0,     150,   95,    // Safe Knight Square Count
//    1332,  2028,  1695,  2919,  // Bishop Mobility
//    1708,  2035,  2029,  3103,  // Bishop Mobility
//    1887,  2120,  2316,  3216,  // Bishop Mobility
//    1927,  2232,  2517,  3314,  // Bishop Mobility
//    2067,  2354,  2561,  3393,  // Bishop Mobility
//    2067,  2462,  2713,  3452,  // Bishop Mobility
//    2069,  2574,  2859,  3506,  // Bishop Mobility
//    2112,  2685,  2867,  3568,  // Bishop Mobility
//    2154,  2716,  3013,  3575,  // Bishop Mobility
//    2159,  2718,  3136,  3601,  // Bishop Mobility
//    2159,  2718,  3136,  3749,  // Bishop Mobility
//    2160,  2719,  3291,  3853,  // Bishop Mobility
//    2400,  2721,  3291,  3866,  // Bishop Mobility
//    2402,  2723,  3298,  3873,  // Bishop Mobility
//    4329,  4391,  3336,  5846,  // Rook Mobility
//    4371,  4516,  3732,  5968,  // Rook Mobility
//    4629,  4815,  3766,  6004,  // Rook Mobility
//    4846,  4841,  3841,  6069,  // Rook Mobility
//    5046,  4923,  3849,  6080,  // Rook Mobility
//    5297,  4949,  3946,  6164,  // Rook Mobility
//    5377,  5029,  4056,  6210,  // Rook Mobility
//    5384,  5029,  4212,  6253,  // Rook Mobility
//    5401,  5035,  4456,  6323,  // Rook Mobility
//    5412,  5084,  4595,  6324,  // Rook Mobility
//    5424,  5123,  4700,  6472,  // Rook Mobility
//    5491,  5127,  4885,  6480,  // Rook Mobility
//    5565,  5150,  4957,  6620,  // Rook Mobility
//    5565,  5161,  5009,  6759,  // Rook Mobility
//    5565,  5237,  5011,  6759,  // Rook Mobility
//    8621,  9738,  8896,  10609, // Queen Mobility
//    8908,  10017, 8927,  10688, // Queen Mobility
//    8942,  10059, 8960,  10806, // Queen Mobility
//    8955,  10069, 8960,  10806, // Queen Mobility
//    9030,  10145, 8978,  10883, // Queen Mobility
//    9086,  10306, 9016,  10886, // Queen Mobility
//    9094,  10314, 9023,  10886, // Queen Mobility
//    9113,  10331, 9024,  10886, // Queen Mobility
//    9157,  10363, 9025,  10886, // Queen Mobility
//    9204,  10434, 9031,  10886, // Queen Mobility
//    9300,  10492, 9033,  10886, // Queen Mobility
//    9312,  10502, 9036,  10886, // Queen Mobility
//    9312,  10502, 9036,  10886, // Queen Mobility
//    9434,  10599, 9036,  10886, // Queen Mobility
//    9437,  10602, 9036,  10886, // Queen Mobility
//    9488,  10663, 9039,  10886, // Queen Mobility
//    9524,  10729, 9069,  10887, // Queen Mobility
//    9524,  10729, 9080,  10906, // Queen Mobility
//    9537,  10732, 9108,  11024, // Queen Mobility
//    9537,  10732, 9134,  11284, // Queen Mobility
//    9537,  10733, 9140,  11291, // Queen Mobility
//    9538,  10734, 9153,  11385, // Queen Mobility
//    9538,  10734, 9153,  11385, // Queen Mobility
//    9538,  10734, 9163,  11414, // Queen Mobility
//    9538,  10734, 9163,  11414, // Queen Mobility
//    9538,  10734, 9163,  11414, // Queen Mobility
//    9543,  10735, 9179,  11465, // Queen Mobility
//    9543,  10736, 9179,  11465, // Queen Mobility
//    -546,  318,   910,   53,    // Safe Checks
//    -282,  126,   342,   -9,    // Unsafe Checks
//    403,   -53,   -428,  -50,   // Unprotected Pieces
//    526,   550,   417,   414,   // Bishop's Position Relative To King
//    773,   603,   -32,   471,   // Bishop's Position Relative To King
//    434,   944,   405,   348,   // Bishop's Position Relative To King
//    285,   589,   607,   336,   // Bishop's Position Relative To King
//    923,   458,   277,   607,   // Bishop's Position Relative To King
//    841,   617,   184,   311,   // Bishop's Position Relative To King
//    960,   826,   97,    302,   // Bishop's Position Relative To King
//    527,   671,   285,   173,   // Bishop's Position Relative To King
//    566,   684,   681,   830,   // Bishop's Position Relative To King
//    869,   374,   414,   446,   // Bishop's Position Relative To King
//    763,   390,   552,   580,   // Bishop's Position Relative To King
//    726,   514,   341,   373,   // Bishop's Position Relative To King
//    892,   486,   405,   515,   // Bishop's Position Relative To King
//    381,   581,   419,   382,   // Bishop's Position Relative To King
//    684,   550,   548,   486,   // Bishop's Position Relative To King
//    682,   873,   356,   731,   // Bishop's Position Relative To King
//    577,   533,   518,   322,   // Bishop's Position Relative To King
//    571,   748,   429,   412,   // Bishop's Position Relative To King
//    657,   513,   345,   368,   // Bishop's Position Relative To King
//    773,   532,   524,   394,   // Bishop's Position Relative To King
//    643,   302,   700,   322,   // Bishop's Position Relative To King
//    772,   769,   716,   334,   // Bishop's Position Relative To King
//    341,   645,   509,   351,   // Bishop's Position Relative To King
//    694,   565,   521,   369,   // Bishop's Position Relative To King
//    629,   535,   253,   280,   // Bishop's Position Relative To King
//    732,   706,   421,   354,   // Bishop's Position Relative To King
//    556,   1167,  435,   383,   // Bishop's Position Relative To King
//    307,   653,   607,   200,   // Bishop's Position Relative To King
//    668,   444,   736,   203,   // Bishop's Position Relative To King
//    179,   575,   679,   267,   // Bishop's Position Relative To King
//    78,    1025,  1923,  789,   // Bishop's Position Relative To King
//    57,    492,   999,   409,   // Bishop's Position Relative To King
//    613,   594,   548,   353,   // Bishop's Position Relative To King
//    1212,  1140,  1539,  2220,  // Bishop's Position Relative To King
//    33,    525,   1015,  -437,  // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    262,   531,   634,   197,   // Opposed Pawns
//    307,   528,   494,   148,   // Opposed Pawns
//    248,   566,   558,   198,   // Opposed Pawns
//    307,   664,   505,   284,   // Opposed Pawns
//    128,   903,   900,   444,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    307,   578,   701,   318,   // Opposed Pawns
//    277,   594,   645,   344,   // Opposed Pawns
//    433,   659,   571,   563,   // Opposed Pawns
//    355,   1466,  1022,  270,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    298,   444,   706,   261,   // Opposed Pawns
//    240,   509,   556,   269,   // Opposed Pawns
//    301,   523,   630,   395,   // Opposed Pawns
//    455,   495,   770,   277,   // Opposed Pawns
//    1926,  2155,  2057,  2143,  // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    476,   578,   741,   338,   // Opposed Pawns
//    83,    716,   852,   506,   // Opposed Pawns
//    531,   760,   1248,  690,   // Opposed Pawns
//    1369,  1365,  1367,  1357,  // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    442,   438,   426,   523,   // Unopposed Pawns
//    403,   532,   232,   381,   // Unopposed Pawns
//    435,   518,   267,   808,   // Unopposed Pawns
//    487,   554,   692,   801,   // Unopposed Pawns
//    729,   1053,  885,   2038,  // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    488,   593,   586,   769,   // Unopposed Pawns
//    592,   533,   597,   1108,  // Unopposed Pawns
//    794,   710,   846,   1123,  // Unopposed Pawns
//    1980,  1628,  1543,  1628,  // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    559,   390,   543,   648,   // Unopposed Pawns
//    609,   531,   334,   697,   // Unopposed Pawns
//    872,   419,   578,   931,   // Unopposed Pawns
//    1031,  483,   1590,  1140,  // Unopposed Pawns
//    1815,  2069,  1956,  1995,  // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    718,   565,   749,   860,   // Unopposed Pawns
//    916,   1072,  579,   1024,  // Unopposed Pawns
//    1487,  1081,  1119,  1364,  // Unopposed Pawns
//    566,   557,   559,   562,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    737,   551,   445,   566,   // Covered Passed Pawns
//    772,   664,   17,    727,   // Covered Passed Pawns
//    921,   843,   58,    732,   // Covered Passed Pawns
//    849,   1042,  277,   1025,  // Covered Passed Pawns
//    642,   1162,  1487,  1022,  // Covered Passed Pawns
//    123,   583,   2269,  2771,  // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    609,   727,   891,   535,   // Covered Passed Pawns
//    910,   1092,  275,   933,   // Covered Passed Pawns
//    955,   1389,  485,   967,   // Covered Passed Pawns
//    902,   1882,  2053,  1688,  // Covered Passed Pawns
//    1626,  1920,  1699,  1636,  // Covered Passed Pawns
//    499,   572,   684,   628,   // Covered Passed Pawns
//    716,   671,   432,   627,   // Covered Passed Pawns
//    1000,  1291,  42,    615,   // Covered Passed Pawns
//    969,   1830,  598,   1092,  // Covered Passed Pawns
//    2382,  2403,  2147,  2138,  // Covered Passed Pawns
//    180,   179,   178,   179,   // Covered Passed Pawns
//    499,   572,   684,   628,   // Covered Passed Pawns
//    716,   727,   891,   627,   // Covered Passed Pawns
//    1000,  1291,  275,   933,   // Covered Passed Pawns
//    969,   1830,  598,   1092,  // Covered Passed Pawns
//    2382,  2403,  2147,  2138,  // Covered Passed Pawns
//    1626,  1920,  1699,  1636,  // Covered Passed Pawns
//    1099,  536,   -86,   364,   // Uncovered Passed Pawns
//    1189,  495,   174,   438,   // Uncovered Passed Pawns
//    1767,  803,   -231,  817,   // Uncovered Passed Pawns
//    2680,  989,   -48,   681,   // Uncovered Passed Pawns
//    4047,  1410,  248,   589,   // Uncovered Passed Pawns
//    4365,  2235,  528,   1235,  // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    517,   1046,  269,   325,   // Uncovered Passed Pawns
//    1345,  966,   212,   1059,  // Uncovered Passed Pawns
//    2274,  1616,  165,   256,   // Uncovered Passed Pawns
//    3126,  2559,  985,   943,   // Uncovered Passed Pawns
//    3711,  4163,  3851,  3775,  // Uncovered Passed Pawns
//    1145,  593,   201,   -41,   // Uncovered Passed Pawns
//    257,   609,   538,   574,   // Uncovered Passed Pawns
//    1378,  1194,  354,   752,   // Uncovered Passed Pawns
//    2119,  1679,  1255,  1557,  // Uncovered Passed Pawns
//    3893,  3997,  3561,  3669,  // Uncovered Passed Pawns
//    6101,  5963,  5906,  5908,  // Uncovered Passed Pawns
//    1145,  593,   201,   0,     // Uncovered Passed Pawns
//    517,   1046,  538,   574,   // Uncovered Passed Pawns
//    1378,  1194,  354,   1059,  // Uncovered Passed Pawns
//    2274,  1679,  1255,  1557,  // Uncovered Passed Pawns
//    3893,  3997,  3561,  3669,  // Uncovered Passed Pawns
//    6101,  5963,  5906,  5908   // Uncovered Passed Pawns
//};

// 190101, 0.5 mix, rook activity
//const std::array<int, (272*4)> eval_weights = {
//    -476,  -647,  -29,   111,   // King Piece Square Table
//    -289,  -398,  210,   298,   // King Piece Square Table
//    -45,   -41,   -296,  -68,   // King Piece Square Table
//    -66,   68,    -501,  -84,   // King Piece Square Table
//    -70,   -127,  427,   -73,   // King Piece Square Table
//    102,   167,   239,   -336,  // King Piece Square Table
//    112,   398,   -199,  -447,  // King Piece Square Table
//    176,   221,   183,   -87,   // King Piece Square Table
//    214,   301,   142,   -155,  // King Piece Square Table
//    314,   210,   10,    108,   // King Piece Square Table
//    2198,  2367,  1894,  3893,  // Knight Piece Square Table
//    2098,  2513,  2647,  4210,  // Knight Piece Square Table
//    2236,  2634,  2553,  4162,  // Knight Piece Square Table
//    2159,  2697,  2857,  4260,  // Knight Piece Square Table
//    2269,  2694,  2556,  3986,  // Knight Piece Square Table
//    2152,  2748,  2775,  4379,  // Knight Piece Square Table
//    2307,  2732,  2829,  4404,  // Knight Piece Square Table
//    2257,  2785,  2873,  4446,  // Knight Piece Square Table
//    2243,  2903,  2943,  4398,  // Knight Piece Square Table
//    2261,  3002,  2870,  4409,  // Knight Piece Square Table
//    975,   662,   223,   492,   // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    364,   210,   158,   1197,  // Bishop Pair
//    128,   7,     2,     25,    // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     68,    62,    38,    // Abstract Piece Activity Bonus
//    48,    29,    10,    7,     // Abstract Piece Activity Bonus
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
//    -25,   -2,    0,     -12,   // Double Pawn Penalty
//    -16,   -91,   26,    -129,  // Isolated Pawn
//    100,   100,   100,   100,   // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    21,    39,    -124,  -44,   // King Vector Exposure
//    -67,   48,    -110,  -18,   // King Vector Exposure
//    -3,    335,   49,    157,   // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    10,    71,    795,   527,   // Pawn Attack
//    1,     1,     583,   394,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    -3,    -3,    -65,   -8,    // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    0,     -1,    -52,   -1,    // King Attack Distance
//    -158,  -92,   -25,   -6,    // King Attack Distance
//    2,     5,     37,    15,    // King Attack Danger
//    2,     1,     12,    8,     // King Attack Danger
//    5,     3,     69,    44,    // King Attack Danger
//    1,     96,    43,    3,     // King Attack Danger
//    -563,  -561,  -501,  -289,  // Safe Knight Square Count
//    -43,   -85,   -443,  -194,  // Safe Knight Square Count
//    -18,   -68,   -228,  -118,  // Safe Knight Square Count
//    -7,    -13,   -148,  -80,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    8,     8,     101,   47,    // Safe Knight Square Count
//    9,     8,     171,   94,    // Safe Knight Square Count
//    9,     9,     174,   97,    // Safe Knight Square Count
//    12,    12,    184,   124,   // Safe Knight Square Count
//    1604,  1984,  1625,  2360,  // Bishop Mobility
//    1932,  2083,  1873,  2515,  // Bishop Mobility
//    2107,  2159,  2124,  2618,  // Bishop Mobility
//    2151,  2247,  2280,  2700,  // Bishop Mobility
//    2252,  2347,  2343,  2779,  // Bishop Mobility
//    2265,  2446,  2456,  2820,  // Bishop Mobility
//    2288,  2572,  2569,  2893,  // Bishop Mobility
//    2288,  2662,  2569,  2934,  // Bishop Mobility
//    2300,  2696,  2686,  2955,  // Bishop Mobility
//    2317,  2710,  2812,  2975,  // Bishop Mobility
//    2319,  2713,  2813,  3107,  // Bishop Mobility
//    2329,  2730,  2892,  3218,  // Bishop Mobility
//    2503,  2737,  2899,  3236,  // Bishop Mobility
//    2524,  2760,  2918,  3254,  // Bishop Mobility
//    4302,  4311,  2850,  4443,  // Rook Mobility
//    4425,  4488,  3061,  4524,  // Rook Mobility
//    4615,  4709,  3087,  4575,  // Rook Mobility
//    4748,  4764,  3131,  4609,  // Rook Mobility
//    4870,  4877,  3095,  4599,  // Rook Mobility
//    5047,  4864,  3190,  4663,  // Rook Mobility
//    5056,  5000,  3258,  4690,  // Rook Mobility
//    5138,  4957,  3351,  4721,  // Rook Mobility
//    5159,  5007,  3478,  4752,  // Rook Mobility
//    5218,  5079,  3545,  4696,  // Rook Mobility
//    5174,  5116,  3527,  4902,  // Rook Mobility
//    5313,  5149,  3572,  4819,  // Rook Mobility
//    5593,  5097,  3586,  4873,  // Rook Mobility
//    5606,  5044,  3605,  5092,  // Rook Mobility
//    5348,  5319,  3616,  5027,  // Rook Mobility
//    -481,  -73,   556,   541,   // Rook on Open File
//    8727,  8904,  8291,  7765,  // Queen Mobility
//    8872,  9057,  8331,  7833,  // Queen Mobility
//    8927,  9113,  8389,  7938,  // Queen Mobility
//    8934,  9118,  8389,  7951,  // Queen Mobility
//    9033,  9219,  8430,  7986,  // Queen Mobility
//    9063,  9302,  8432,  7993,  // Queen Mobility
//    9076,  9324,  8438,  7995,  // Queen Mobility
//    9117,  9364,  8452,  7996,  // Queen Mobility
//    9170,  9417,  8455,  7997,  // Queen Mobility
//    9191,  9446,  8455,  7997,  // Queen Mobility
//    9236,  9480,  8470,  7997,  // Queen Mobility
//    9266,  9496,  8473,  7998,  // Queen Mobility
//    9274,  9503,  8478,  8000,  // Queen Mobility
//    9368,  9594,  8478,  7999,  // Queen Mobility
//    9383,  9612,  8491,  8000,  // Queen Mobility
//    9418,  9639,  8491,  7999,  // Queen Mobility
//    9423,  9664,  8503,  8007,  // Queen Mobility
//    9424,  9666,  8507,  8008,  // Queen Mobility
//    9426,  9667,  8535,  8103,  // Queen Mobility
//    9432,  9674,  8590,  8222,  // Queen Mobility
//    9433,  9675,  8593,  8226,  // Queen Mobility
//    9433,  9674,  8602,  8273,  // Queen Mobility
//    9435,  9676,  8606,  8277,  // Queen Mobility
//    9435,  9677,  8608,  8299,  // Queen Mobility
//    9436,  9677,  8608,  8299,  // Queen Mobility
//    9437,  9679,  8613,  8305,  // Queen Mobility
//    9439,  9680,  8617,  8309,  // Queen Mobility
//    9446,  9687,  8625,  8323,  // Queen Mobility
//    -410,  317,   710,   64,    // Safe Checks
//    -179,  78,    267,   -13,   // Unsafe Checks
//    273,   -25,   -321,  -65,   // Unprotected Pieces
//    287,   287,   248,   255,   // Bishop's Position Relative To King
//    330,   307,   135,   198,   // Bishop's Position Relative To King
//    362,   504,   273,   132,   // Bishop's Position Relative To King
//    279,   346,   274,   156,   // Bishop's Position Relative To King
//    518,   316,   118,   323,   // Bishop's Position Relative To King
//    478,   409,   44,    136,   // Bishop's Position Relative To King
//    706,   600,   -32,   87,    // Bishop's Position Relative To King
//    460,   435,   160,   24,    // Bishop's Position Relative To King
//    452,   481,   457,   393,   // Bishop's Position Relative To King
//    499,   278,   259,   281,   // Bishop's Position Relative To King
//    458,   285,   326,   370,   // Bishop's Position Relative To King
//    491,   317,   106,   172,   // Bishop's Position Relative To King
//    571,   360,   233,   286,   // Bishop's Position Relative To King
//    345,   286,   219,   213,   // Bishop's Position Relative To King
//    427,   364,   364,   276,   // Bishop's Position Relative To King
//    545,   564,   363,   469,   // Bishop's Position Relative To King
//    400,   328,   264,   193,   // Bishop's Position Relative To King
//    429,   481,   265,   184,   // Bishop's Position Relative To King
//    513,   262,   174,   180,   // Bishop's Position Relative To King
//    514,   333,   399,   169,   // Bishop's Position Relative To King
//    459,   146,   459,   174,   // Bishop's Position Relative To King
//    602,   539,   419,   139,   // Bishop's Position Relative To King
//    261,   376,   274,   159,   // Bishop's Position Relative To King
//    499,   383,   373,   138,   // Bishop's Position Relative To King
//    425,   309,   135,   120,   // Bishop's Position Relative To King
//    419,   481,   294,   154,   // Bishop's Position Relative To King
//    514,   746,   298,   177,   // Bishop's Position Relative To King
//    192,   374,   356,   88,    // Bishop's Position Relative To King
//    407,   279,   414,   47,    // Bishop's Position Relative To King
//    137,   272,   369,   120,   // Bishop's Position Relative To King
//    354,   609,   1035,  503,   // Bishop's Position Relative To King
//    -33,   249,   637,   224,   // Bishop's Position Relative To King
//    301,   333,   350,   262,   // Bishop's Position Relative To King
//    859,   831,   942,   1049,  // Bishop's Position Relative To King
//    -25,   255,   447,   -179,  // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -116,  57,    113,   -31,   // Opposed Pawns
//    -49,   19,    19,    -76,   // Opposed Pawns
//    -116,  31,    59,    -10,   // Opposed Pawns
//    -47,   153,   7,     77,    // Opposed Pawns
//    -232,  361,   409,   254,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -11,   93,    158,   75,    // Opposed Pawns
//    -50,   85,    124,   114,   // Opposed Pawns
//    111,   142,   89,    316,   // Opposed Pawns
//    256,   728,   497,   231,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -47,   -54,   167,   32,    // Opposed Pawns
//    -85,   -4,    51,    29,    // Opposed Pawns
//    -20,   30,    103,   154,   // Opposed Pawns
//    82,    13,    301,   153,   // Opposed Pawns
//    592,   632,   610,   633,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    99,    142,   179,   96,    // Opposed Pawns
//    -138,  131,   281,   276,   // Opposed Pawns
//    222,   291,   513,   385,   // Opposed Pawns
//    429,   429,   426,   425,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -166,  -13,   106,   40,    // Unopposed Pawns
//    -163,  59,    -80,   -63,   // Unopposed Pawns
//    -169,  21,    8,     291,   // Unopposed Pawns
//    -92,   92,    362,   290,   // Unopposed Pawns
//    415,   507,   670,   912,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -107,  118,   234,   271,   // Unopposed Pawns
//    10,    77,    266,   570,   // Unopposed Pawns
//    238,   299,   463,   607,   // Unopposed Pawns
//    1071,  986,   933,   954,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -107,  -69,   204,   167,   // Unopposed Pawns
//    0,     41,    -35,   181,   // Unopposed Pawns
//    175,   27,    183,   441,   // Unopposed Pawns
//    464,   329,   715,   573,   // Unopposed Pawns
//    718,   789,   779,   789,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    69,    182,   314,   325,   // Unopposed Pawns
//    333,   447,   266,   507,   // Unopposed Pawns
//    780,   590,   601,   614,   // Unopposed Pawns
//    139,   138,   137,   136,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -30,   120,   214,   27,    // Covered Passed Pawns
//    5,     211,   -75,   155,   // Covered Passed Pawns
//    194,   366,   -82,   214,   // Covered Passed Pawns
//    158,   543,   164,   511,   // Covered Passed Pawns
//    -46,   736,   1155,  548,   // Covered Passed Pawns
//    -279,  389,   1351,  1482,  // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    -97,   219,   498,   231,   // Covered Passed Pawns
//    246,   570,   72,    364,   // Covered Passed Pawns
//    383,   814,   285,   403,   // Covered Passed Pawns
//    693,   1272,  1245,  1159,  // Covered Passed Pawns
//    912,   949,   899,   886,   // Covered Passed Pawns
//    -144,  82,    341,   167,   // Covered Passed Pawns
//    79,    162,   123,   172,   // Covered Passed Pawns
//    323,   619,   48,    192,   // Covered Passed Pawns
//    495,   942,   488,   599,   // Covered Passed Pawns
//    1391,  1367,  1300,  1297,  // Covered Passed Pawns
//    64,    64,    64,    64,    // Covered Passed Pawns
//    0,     82,    341,   167,   // Covered Passed Pawns
//    79,    219,   498,   231,   // Covered Passed Pawns
//    323,   619,   72,    364,   // Covered Passed Pawns
//    495,   942,   488,   599,   // Covered Passed Pawns
//    1391,  1367,  1300,  1297,  // Covered Passed Pawns
//    912,   949,   899,   886,   // Covered Passed Pawns
//    283,   145,   -274,  -156,  // Uncovered Passed Pawns
//    362,   81,    -68,   -20,   // Uncovered Passed Pawns
//    1046,  280,   -285,  139,   // Uncovered Passed Pawns
//    1828,  527,   -169,  189,   // Uncovered Passed Pawns
//    3232,  817,   8,     133,   // Uncovered Passed Pawns
//    3569,  1593,  436,   743,   // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    129,   317,   44,    0,     // Uncovered Passed Pawns
//    725,   405,   76,    345,   // Uncovered Passed Pawns
//    1523,  976,   54,    62,    // Uncovered Passed Pawns
//    2229,  1739,  956,   1044,  // Uncovered Passed Pawns
//    2374,  2641,  2571,  2534,  // Uncovered Passed Pawns
//    234,   96,    -105,  -182,  // Uncovered Passed Pawns
//    -28,   83,    59,    74,    // Uncovered Passed Pawns
//    712,   519,   249,   401,   // Uncovered Passed Pawns
//    1249,  1041,  869,   955,   // Uncovered Passed Pawns
//    2283,  2181,  2037,  2083,  // Uncovered Passed Pawns
//    2255,  2203,  2197,  2194,  // Uncovered Passed Pawns
//    234,   96,    0,     0,     // Uncovered Passed Pawns
//    129,   317,   59,    74,    // Uncovered Passed Pawns
//    725,   519,   249,   401,   // Uncovered Passed Pawns
//    1523,  1041,  869,   955,   // Uncovered Passed Pawns
//    2283,  2181,  2037,  2083,  // Uncovered Passed Pawns
//    2374,  2641,  2571,  2534   // Uncovered Passed Pawns
//};

// 190102, 0.5 mix, constraints
//const std::array<int, (272*4)> eval_weights = {
//    -450,  -663,  -38,   100,   // King Piece Square Table
//    -290,  -385,  214,   290,   // King Piece Square Table
//    -55,   -43,   -295,  -67,   // King Piece Square Table
//    -76,   59,    -504,  -78,   // King Piece Square Table
//    -60,   -128,  455,   -96,   // King Piece Square Table
//    96,    153,   225,   -346,  // King Piece Square Table
//    110,   395,   -202,  -448,  // King Piece Square Table
//    184,   235,   230,   -81,   // King Piece Square Table
//    217,   307,   137,   -186,  // King Piece Square Table
//    333,   240,   0,     110,   // King Piece Square Table
//    2238,  2407,  1872,  3922,  // Knight Piece Square Table
//    2095,  2526,  2689,  4220,  // Knight Piece Square Table
//    2234,  2640,  2555,  4174,  // Knight Piece Square Table
//    2138,  2703,  2847,  4264,  // Knight Piece Square Table
//    2307,  2740,  2594,  4022,  // Knight Piece Square Table
//    2148,  2769,  2779,  4385,  // Knight Piece Square Table
//    2299,  2767,  2811,  4436,  // Knight Piece Square Table
//    2274,  2823,  2866,  4456,  // Knight Piece Square Table
//    2270,  2924,  2942,  4413,  // Knight Piece Square Table
//    2265,  3020,  2831,  4428,  // Knight Piece Square Table
//    946,   649,   194,   476,   // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    342,   183,   148,   1222,  // Bishop Pair
//    141,   4,     0,     29,    // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     81,    62,    34,    // Abstract Piece Activity Bonus
//    54,    21,    19,    5,     // Abstract Piece Activity Bonus
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
//    -1,    0,     0,     0,     // Double Pawn Penalty
//    -23,   -74,   0,     -118,  // Isolated Pawn
//    100,   100,   100,   100,   // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    23,    44,    -127,  -46,   // King Vector Exposure
//    -46,   47,    -112,  -24,   // King Vector Exposure
//    -14,   351,   40,    154,   // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    3,     82,    783,   515,   // Pawn Attack
//    0,     3,     575,   397,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     -64,   0,     // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    0,     0,     -59,   0,     // King Attack Distance
//    -160,  -115,  -10,   0,     // King Attack Distance
//    0,     0,     16,    6,     // King Attack Danger
//    0,     0,     19,    9,     // King Attack Danger
//    0,     0,     67,    29,    // King Attack Danger
//    1,     117,   49,    1,     // King Attack Danger
//    -589,  -593,  -528,  -302,  // Safe Knight Square Count
//    -53,   -76,   -460,  -183,  // Safe Knight Square Count
//    -4,    -74,   -219,  -111,  // Safe Knight Square Count
//    0,     -4,    -131,  -74,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    2,     0,     121,   47,    // Safe Knight Square Count
//    2,     0,     176,   95,    // Safe Knight Square Count
//    2,     0,     176,   98,    // Safe Knight Square Count
//    2,     0,     177,   99,    // Safe Knight Square Count
//    1589,  2019,  1590,  2387,  // Bishop Mobility
//    1919,  2100,  1863,  2554,  // Bishop Mobility
//    2107,  2183,  2101,  2649,  // Bishop Mobility
//    2148,  2276,  2255,  2742,  // Bishop Mobility
//    2262,  2380,  2316,  2807,  // Bishop Mobility
//    2264,  2477,  2439,  2862,  // Bishop Mobility
//    2265,  2577,  2555,  2909,  // Bishop Mobility
//    2271,  2688,  2563,  2959,  // Bishop Mobility
//    2300,  2743,  2677,  2975,  // Bishop Mobility
//    2302,  2744,  2773,  2996,  // Bishop Mobility
//    2304,  2746,  2775,  3131,  // Bishop Mobility
//    2306,  2753,  2895,  3269,  // Bishop Mobility
//    2506,  2759,  2896,  3284,  // Bishop Mobility
//    2513,  2771,  2909,  3296,  // Bishop Mobility
//    4230,  4332,  2903,  4521,  // Rook Mobility
//    4385,  4542,  3100,  4627,  // Rook Mobility
//    4571,  4773,  3130,  4660,  // Rook Mobility
//    4740,  4846,  3160,  4705,  // Rook Mobility
//    4837,  4916,  3162,  4716,  // Rook Mobility
//    5024,  4934,  3255,  4779,  // Rook Mobility
//    5076,  4998,  3334,  4811,  // Rook Mobility
//    5078,  4999,  3430,  4829,  // Rook Mobility
//    5100,  5003,  3585,  4872,  // Rook Mobility
//    5130,  5041,  3644,  4875,  // Rook Mobility
//    5144,  5069,  3672,  5016,  // Rook Mobility
//    5196,  5069,  3743,  5029,  // Rook Mobility
//    5258,  5073,  3793,  5123,  // Rook Mobility
//    5258,  5075,  3807,  5165,  // Rook Mobility
//    5258,  5122,  3810,  5167,  // Rook Mobility
//    0,     0,     174,   432,   // Rook on Open File
//    8912,  9030,  8300,  7871,  // Queen Mobility
//    9099,  9225,  8379,  7946,  // Queen Mobility
//    9137,  9261,  8402,  8043,  // Queen Mobility
//    9141,  9264,  8405,  8049,  // Queen Mobility
//    9215,  9336,  8413,  8098,  // Queen Mobility
//    9260,  9448,  8438,  8107,  // Queen Mobility
//    9274,  9464,  8447,  8108,  // Queen Mobility
//    9305,  9500,  8452,  8108,  // Queen Mobility
//    9351,  9535,  8455,  8108,  // Queen Mobility
//    9392,  9597,  8459,  8108,  // Queen Mobility
//    9452,  9632,  8459,  8108,  // Queen Mobility
//    9478,  9638,  8460,  8108,  // Queen Mobility
//    9479,  9641,  8461,  8108,  // Queen Mobility
//    9571,  9724,  8461,  8108,  // Queen Mobility
//    9584,  9735,  8462,  8108,  // Queen Mobility
//    9603,  9766,  8465,  8109,  // Queen Mobility
//    9639,  9822,  8490,  8110,  // Queen Mobility
//    9640,  9822,  8507,  8125,  // Queen Mobility
//    9642,  9823,  8531,  8189,  // Queen Mobility
//    9643,  9823,  8579,  8342,  // Queen Mobility
//    9643,  9823,  8580,  8343,  // Queen Mobility
//    9643,  9823,  8590,  8407,  // Queen Mobility
//    9644,  9824,  8592,  8410,  // Queen Mobility
//    9645,  9825,  8593,  8416,  // Queen Mobility
//    9645,  9825,  8593,  8416,  // Queen Mobility
//    9646,  9826,  8594,  8418,  // Queen Mobility
//    9647,  9826,  8599,  8429,  // Queen Mobility
//    9647,  9826,  8599,  8429,  // Queen Mobility
//    -408,  316,   706,   60,    // Safe Checks
//    -190,  88,    269,   -2,    // Unsafe Checks
//    253,   -8,    -331,  -54,   // Unprotected Pieces
//    293,   293,   252,   258,   // Bishop's Position Relative To King
//    363,   337,   153,   237,   // Bishop's Position Relative To King
//    387,   548,   310,   153,   // Bishop's Position Relative To King
//    287,   371,   299,   177,   // Bishop's Position Relative To King
//    570,   347,   137,   321,   // Bishop's Position Relative To King
//    494,   406,   54,    153,   // Bishop's Position Relative To King
//    718,   605,   -53,   93,    // Bishop's Position Relative To King
//    439,   417,   109,   11,    // Bishop's Position Relative To King
//    479,   514,   491,   427,   // Bishop's Position Relative To King
//    508,   270,   258,   273,   // Bishop's Position Relative To King
//    491,   296,   345,   356,   // Bishop's Position Relative To King
//    510,   323,   133,   206,   // Bishop's Position Relative To King
//    593,   362,   234,   304,   // Bishop's Position Relative To King
//    390,   318,   200,   214,   // Bishop's Position Relative To King
//    448,   391,   388,   286,   // Bishop's Position Relative To King
//    549,   574,   342,   436,   // Bishop's Position Relative To King
//    419,   348,   268,   186,   // Bishop's Position Relative To King
//    447,   493,   276,   227,   // Bishop's Position Relative To King
//    525,   261,   189,   195,   // Bishop's Position Relative To King
//    542,   341,   382,   191,   // Bishop's Position Relative To King
//    458,   154,   451,   148,   // Bishop's Position Relative To King
//    641,   589,   442,   170,   // Bishop's Position Relative To King
//    268,   405,   306,   159,   // Bishop's Position Relative To King
//    485,   371,   353,   156,   // Bishop's Position Relative To King
//    438,   328,   120,   109,   // Bishop's Position Relative To King
//    449,   484,   295,   166,   // Bishop's Position Relative To King
//    521,   777,   282,   190,   // Bishop's Position Relative To King
//    219,   405,   370,   79,    // Bishop's Position Relative To King
//    447,   295,   449,   62,    // Bishop's Position Relative To King
//    155,   309,   387,   134,   // Bishop's Position Relative To King
//    389,   678,   1126,  539,   // Bishop's Position Relative To King
//    -23,   278,   671,   243,   // Bishop's Position Relative To King
//    330,   362,   381,   285,   // Bishop's Position Relative To King
//    874,   853,   962,   1082,  // Bishop's Position Relative To King
//    -10,   259,   483,   -195,  // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -90,   61,    151,   -16,   // Opposed Pawns
//    -32,   29,    40,    -71,   // Opposed Pawns
//    -91,   62,    87,    3,     // Opposed Pawns
//    -43,   184,   24,    88,    // Opposed Pawns
//    -218,  398,   451,   260,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    3,     109,   190,   93,    // Opposed Pawns
//    -45,   116,   143,   132,   // Opposed Pawns
//    92,    188,   93,    324,   // Opposed Pawns
//    221,   769,   500,   210,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -46,   -36,   213,   44,    // Opposed Pawns
//    -75,   12,    83,    44,    // Opposed Pawns
//    -27,   43,    133,   171,   // Opposed Pawns
//    82,    50,    328,   178,   // Opposed Pawns
//    638,   687,   659,   683,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    101,   167,   221,   114,   // Opposed Pawns
//    -112,  178,   311,   292,   // Opposed Pawns
//    252,   319,   548,   394,   // Opposed Pawns
//    488,   488,   485,   484,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -139,  16,    116,   89,    // Unopposed Pawns
//    -153,  74,    -34,   -37,   // Unopposed Pawns
//    -121,  65,    27,    341,   // Unopposed Pawns
//    -67,   134,   380,   332,   // Unopposed Pawns
//    421,   538,   695,   899,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -88,   179,   255,   305,   // Unopposed Pawns
//    29,    133,   272,   605,   // Unopposed Pawns
//    251,   333,   486,   629,   // Unopposed Pawns
//    1082,  988,   941,   962,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -64,   -18,   217,   198,   // Unopposed Pawns
//    31,    88,    25,    229,   // Unopposed Pawns
//    215,   70,    215,   458,   // Unopposed Pawns
//    467,   323,   722,   575,   // Unopposed Pawns
//    771,   858,   845,   854,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    90,    231,   341,   364,   // Unopposed Pawns
//    367,   502,   298,   563,   // Unopposed Pawns
//    826,   618,   628,   650,   // Unopposed Pawns
//    158,   158,   157,   158,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    27,    139,   227,   64,    // Covered Passed Pawns
//    49,    226,   -78,   163,   // Covered Passed Pawns
//    237,   374,   -70,   233,   // Covered Passed Pawns
//    203,   563,   170,   514,   // Covered Passed Pawns
//    -4,    757,   1197,  559,   // Covered Passed Pawns
//    -274,  385,   1419,  1557,  // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    -63,   280,   533,   234,   // Covered Passed Pawns
//    296,   628,   57,    397,   // Covered Passed Pawns
//    404,   890,   309,   460,   // Covered Passed Pawns
//    671,   1334,  1301,  1205,  // Covered Passed Pawns
//    992,   1047,  986,   970,   // Covered Passed Pawns
//    -104,  121,   383,   190,   // Covered Passed Pawns
//    108,   212,   131,   193,   // Covered Passed Pawns
//    353,   671,   18,    190,   // Covered Passed Pawns
//    535,   1020,  513,   635,   // Covered Passed Pawns
//    1512,  1503,  1426,  1421,  // Covered Passed Pawns
//    82,    82,    82,    82,    // Covered Passed Pawns
//    0,     121,   383,   190,   // Covered Passed Pawns
//    108,   280,   533,   234,   // Covered Passed Pawns
//    353,   671,   57,    397,   // Covered Passed Pawns
//    535,   1020,  513,   635,   // Covered Passed Pawns
//    1512,  1503,  1426,  1421,  // Covered Passed Pawns
//    992,   1047,  986,   970,   // Covered Passed Pawns
//    335,   133,   -270,  -131,  // Uncovered Passed Pawns
//    448,   111,   -58,   1,     // Uncovered Passed Pawns
//    1113,  291,   -308,  143,   // Uncovered Passed Pawns
//    1930,  527,   -202,  185,   // Uncovered Passed Pawns
//    3323,  850,   14,    124,   // Uncovered Passed Pawns
//    3700,  1624,  423,   777,   // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    129,   332,   32,    0,     // Uncovered Passed Pawns
//    763,   439,   54,    361,   // Uncovered Passed Pawns
//    1583,  1027,  21,    14,    // Uncovered Passed Pawns
//    2300,  1778,  937,   1027,  // Uncovered Passed Pawns
//    2468,  2764,  2691,  2651,  // Uncovered Passed Pawns
//    303,   142,   -89,   -181,  // Uncovered Passed Pawns
//    -11,   122,   88,    95,    // Uncovered Passed Pawns
//    738,   535,   223,   391,   // Uncovered Passed Pawns
//    1311,  1082,  889,   990,   // Uncovered Passed Pawns
//    2349,  2250,  2094,  2144,  // Uncovered Passed Pawns
//    2508,  2447,  2440,  2439,  // Uncovered Passed Pawns
//    303,   142,   0,     0,     // Uncovered Passed Pawns
//    129,   332,   88,    95,    // Uncovered Passed Pawns
//    763,   535,   223,   391,   // Uncovered Passed Pawns
//    1583,  1082,  889,   990,   // Uncovered Passed Pawns
//    2349,  2250,  2094,  2144,  // Uncovered Passed Pawns
//    2508,  2764,  2691,  2651   // Uncovered Passed Pawns
//};

// Winter Dset depth 4
//const std::array<int, (272*4)> eval_weights = {
//    -583,  -676,  -72,   126,   // King Piece Square Table
//    -340,  -330,  154,   286,   // King Piece Square Table
//    -36,   -1,    -348,  -103,  // King Piece Square Table
//    -81,   103,   -571,  -74,   // King Piece Square Table
//    -25,   -105,  463,   -166,  // King Piece Square Table
//    143,   157,   245,   -324,  // King Piece Square Table
//    138,   401,   -167,  -453,  // King Piece Square Table
//    235,   209,   329,   -82,   // King Piece Square Table
//    256,   281,   237,   -174,  // King Piece Square Table
//    378,   177,   172,   148,   // King Piece Square Table
//    2408,  2613,  2147,  3805,  // Knight Piece Square Table
//    2243,  2731,  3071,  4161,  // Knight Piece Square Table
//    2373,  2847,  2951,  4125,  // Knight Piece Square Table
//    2279,  2906,  3255,  4211,  // Knight Piece Square Table
//    2474,  2929,  2898,  4013,  // Knight Piece Square Table
//    2249,  3001,  3183,  4328,  // Knight Piece Square Table
//    2404,  3022,  3187,  4384,  // Knight Piece Square Table
//    2425,  3052,  3239,  4400,  // Knight Piece Square Table
//    2416,  3148,  3302,  4361,  // Knight Piece Square Table
//    2358,  3277,  3214,  4370,  // Knight Piece Square Table
//    1024,  699,   258,   466,   // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    313,   211,   186,   1166,  // Bishop Pair
//    152,   5,     0,     28,    // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     80,    62,    34,    // Abstract Piece Activity Bonus
//    59,    17,    21,    4,     // Abstract Piece Activity Bonus
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
//    -1,    0,     0,     0,     // Double Pawn Penalty
//    -30,   -85,   0,     -115,  // Isolated Pawn
//    100,   100,   100,   100,   // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    15,    64,    -143,  -45,   // King Vector Exposure
//    -23,   49,    -135,  -23,   // King Vector Exposure
//    -27,   357,   57,    154,   // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    0,     75,    847,   519,   // Pawn Attack
//    4,     20,    587,   404,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     -69,   0,     // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    0,     0,     -70,   0,     // King Attack Distance
//    0,     -273,  0,     0,     // King Attack Distance
//    0,     0,     6,     5,     // King Attack Danger
//    0,     0,     6,     8,     // King Attack Danger
//    0,     0,     81,    30,    // King Attack Danger
//    16,    329,   0,     0,     // King Attack Danger
//    -542,  -591,  -538,  -326,  // Safe Knight Square Count
//    -37,   -91,   -487,  -191,  // Safe Knight Square Count
//    -5,    -75,   -227,  -118,  // Safe Knight Square Count
//    0,     -8,    -129,  -77,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    1,     0,     144,   41,    // Safe Knight Square Count
//    1,     0,     203,   84,    // Safe Knight Square Count
//    1,     0,     203,   88,    // Safe Knight Square Count
//    1,     0,     203,   88,    // Safe Knight Square Count
//    1713,  2344,  1855,  2369,  // Bishop Mobility
//    1980,  2485,  2162,  2535,  // Bishop Mobility
//    2238,  2519,  2392,  2631,  // Bishop Mobility
//    2273,  2621,  2568,  2722,  // Bishop Mobility
//    2375,  2738,  2624,  2791,  // Bishop Mobility
//    2375,  2841,  2739,  2845,  // Bishop Mobility
//    2378,  2932,  2868,  2888,  // Bishop Mobility
//    2385,  3051,  2879,  2937,  // Bishop Mobility
//    2413,  3088,  3010,  2946,  // Bishop Mobility
//    2419,  3101,  3117,  2977,  // Bishop Mobility
//    2433,  3101,  3117,  3131,  // Bishop Mobility
//    2434,  3105,  3211,  3300,  // Bishop Mobility
//    2605,  3106,  3215,  3316,  // Bishop Mobility
//    2622,  3119,  3217,  3320,  // Bishop Mobility
//    4745,  4728,  3349,  4444,  // Rook Mobility
//    4945,  4922,  3550,  4544,  // Rook Mobility
//    5046,  5192,  3600,  4580,  // Rook Mobility
//    5218,  5264,  3632,  4624,  // Rook Mobility
//    5328,  5328,  3632,  4634,  // Rook Mobility
//    5543,  5334,  3725,  4696,  // Rook Mobility
//    5585,  5410,  3801,  4734,  // Rook Mobility
//    5587,  5410,  3893,  4757,  // Rook Mobility
//    5607,  5420,  4058,  4796,  // Rook Mobility
//    5631,  5451,  4131,  4796,  // Rook Mobility
//    5643,  5479,  4187,  4909,  // Rook Mobility
//    5685,  5483,  4285,  4916,  // Rook Mobility
//    5732,  5485,  4368,  4963,  // Rook Mobility
//    5732,  5485,  4404,  5082,  // Rook Mobility
//    5732,  5521,  4413,  5085,  // Rook Mobility
//    0,     0,     230,   363,   // Rook on Open File
//    10547, 9865,  9227,  8682,  // Queen Mobility
//    10742, 10067, 9310,  8767,  // Queen Mobility
//    10776, 10100, 9330,  8857,  // Queen Mobility
//    10778, 10102, 9331,  8862,  // Queen Mobility
//    10827, 10174, 9368,  8913,  // Queen Mobility
//    10887, 10260, 9382,  8929,  // Queen Mobility
//    10892, 10265, 9386,  8929,  // Queen Mobility
//    10937, 10304, 9390,  8929,  // Queen Mobility
//    10984, 10338, 9390,  8929,  // Queen Mobility
//    11032, 10386, 9394,  8929,  // Queen Mobility
//    11073, 10421, 9394,  8929,  // Queen Mobility
//    11106, 10448, 9394,  8929,  // Queen Mobility
//    11106, 10448, 9394,  8929,  // Queen Mobility
//    11194, 10490, 9396,  8929,  // Queen Mobility
//    11224, 10535, 9396,  8929,  // Queen Mobility
//    11226, 10537, 9398,  8929,  // Queen Mobility
//    11238, 10561, 9485,  8932,  // Queen Mobility
//    11238, 10561, 9485,  8966,  // Queen Mobility
//    11238, 10561, 9510,  9021,  // Queen Mobility
//    11238, 10561, 9513,  9208,  // Queen Mobility
//    11238, 10561, 9513,  9220,  // Queen Mobility
//    11238, 10561, 9514,  9221,  // Queen Mobility
//    11238, 10561, 9514,  9221,  // Queen Mobility
//    11238, 10561, 9514,  9221,  // Queen Mobility
//    11238, 10561, 9514,  9221,  // Queen Mobility
//    11238, 10561, 9514,  9221,  // Queen Mobility
//    11238, 10561, 9514,  9221,  // Queen Mobility
//    11238, 10561, 9514,  9221,  // Queen Mobility
//    -353,  284,   806,   59,    // Safe Checks
//    -98,   61,    247,   -5,    // Unsafe Checks
//    226,   1,     -339,  -55,   // Unprotected Pieces
//    264,   264,   237,   228,   // Bishop's Position Relative To King
//    322,   329,   95,    266,   // Bishop's Position Relative To King
//    341,   495,   275,   175,   // Bishop's Position Relative To King
//    148,   363,   336,   169,   // Bishop's Position Relative To King
//    515,   309,   211,   296,   // Bishop's Position Relative To King
//    527,   385,   36,    145,   // Bishop's Position Relative To King
//    711,   616,   -94,   105,   // Bishop's Position Relative To King
//    403,   424,   131,   -7,    // Bishop's Position Relative To King
//    430,   471,   507,   492,   // Bishop's Position Relative To King
//    463,   266,   265,   270,   // Bishop's Position Relative To King
//    421,   292,   386,   335,   // Bishop's Position Relative To King
//    572,   257,   135,   203,   // Bishop's Position Relative To King
//    560,   333,   242,   320,   // Bishop's Position Relative To King
//    391,   292,   194,   217,   // Bishop's Position Relative To King
//    377,   390,   385,   305,   // Bishop's Position Relative To King
//    474,   505,   416,   444,   // Bishop's Position Relative To King
//    452,   342,   255,   174,   // Bishop's Position Relative To King
//    376,   476,   306,   250,   // Bishop's Position Relative To King
//    498,   233,   199,   179,   // Bishop's Position Relative To King
//    474,   341,   411,   208,   // Bishop's Position Relative To King
//    478,   138,   447,   152,   // Bishop's Position Relative To King
//    580,   601,   442,   179,   // Bishop's Position Relative To King
//    269,   395,   323,   137,   // Bishop's Position Relative To King
//    403,   406,   338,   175,   // Bishop's Position Relative To King
//    476,   296,   110,   99,    // Bishop's Position Relative To King
//    390,   449,   359,   170,   // Bishop's Position Relative To King
//    555,   755,   326,   205,   // Bishop's Position Relative To King
//    229,   393,   349,   85,    // Bishop's Position Relative To King
//    334,   330,   475,   51,    // Bishop's Position Relative To King
//    164,   326,   366,   115,   // Bishop's Position Relative To King
//    394,   705,   1074,  571,   // Bishop's Position Relative To King
//    -4,    271,   689,   201,   // Bishop's Position Relative To King
//    186,   427,   385,   323,   // Bishop's Position Relative To King
//    887,   929,   943,   1055,  // Bishop's Position Relative To King
//    -22,   267,   464,   -145,  // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -124,  38,    142,   -20,   // Opposed Pawns
//    -69,   19,    8,     -72,   // Opposed Pawns
//    -133,  52,    69,    -2,    // Opposed Pawns
//    -77,   181,   -6,    88,    // Opposed Pawns
//    -235,  388,   514,   206,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -40,   100,   174,   92,    // Opposed Pawns
//    -73,   98,    143,   125,   // Opposed Pawns
//    89,    157,   100,   309,   // Opposed Pawns
//    202,   775,   469,   228,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -85,   -58,   204,   41,    // Opposed Pawns
//    -99,   -14,   67,    44,    // Opposed Pawns
//    -47,   29,    112,   168,   // Opposed Pawns
//    113,   23,    256,   201,   // Opposed Pawns
//    617,   673,   639,   657,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    65,    179,   187,   109,   // Opposed Pawns
//    -99,   149,   301,   281,   // Opposed Pawns
//    292,   347,   473,   402,   // Opposed Pawns
//    432,   421,   420,   418,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -185,  -13,   117,   95,    // Unopposed Pawns
//    -202,  55,    -56,   -20,   // Unopposed Pawns
//    -181,  56,    -1,    356,   // Unopposed Pawns
//    -106,  108,   373,   332,   // Unopposed Pawns
//    389,   476,   790,   911,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -155,  186,   234,   319,   // Unopposed Pawns
//    -5,    105,   287,   618,   // Unopposed Pawns
//    191,   307,   535,   650,   // Unopposed Pawns
//    1030,  1020,  997,   994,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -110,  -37,   218,   214,   // Unopposed Pawns
//    -27,   79,    11,    237,   // Unopposed Pawns
//    172,   24,    241,   474,   // Unopposed Pawns
//    459,   281,   743,   612,   // Unopposed Pawns
//    849,   927,   895,   904,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    36,    230,   327,   399,   // Unopposed Pawns
//    359,   468,   333,   545,   // Unopposed Pawns
//    820,   631,   624,   647,   // Unopposed Pawns
//    145,   147,   146,   147,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -4,    106,   234,   109,   // Covered Passed Pawns
//    22,    226,   -82,   127,   // Covered Passed Pawns
//    208,   376,   -86,   239,   // Covered Passed Pawns
//    172,   588,   168,   514,   // Covered Passed Pawns
//    -24,   760,   1290,  553,   // Covered Passed Pawns
//    -270,  461,   1487,  1595,  // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    -79,   292,   515,   218,   // Covered Passed Pawns
//    293,   611,   44,    405,   // Covered Passed Pawns
//    395,   929,   303,   460,   // Covered Passed Pawns
//    653,   1460,  1360,  1281,  // Covered Passed Pawns
//    1120,  1179,  1108,  1095,  // Covered Passed Pawns
//    -189,  98,    417,   209,   // Covered Passed Pawns
//    108,   240,   98,    107,   // Covered Passed Pawns
//    336,   711,   11,    178,   // Covered Passed Pawns
//    528,   986,   651,   754,   // Covered Passed Pawns
//    1486,  1610,  1574,  1555,  // Covered Passed Pawns
//    146,   146,   145,   145,   // Covered Passed Pawns
//    0,     98,    417,   209,   // Covered Passed Pawns
//    108,   292,   515,   218,   // Covered Passed Pawns
//    336,   711,   44,    405,   // Covered Passed Pawns
//    528,   986,   651,   754,   // Covered Passed Pawns
//    1486,  1610,  1574,  1555,  // Covered Passed Pawns
//    1120,  1179,  1108,  1095,  // Covered Passed Pawns
//    310,   96,    -226,  -192,  // Uncovered Passed Pawns
//    441,   67,    -59,   25,    // Uncovered Passed Pawns
//    1128,  283,   -363,  161,   // Uncovered Passed Pawns
//    2014,  525,   -223,  122,   // Uncovered Passed Pawns
//    3529,  834,   31,    56,    // Uncovered Passed Pawns
//    4004,  1607,  440,   734,   // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    155,   341,   -17,   -26,   // Uncovered Passed Pawns
//    759,   460,   15,    387,   // Uncovered Passed Pawns
//    1677,  1074,  -13,   -92,   // Uncovered Passed Pawns
//    2352,  1907,  981,   986,   // Uncovered Passed Pawns
//    2735,  2905,  2816,  2816,  // Uncovered Passed Pawns
//    251,   161,   -138,  -281,  // Uncovered Passed Pawns
//    21,    108,   24,    37,    // Uncovered Passed Pawns
//    769,   540,   188,   354,   // Uncovered Passed Pawns
//    1338,  1184,  902,   1020,  // Uncovered Passed Pawns
//    2384,  2326,  2216,  2255,  // Uncovered Passed Pawns
//    2648,  2589,  2585,  2587,  // Uncovered Passed Pawns
//    251,   161,   0,     0,     // Uncovered Passed Pawns
//    155,   341,   24,    37,    // Uncovered Passed Pawns
//    769,   540,   188,   387,   // Uncovered Passed Pawns
//    1677,  1184,  902,   1020,  // Uncovered Passed Pawns
//    2384,  2326,  2216,  2255,  // Uncovered Passed Pawns
//    2735,  2905,  2816,  2816   // Uncovered Passed Pawns
//};

// Winter Dset d4, reduced base learn rate
const std::array<int, (272*4)> eval_weights = {
    -607,  -678,  -51,   120,   // King Piece Square Table
    -349,  -361,  183,   289,   // King Piece Square Table
    -36,   -34,   -317,  -98,   // King Piece Square Table
    -80,   69,    -537,  -72,   // King Piece Square Table
    -44,   -100,  380,   -44,   // King Piece Square Table
    137,   164,   196,   -295,  // King Piece Square Table
    132,   398,   -178,  -448,  // King Piece Square Table
    211,   227,   244,   36,    // King Piece Square Table
    238,   286,   172,   -63,   // King Piece Square Table
    348,   172,   196,   200,   // King Piece Square Table
    2268,  2494,  2207,  3898,  // Knight Piece Square Table
    2295,  2674,  2849,  4212,  // Knight Piece Square Table
    2405,  2777,  2770,  4175,  // Knight Piece Square Table
    2342,  2838,  3036,  4269,  // Knight Piece Square Table
    2489,  2854,  2739,  4086,  // Knight Piece Square Table
    2277,  2893,  3060,  4377,  // Knight Piece Square Table
    2420,  2935,  3044,  4434,  // Knight Piece Square Table
    2439,  2944,  3126,  4447,  // Knight Piece Square Table
    2423,  3039,  3202,  4412,  // Knight Piece Square Table
    2376,  3153,  3130,  4418,  // Knight Piece Square Table
    1171,  803,   318,   577,   // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    0,     0,     0,     0,     // Piece Base Value
    407,   250,   141,   1132,  // Bishop Pair
    153,   4,     0,     27,    // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    0,     0,     0,     0,     // Abstract Piece Activity Bonus
    0,     86,    59,    34,    // Abstract Piece Activity Bonus
    74,    20,    12,    4,     // Abstract Piece Activity Bonus
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
    -60,   0,     0,     0,     // Double Pawn Penalty
    -11,   -92,   0,     -119,  // Isolated Pawn
    100,   100,   100,   100,   // Tempo Bonus
    0,     0,     0,     0,     // Direct Pawn Shield Bonus
    22,    50,    -138,  -45,   // King Vector Exposure
    -49,   57,    -129,  -23,   // King Vector Exposure
    -19,   341,   78,    148,   // Knight Outpost
    0,     0,     0,     0,     // Castling Right
    0,     115,   788,   526,   // Pawn Attack
    1,     13,    606,   398,   // Minor Attack
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     0,     0,     // King Defence Distance
    0,     0,     -49,   0,     // King Attack Distance
    0,     0,     0,     0,     // King Attack Distance
    0,     0,     -67,   0,     // King Attack Distance
    0,     -179,  -40,   0,     // King Attack Distance
    0,     0,     14,    5,     // King Attack Danger
    0,     0,     9,     6,     // King Attack Danger
    0,     0,     82,    28,    // King Attack Danger
    45,    309,   0,     0,     // King Attack Danger
    -540,  -569,  -509,  -330,  // Safe Knight Square Count
    -105,  -116,  -406,  -196,  // Safe Knight Square Count
    -8,    -94,   -186,  -122,  // Safe Knight Square Count
    -1,    -14,   -106,  -81,   // Safe Knight Square Count
    0,     0,     0,     0,     // Safe Knight Square Count
    0,     24,    131,   40,    // Safe Knight Square Count
    0,     24,    181,   86,    // Safe Knight Square Count
    0,     24,    181,   92,    // Safe Knight Square Count
    0,     24,    181,   92,    // Safe Knight Square Count
    1781,  2118,  1953,  2457,  // Bishop Mobility
    2054,  2317,  2223,  2627,  // Bishop Mobility
    2288,  2374,  2447,  2720,  // Bishop Mobility
    2331,  2467,  2626,  2812,  // Bishop Mobility
    2469,  2567,  2676,  2881,  // Bishop Mobility
    2469,  2671,  2793,  2934,  // Bishop Mobility
    2470,  2769,  2916,  2977,  // Bishop Mobility
    2477,  2890,  2926,  3025,  // Bishop Mobility
    2501,  2938,  3043,  3034,  // Bishop Mobility
    2511,  2955,  3137,  3068,  // Bishop Mobility
    2522,  2961,  3137,  3217,  // Bishop Mobility
    2541,  2971,  3188,  3386,  // Bishop Mobility
    2729,  2972,  3188,  3391,  // Bishop Mobility
    2738,  2984,  3189,  3391,  // Bishop Mobility
    4274,  4633,  3508,  4526,  // Rook Mobility
    4474,  4826,  3699,  4627,  // Rook Mobility
    4637,  5082,  3736,  4662,  // Rook Mobility
    4789,  5191,  3750,  4704,  // Rook Mobility
    4872,  5274,  3750,  4714,  // Rook Mobility
    5073,  5305,  3828,  4779,  // Rook Mobility
    5106,  5396,  3899,  4812,  // Rook Mobility
    5109,  5397,  3989,  4836,  // Rook Mobility
    5156,  5403,  4143,  4877,  // Rook Mobility
    5169,  5451,  4203,  4878,  // Rook Mobility
    5176,  5488,  4229,  5034,  // Rook Mobility
    5215,  5496,  4320,  5050,  // Rook Mobility
    5281,  5498,  4372,  5128,  // Rook Mobility
    5281,  5498,  4419,  5239,  // Rook Mobility
    5281,  5546,  4422,  5241,  // Rook Mobility
    0,     0,     216,   386,   // Rook on Open File
    9573,  9445,  9386,  9132,  // Queen Mobility
    9751,  9624,  9511,  9215,  // Queen Mobility
    9788,  9668,  9532,  9304,  // Queen Mobility
    9796,  9669,  9533,  9308,  // Queen Mobility
    9851,  9749,  9567,  9357,  // Queen Mobility
    9898,  9820,  9599,  9375,  // Queen Mobility
    9909,  9832,  9601,  9375,  // Queen Mobility
    9946,  9882,  9604,  9375,  // Queen Mobility
    9992,  9920,  9604,  9375,  // Queen Mobility
    10041, 9971,  9606,  9375,  // Queen Mobility
    10084, 10013, 9606,  9375,  // Queen Mobility
    10118, 10037, 9606,  9375,  // Queen Mobility
    10118, 10041, 9606,  9375,  // Queen Mobility
    10202, 10089, 9606,  9375,  // Queen Mobility
    10235, 10138, 9607,  9375,  // Queen Mobility
    10237, 10141, 9608,  9375,  // Queen Mobility
    10243, 10157, 9709,  9378,  // Queen Mobility
    10243, 10157, 9709,  9426,  // Queen Mobility
    10243, 10157, 9737,  9493,  // Queen Mobility
    10243, 10157, 9768,  9627,  // Queen Mobility
    10243, 10157, 9773,  9639,  // Queen Mobility
    10243, 10157, 9774,  9639,  // Queen Mobility
    10243, 10157, 9774,  9639,  // Queen Mobility
    10243, 10157, 9774,  9639,  // Queen Mobility
    10243, 10157, 9774,  9639,  // Queen Mobility
    10243, 10157, 9774,  9639,  // Queen Mobility
    10243, 10157, 9774,  9639,  // Queen Mobility
    10243, 10157, 9774,  9639,  // Queen Mobility
    -344,  284,   799,   60,    // Safe Checks
    -69,   56,    242,   -5,    // Unsafe Checks
    195,   23,    -336,  -54,   // Unprotected Pieces
    152,   153,   135,   132,   // Bishop's Position Relative To King
    267,   269,   130,   224,   // Bishop's Position Relative To King
    345,   425,   260,   132,   // Bishop's Position Relative To King
    205,   307,   284,   145,   // Bishop's Position Relative To King
    418,   299,   204,   258,   // Bishop's Position Relative To King
    420,   339,   53,    120,   // Bishop's Position Relative To King
    608,   554,   -63,   69,    // Bishop's Position Relative To King
    357,   351,   149,   -35,   // Bishop's Position Relative To King
    450,   463,   438,   419,   // Bishop's Position Relative To King
    388,   260,   233,   246,   // Bishop's Position Relative To King
    342,   266,   365,   316,   // Bishop's Position Relative To King
    477,   247,   112,   182,   // Bishop's Position Relative To King
    460,   319,   233,   287,   // Bishop's Position Relative To King
    347,   253,   175,   194,   // Bishop's Position Relative To King
    345,   345,   370,   271,   // Bishop's Position Relative To King
    442,   452,   396,   419,   // Bishop's Position Relative To King
    406,   294,   227,   159,   // Bishop's Position Relative To King
    347,   415,   288,   220,   // Bishop's Position Relative To King
    436,   198,   172,   161,   // Bishop's Position Relative To King
    393,   307,   404,   174,   // Bishop's Position Relative To King
    391,   117,   427,   131,   // Bishop's Position Relative To King
    553,   549,   409,   151,   // Bishop's Position Relative To King
    257,   329,   275,   139,   // Bishop's Position Relative To King
    328,   356,   335,   145,   // Bishop's Position Relative To King
    433,   240,   101,   76,    // Bishop's Position Relative To King
    327,   406,   349,   141,   // Bishop's Position Relative To King
    532,   653,   343,   174,   // Bishop's Position Relative To King
    205,   326,   328,   68,    // Bishop's Position Relative To King
    252,   280,   448,   53,    // Bishop's Position Relative To King
    143,   254,   335,   110,   // Bishop's Position Relative To King
    476,   636,   880,   604,   // Bishop's Position Relative To King
    -32,   224,   589,   244,   // Bishop's Position Relative To King
    153,   329,   353,   328,   // Bishop's Position Relative To King
    841,   860,   850,   920,   // Bishop's Position Relative To King
    -12,   191,   316,   -17,   // Bishop's Position Relative To King
    0,     0,     0,     0,     // Bishop's Position Relative To King
    -140,  -65,   2,     -78,   // Opposed Pawns
    -85,   -88,   -124,  -133,  // Opposed Pawns
    -160,  -52,   -58,   -61,   // Opposed Pawns
    -103,  72,    -126,  30,    // Opposed Pawns
    -238,  267,   362,   173,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    -76,   -5,    55,    32,    // Opposed Pawns
    -113,  -1,    18,    68,    // Opposed Pawns
    45,    56,    -21,   251,   // Opposed Pawns
    232,   582,   370,   241,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    -110,  -157,  69,    -18,   // Opposed Pawns
    -130,  -113,  -60,   -19,   // Opposed Pawns
    -83,   -83,   -2,    110,   // Opposed Pawns
    21,    -43,   147,   149,   // Opposed Pawns
    479,   515,   493,   502,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    -10,   74,    90,    50,    // Opposed Pawns
    -132,  46,    179,   224,   // Opposed Pawns
    252,   289,   363,   322,   // Opposed Pawns
    233,   228,   228,   227,   // Opposed Pawns
    0,     0,     0,     0,     // Opposed Pawns
    -289,  -134,  33,    -14,   // Unopposed Pawns
    -311,  -69,   -131,  -132,  // Unopposed Pawns
    -298,  -65,   -72,   246,   // Unopposed Pawns
    -219,  -22,   321,   224,   // Unopposed Pawns
    341,   406,   593,   804,   // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    -275,  55,    170,   207,   // Unopposed Pawns
    -127,  -22,   217,   509,   // Unopposed Pawns
    91,    190,   434,   546,   // Unopposed Pawns
    879,   862,   832,   842,   // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    -222,  -163,  142,   102,   // Unopposed Pawns
    -124,  -48,   -70,   124,   // Unopposed Pawns
    55,    -67,   138,   365,   // Unopposed Pawns
    339,   246,   544,   489,   // Unopposed Pawns
    649,   693,   668,   675,   // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    -47,   94,    223,   300,   // Unopposed Pawns
    268,   328,   262,   412,   // Unopposed Pawns
    627,   529,   512,   527,   // Unopposed Pawns
    86,    87,    86,    87,    // Unopposed Pawns
    0,     0,     0,     0,     // Unopposed Pawns
    -193,  23,    131,   32,    // Covered Passed Pawns
    -166,  116,   -102,  -6,    // Covered Passed Pawns
    19,    260,   -94,   107,   // Covered Passed Pawns
    -9,    456,   178,   384,   // Covered Passed Pawns
    -181,  660,   1095,  597,   // Covered Passed Pawns
    -113,  431,   1058,  1124,  // Covered Passed Pawns
    0,     0,     0,     0,     // Covered Passed Pawns
    -209,  157,   398,   173,   // Covered Passed Pawns
    134,   466,   46,    258,   // Covered Passed Pawns
    273,   728,   306,   378,   // Covered Passed Pawns
    676,   1236,  1164,  1132,  // Covered Passed Pawns
    872,   895,   860,   853,   // Covered Passed Pawns
    -272,  -30,   290,   131,   // Covered Passed Pawns
    -15,   93,    23,    26,    // Covered Passed Pawns
    221,   479,   45,    131,   // Covered Passed Pawns
    459,   774,   583,   631,   // Covered Passed Pawns
    1310,  1381,  1365,  1355,  // Covered Passed Pawns
    73,    73,    73,    73,    // Covered Passed Pawns
    0,     0,     290,   131,   // Covered Passed Pawns
    -15,   157,   398,   173,   // Covered Passed Pawns
    221,   479,   46,    258,   // Covered Passed Pawns
    459,   774,   583,   631,   // Covered Passed Pawns
    1310,  1381,  1365,  1355,  // Covered Passed Pawns
    872,   895,   860,   853,   // Covered Passed Pawns
    86,    -11,   -263,  -249,  // Uncovered Passed Pawns
    224,   -14,   -122,  -80,   // Uncovered Passed Pawns
    899,   197,   -337,  -22,   // Uncovered Passed Pawns
    1778,  431,   -206,  -18,   // Uncovered Passed Pawns
    3199,  795,   16,    15,    // Uncovered Passed Pawns
    3474,  1587,  647,   735,   // Uncovered Passed Pawns
    0,     0,     0,     0,     // Uncovered Passed Pawns
    40,    149,   -55,   -68,   // Uncovered Passed Pawns
    533,   348,   34,    242,   // Uncovered Passed Pawns
    1394,  914,   23,    -24,   // Uncovered Passed Pawns
    1974,  1721,  1115,  1147,  // Uncovered Passed Pawns
    2388,  2509,  2442,  2443,  // Uncovered Passed Pawns
    46,    -2,    -181,  -289,  // Uncovered Passed Pawns
    -87,   -34,   -71,   -66,   // Uncovered Passed Pawns
    541,   411,   213,   308,   // Uncovered Passed Pawns
    1112,  1048,  905,   966,   // Uncovered Passed Pawns
    2142,  2119,  2057,  2069,  // Uncovered Passed Pawns
    1503,  1475,  1479,  1477,  // Uncovered Passed Pawns
    46,    0,     0,     0,     // Uncovered Passed Pawns
    40,    149,   -55,   -66,   // Uncovered Passed Pawns
    541,   411,   213,   308,   // Uncovered Passed Pawns
    1394,  1048,  905,   966,   // Uncovered Passed Pawns
    2142,  2119,  2057,  2069,  // Uncovered Passed Pawns
    2388,  2509,  2442,  2443   // Uncovered Passed Pawns
};


// SF 10kn dset, 0.5 mix
//const std::array<int, (272*4)> eval_weights = {
//    -995,  -532,  -16,   197,   // King Piece Square Table
//    -695,  -126,  128,   323,   // King Piece Square Table
//    -144,  48,    -361,  -37,   // King Piece Square Table
//    -102,  60,    -458,  -79,   // King Piece Square Table
//    -76,   25,    266,   -201,  // King Piece Square Table
//    199,   184,   106,   -353,  // King Piece Square Table
//    253,   359,   -162,  -558,  // King Piece Square Table
//    382,   76,    336,   -32,   // King Piece Square Table
//    430,   95,    438,   -112,  // King Piece Square Table
//    583,   -101,  458,   392,   // King Piece Square Table
//    2866,  2254,  2516,  2724,  // Knight Piece Square Table
//    2670,  2450,  3370,  3155,  // Knight Piece Square Table
//    2877,  2549,  3238,  3130,  // Knight Piece Square Table
//    2779,  2608,  3508,  3224,  // Knight Piece Square Table
//    2965,  2596,  3215,  3108,  // Knight Piece Square Table
//    2810,  2656,  3436,  3318,  // Knight Piece Square Table
//    2940,  2693,  3451,  3393,  // Knight Piece Square Table
//    2931,  2749,  3480,  3399,  // Knight Piece Square Table
//    2903,  2795,  3624,  3364,  // Knight Piece Square Table
//    2866,  2955,  3479,  3374,  // Knight Piece Square Table
//    1241,  577,   324,   423,   // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    400,   44,    260,   830,   // Bishop Pair
//    163,   1,     0,     30,    // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     75,    50,    30,    // Abstract Piece Activity Bonus
//    102,   1,     12,    0,     // Abstract Piece Activity Bonus
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
//    -74,   0,     0,     0,     // Double Pawn Penalty
//    -14,   -111,  0,     -84,   // Isolated Pawn
//    100,   100,   100,   100,   // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    -96,   137,   -133,  -41,   // King Vector Exposure
//    -62,   91,    -147,  -12,   // King Vector Exposure
//    8,     352,   40,    137,   // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    0,     15,    720,   469,   // Pawn Attack
//    0,     0,     305,   344,   // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     -55,   0,     // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    0,     0,     -66,   0,     // King Attack Distance
//    0,     -163,  0,     0,     // King Attack Distance
//    0,     0,     1,     2,     // King Attack Danger
//    0,     0,     2,     11,    // King Attack Danger
//    0,     0,     80,    19,    // King Attack Danger
//    290,   212,   0,     0,     // King Attack Danger
//    -434,  -441,  -528,  -308,  // Safe Knight Square Count
//    -3,    -32,   -499,  -193,  // Safe Knight Square Count
//    0,     -20,   -247,  -117,  // Safe Knight Square Count
//    0,     0,     -133,  -73,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    0,     0,     140,   48,    // Safe Knight Square Count
//    0,     0,     198,   85,    // Safe Knight Square Count
//    0,     0,     198,   85,    // Safe Knight Square Count
//    0,     0,     198,   85,    // Safe Knight Square Count
//    1752,  2591,  2054,  1891,  // Bishop Mobility
//    1966,  2637,  2395,  2029,  // Bishop Mobility
//    2112,  2723,  2601,  2123,  // Bishop Mobility
//    2122,  2796,  2772,  2215,  // Bishop Mobility
//    2215,  2928,  2805,  2288,  // Bishop Mobility
//    2216,  3015,  2925,  2330,  // Bishop Mobility
//    2230,  3123,  3034,  2365,  // Bishop Mobility
//    2260,  3222,  3038,  2422,  // Bishop Mobility
//    2331,  3239,  3183,  2422,  // Bishop Mobility
//    2342,  3245,  3322,  2436,  // Bishop Mobility
//    2345,  3245,  3322,  2635,  // Bishop Mobility
//    2357,  3245,  3403,  2754,  // Bishop Mobility
//    2594,  3245,  3403,  2835,  // Bishop Mobility
//    2595,  3246,  3404,  2839,  // Bishop Mobility
//    4574,  4802,  3716,  3389,  // Rook Mobility
//    4629,  4928,  3987,  3487,  // Rook Mobility
//    4786,  5207,  4004,  3517,  // Rook Mobility
//    4956,  5297,  4036,  3556,  // Rook Mobility
//    5036,  5383,  4036,  3560,  // Rook Mobility
//    5265,  5398,  4100,  3620,  // Rook Mobility
//    5307,  5486,  4154,  3653,  // Rook Mobility
//    5307,  5486,  4255,  3682,  // Rook Mobility
//    5312,  5494,  4443,  3695,  // Rook Mobility
//    5321,  5549,  4503,  3695,  // Rook Mobility
//    5336,  5574,  4605,  3763,  // Rook Mobility
//    5407,  5574,  4718,  3767,  // Rook Mobility
//    5484,  5582,  4772,  3867,  // Rook Mobility
//    5484,  5583,  4878,  3962,  // Rook Mobility
//    5484,  5609,  4899,  3962,  // Rook Mobility
//    0,     0,     285,   244,   // Rook on Open File
//    14447, 9512,  8406,  8095,  // Queen Mobility
//    14522, 9587,  8428,  8163,  // Queen Mobility
//    14537, 9603,  8445,  8266,  // Queen Mobility
//    14544, 9608,  8446,  8271,  // Queen Mobility
//    14667, 9691,  8456,  8328,  // Queen Mobility
//    14730, 9746,  8466,  8334,  // Queen Mobility
//    14731, 9747,  8466,  8334,  // Queen Mobility
//    14800, 9768,  8466,  8334,  // Queen Mobility
//    14835, 9778,  8466,  8334,  // Queen Mobility
//    14898, 9804,  8466,  8334,  // Queen Mobility
//    14965, 9837,  8466,  8334,  // Queen Mobility
//    14988, 9842,  8466,  8334,  // Queen Mobility
//    14988, 9842,  8466,  8334,  // Queen Mobility
//    15130, 9847,  8466,  8334,  // Queen Mobility
//    15169, 9855,  8466,  8334,  // Queen Mobility
//    15194, 9901,  8466,  8334,  // Queen Mobility
//    15196, 9918,  8523,  8334,  // Queen Mobility
//    15196, 9918,  8524,  8428,  // Queen Mobility
//    15196, 9918,  8538,  8543,  // Queen Mobility
//    15196, 9918,  8548,  8672,  // Queen Mobility
//    15196, 9918,  8549,  8681,  // Queen Mobility
//    15196, 9918,  8549,  8690,  // Queen Mobility
//    15196, 9918,  8549,  8690,  // Queen Mobility
//    15196, 9918,  8549,  8690,  // Queen Mobility
//    15196, 9918,  8549,  8690,  // Queen Mobility
//    15196, 9918,  8549,  8690,  // Queen Mobility
//    15196, 9918,  8549,  8690,  // Queen Mobility
//    15196, 9918,  8549,  8690,  // Queen Mobility
//    40,    -6,    867,   49,    // Safe Checks
//    155,   -70,   215,   -20,   // Unsafe Checks
//    165,   34,    -323,  -51,   // Unprotected Pieces
//    301,   298,   287,   286,   // Bishop's Position Relative To King
//    342,   292,   124,   190,   // Bishop's Position Relative To King
//    285,   442,   282,   167,   // Bishop's Position Relative To King
//    314,   284,   320,   124,   // Bishop's Position Relative To King
//    497,   309,   171,   282,   // Bishop's Position Relative To King
//    629,   360,   31,    107,   // Bishop's Position Relative To King
//    676,   528,   -31,   58,    // Bishop's Position Relative To King
//    422,   400,   119,   29,    // Bishop's Position Relative To King
//    426,   425,   420,   342,   // Bishop's Position Relative To King
//    573,   193,   268,   224,   // Bishop's Position Relative To King
//    456,   244,   347,   287,   // Bishop's Position Relative To King
//    614,   265,   114,   168,   // Bishop's Position Relative To King
//    526,   302,   250,   247,   // Bishop's Position Relative To King
//    421,   231,   244,   185,   // Bishop's Position Relative To King
//    319,   347,   416,   240,   // Bishop's Position Relative To King
//    517,   463,   355,   352,   // Bishop's Position Relative To King
//    532,   289,   252,   129,   // Bishop's Position Relative To King
//    264,   474,   308,   205,   // Bishop's Position Relative To King
//    488,   272,   186,   163,   // Bishop's Position Relative To King
//    348,   355,   415,   162,   // Bishop's Position Relative To King
//    461,   174,   448,   111,   // Bishop's Position Relative To King
//    592,   547,   358,   98,    // Bishop's Position Relative To King
//    334,   361,   305,   133,   // Bishop's Position Relative To King
//    197,   484,   373,   120,   // Bishop's Position Relative To King
//    548,   311,   95,    67,    // Bishop's Position Relative To King
//    172,   551,   377,   122,   // Bishop's Position Relative To King
//    610,   709,   250,   156,   // Bishop's Position Relative To King
//    198,   419,   356,   62,    // Bishop's Position Relative To King
//    -79,   538,   554,   -10,   // Bishop's Position Relative To King
//    187,   362,   390,   101,   // Bishop's Position Relative To King
//    496,   591,   822,   435,   // Bishop's Position Relative To King
//    -25,   320,   735,   210,   // Bishop's Position Relative To King
//    -209,  611,   514,   320,   // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -7,    314,   533,   -145,  // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    -237,  35,    115,   -32,   // Opposed Pawns
//    -166,  15,    18,    -60,   // Opposed Pawns
//    -204,  60,    58,    13,    // Opposed Pawns
//    -142,  147,   9,     77,    // Opposed Pawns
//    -230,  263,   409,   176,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -155,  60,    154,   102,   // Opposed Pawns
//    -154,  51,    139,   132,   // Opposed Pawns
//    6,     53,    147,   312,   // Opposed Pawns
//    132,   595,   413,   178,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -226,  -43,   172,   37,    // Opposed Pawns
//    -144,  -43,   67,    56,    // Opposed Pawns
//    -125,  -16,   131,   176,   // Opposed Pawns
//    -41,   -10,   308,   157,   // Opposed Pawns
//    545,   536,   532,   533,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -169,  192,   152,   113,   // Opposed Pawns
//    -175,  84,    273,   278,   // Opposed Pawns
//    163,   186,   515,   456,   // Opposed Pawns
//    401,   377,   372,   371,   // Opposed Pawns
//    0,     0,     0,     0,     // Opposed Pawns
//    -447,  34,    64,    79,    // Unopposed Pawns
//    -407,  81,    -90,   -2,    // Unopposed Pawns
//    -389,  46,    -3,    340,   // Unopposed Pawns
//    -254,  64,    319,   336,   // Unopposed Pawns
//    279,   347,   895,   963,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -373,  158,   174,   330,   // Unopposed Pawns
//    -269,  56,    270,   607,   // Unopposed Pawns
//    -21,   154,   479,   678,   // Unopposed Pawns
//    802,   791,   847,   827,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -402,  27,    169,   215,   // Unopposed Pawns
//    -276,  77,    5,     266,   // Unopposed Pawns
//    -17,   -35,   247,   475,   // Unopposed Pawns
//    339,   231,   642,   582,   // Unopposed Pawns
//    761,   717,   708,   716,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -220,  183,   225,   401,   // Unopposed Pawns
//    60,    360,   301,   536,   // Unopposed Pawns
//    614,   589,   543,   546,   // Unopposed Pawns
//    143,   143,   143,   143,   // Unopposed Pawns
//    0,     0,     0,     0,     // Unopposed Pawns
//    -190,  108,   113,   36,    // Covered Passed Pawns
//    -120,  170,   -137,  146,   // Covered Passed Pawns
//    56,    295,   -118,  225,   // Covered Passed Pawns
//    68,    433,   203,   406,   // Covered Passed Pawns
//    57,    582,   1138,  514,   // Covered Passed Pawns
//    149,   539,   789,   787,   // Covered Passed Pawns
//    0,     0,     0,     0,     // Covered Passed Pawns
//    -338,  292,   430,   188,   // Covered Passed Pawns
//    101,   486,   23,    413,   // Covered Passed Pawns
//    243,   666,   341,   533,   // Covered Passed Pawns
//    549,   1158,  1254,  1064,  // Covered Passed Pawns
//    1043,  1163,  1114,  1104,  // Covered Passed Pawns
//    -416,  151,   230,   169,   // Covered Passed Pawns
//    -145,  261,   -20,   123,   // Covered Passed Pawns
//    190,   623,   -19,   134,   // Covered Passed Pawns
//    419,   781,   598,   640,   // Covered Passed Pawns
//    1602,  1590,  1566,  1557,  // Covered Passed Pawns
//    82,    82,    82,    82,    // Covered Passed Pawns
//    0,     151,   230,   169,   // Covered Passed Pawns
//    -145,  292,   430,   188,   // Covered Passed Pawns
//    190,   623,   23,    413,   // Covered Passed Pawns
//    419,   781,   598,   640,   // Covered Passed Pawns
//    1602,  1590,  1566,  1557,  // Covered Passed Pawns
//    1043,  1163,  1114,  1104,  // Covered Passed Pawns
//    190,   -47,   -314,  -24,   // Uncovered Passed Pawns
//    372,   -38,   -124,  -42,   // Uncovered Passed Pawns
//    1026,  112,   -339,  108,   // Uncovered Passed Pawns
//    2120,  204,   -271,  80,    // Uncovered Passed Pawns
//    3780,  464,   -25,   12,    // Uncovered Passed Pawns
//    4853,  1084,  292,   277,   // Uncovered Passed Pawns
//    0,     0,     0,     0,     // Uncovered Passed Pawns
//    -117,  279,   -20,   4,     // Uncovered Passed Pawns
//    492,   396,   69,    371,   // Uncovered Passed Pawns
//    1590,  830,   -88,   30,    // Uncovered Passed Pawns
//    2372,  1652,  719,   756,   // Uncovered Passed Pawns
//    2841,  2973,  2795,  2806,  // Uncovered Passed Pawns
//    72,    59,    -61,   -304,  // Uncovered Passed Pawns
//    -35,   144,   25,    -18,   // Uncovered Passed Pawns
//    633,   490,   115,   266,   // Uncovered Passed Pawns
//    1252,  1024,  729,   927,   // Uncovered Passed Pawns
//    2495,  2339,  2159,  2211,  // Uncovered Passed Pawns
//    2898,  2845,  2824,  2828,  // Uncovered Passed Pawns
//    72,    59,    0,     0,     // Uncovered Passed Pawns
//    -35,   279,   25,    4,     // Uncovered Passed Pawns
//    633,   490,   115,   371,   // Uncovered Passed Pawns
//    1590,  1024,  729,   927,   // Uncovered Passed Pawns
//    2495,  2339,  2159,  2211,  // Uncovered Passed Pawns
//    2898,  2973,  2824,  2828   // Uncovered Passed Pawns
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

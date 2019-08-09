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

// Clustering Parameters
// ------------------------------------------------------------------------------------------------

// Normalized Fuzzy C-Means Parameters
// First Centroid set to zero vector normalized. Last centroid set to start pos.
// m = 3
constexpr std::array<double, 8+(4*4)> NFCM_params = {
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

// Search Parameters
// ------------------------------------------------------------------------------------------------

constexpr std::array<int, 117> search_params = {
    2000, // Hash Move
    80,   // Killer Move
    77,   // Killer Move
    72,   // Counter Move
    -60,  // Moving and Target Piece Type
    80,   // Moving and Target Piece Type
    89,   // Moving and Target Piece Type
    202,  // Moving and Target Piece Type
    333,  // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    5,    // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    139,  // Moving and Target Piece Type
    290,  // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    0,    // Moving and Target Piece Type
    -65,  // Moving and Target Piece Type
    69,   // Moving and Target Piece Type
    131,  // Moving and Target Piece Type
    286,  // Moving and Target Piece Type
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

// Evaluation Parameters
// ------------------------------------------------------------------------------------------------

// Eval parameters for NFCM
// Winter Dset d4, reduced base learn rate
// NFCM is no longer used as of Winter 0.6.2
constexpr std::array<int, (272*4)> eval_weights = {
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

}

#endif /* SRC_GENERAL_HARDCODED_PARAMS_H_ */

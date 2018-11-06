/*
 * hardcoded_params.h
 *
 *  Created on: Nov 6, 2018
 *      Author: jonathan
 */

#ifndef SRC_GENERAL_HARDCODED_PARAMS_H_
#define SRC_GENERAL_HARDCODED_PARAMS_H_

#include <array>

//The arrays in this file are automatically generated!
namespace hardcode {

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

const std::array<int, (174*4)> eval_weights = {
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

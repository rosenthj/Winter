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
//    // Component 0 weights
//    232312,
//    // Component 0 means
//    1.14705, 3.0032, 3.64416, 3.9962, 1.99397, 7.54282,
//    // Component 0 covariances
//    1.36886, -0.474315, -0.421452, -0.00679908, -0.00599843, -0.0201269,
//    -0.474315, 1.29115, 0.180038, -0.00225386, -0.00218856, 0.0190156,
//    -0.421452, 0.180038, 0.365504, -0.000954343, -0.00082838, -0.00343129,
//    -0.00679908, -0.00225386, -0.000954343, 0.123464, -0.000702406, -0.00233938,
//    -0.00599843, -0.00218856, -0.00082838, -0.000702406, 0.0585644, 0.00605468,
//    -0.0201269, 0.0190156, -0.00343129, -0.00233938, 0.00605468, 1.2366,
//
//    // Component 1 weights
//    360226,
//    // Component 1 means
//    2.71602, 1.35498, 1.5448, 2.33994, 0.956696, 7.5496,
//    // Component 1 covariances
//    2.85683, -0.236128, -0.0900309, -0.200555, 0.119449, -0.268961,
//    -0.236128, 1.4422, 0.559829, 0.679997, -0.23237, 0.358512,
//    -0.0900309, 0.559829, 1.67375, 0.868948, -0.0611065, 0.665372,
//    -0.200555, 0.679997, 0.868948, 2.25224, -0.0223966, 1.18327,
//    0.119449, -0.23237, -0.0611065, -0.0223966, 0.602407, 0.667716,
//    -0.268961, 0.358512, 0.665372, 1.18327, 0.667716, 5.96681,
//
//    // Component 2 weights
//    250071,
//    // Component 2 means
//    1.75169, 1.47608, 1.62392, 2.86597, 2.02147, 6.9647,
//    // Component 2 covariances
//    1.98944, -0.00802263, 0.0437159, -0.198428, 0.0106339, -0.0231454,
//    -0.00802263, 1.56616, 0.299455, 0.477873, -0.00421203, 0.285777,
//    0.0437159, 0.299455, 1.29074, 0.206236, -0.00226505, 0.23902,
//    -0.198428, 0.477873, 0.206236, 1.91085, -0.0101433, 0.374712,
//    0.0106339, -0.00421203, -0.00226505, -0.0101433, 0.0451938, -0.000140486,
//    -0.0231454, 0.285777, 0.23902, 0.374712, -0.000140486, 1.23157,
//
//    // Component 3 weights
//    341882,
//    // Component 3 means
//    1.99046, 0.474488, 0.99657, 1.70535, -0.0163528, 5.77626,
//    // Component 3 covariances
//    1.8794, 0.307235, 0.146055, -0.0676678, -0.00742074, 0.309588,
//    0.307235, 0.477411, 0.0138136, 0.0168707, -0.00562672, 0.200934,
//    0.146055, 0.0138136, 1.0498, 0.00846862, -0.00361105, 0.357167,
//    -0.0676678, 0.0168707, 0.00846862, 1.72611, -0.00360249, 0.797656,
//    -0.00742074, -0.00562672, -0.00361105, -0.00360249, 0.0462161, -0.0168335,
//    0.309588, 0.200934, 0.357167, 0.797656, -0.0168335, 4.68588
//};

//Longstanding
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

//SL181109
//const std::array<int, (174*4)> eval_weights = {
//    128,   -416,  -480,  -302,  // King Piece Square Table
//    236,   -158,  -103,  -157,  // King Piece Square Table
//    -6,    -249,  -279,  -100,  // King Piece Square Table
//    -17,   -296,  -362,  -37,   // King Piece Square Table
//    304,   -45,   155,   -20,   // King Piece Square Table
//    -23,   56,    103,   46,    // King Piece Square Table
//    -199,  72,    -34,   75,    // King Piece Square Table
//    -120,  250,   248,   119,   // King Piece Square Table
//    -381,  315,   269,   171,   // King Piece Square Table
//    78,    472,   483,   206,   // King Piece Square Table
//    153,   2132,  2160,  2041,  // Knight Piece Square Table
//    711,   2455,  2743,  2132,  // Knight Piece Square Table
//    584,   2458,  2690,  2074,  // Knight Piece Square Table
//    693,   2603,  2765,  2163,  // Knight Piece Square Table
//    531,   2501,  2679,  2187,  // Knight Piece Square Table
//    610,   2469,  2668,  2139,  // Knight Piece Square Table
//    749,   2555,  2811,  2145,  // Knight Piece Square Table
//    759,   2499,  2760,  2192,  // Knight Piece Square Table
//    782,   2608,  2825,  2244,  // Knight Piece Square Table
//    698,   2627,  2764,  2272,  // Knight Piece Square Table
//    167,   562,   526,   679,   // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    0,     0,     0,     0,     // Piece Base Value
//    1,     323,   249,   246,   // Bishop Pair
//    0,     42,    15,    42,    // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    31,    65,    76,    54,    // Abstract Piece Activity Bonus
//    17,    44,    47,    49,    // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Abstract Piece Activity Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     2,     0,     145,   // Passed Pawn Bonus
//    79,    219,   200,   280,   // Passed Pawn Bonus
//    385,   443,   573,   341,   // Passed Pawn Bonus
//    0,     113,   0,     138,   // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    0,     0,     0,     0,     // Passed Pawn Bonus
//    10,    97,    59,    161,   // Passed Pawn Bonus
//    118,   438,   352,   483,   // Passed Pawn Bonus
//    328,   900,   831,   803,   // Passed Pawn Bonus
//    628,   1538,  1288,  942,   // Passed Pawn Bonus
//    -53,   -42,   -32,   -113,  // Double Pawn Penalty
//    60,    89,    80,    34,    // Tempo Bonus
//    0,     0,     0,     0,     // Direct Pawn Shield Bonus
//    5,     -20,   -39,   17,    // King Vector Exposure
//    -10,   -39,   -48,   -4,    // King Vector Exposure
//    122,   82,    148,   185,   // Knight Outpost
//    0,     0,     0,     0,     // Castling Right
//    182,   365,   368,   220,   // Pawn Attack
//    130,   263,   220,   45,    // Minor Attack
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    0,     0,     0,     0,     // King Defence Distance
//    -25,   -37,   -97,   0,     // King Attack Distance
//    0,     0,     0,     0,     // King Attack Distance
//    -26,   -71,   -94,   -19,   // King Attack Distance
//    0,     -73,   -58,   -457,  // King Attack Distance
//    21,    3,     32,    0,     // King Attack Danger
//    2,     6,     22,    9,     // King Attack Danger
//    28,    30,    35,    22,    // King Attack Danger
//    4,     56,    0,     764,   // King Attack Danger
//    -322,  -834,  -719,  -1005, // Safe Knight Square Count
//    -164,  -431,  -353,  -499,  // Safe Knight Square Count
//    -83,   -232,  -180,  -232,  // Safe Knight Square Count
//    -43,   -100,  -76,   -77,   // Safe Knight Square Count
//    0,     0,     0,     0,     // Safe Knight Square Count
//    16,    77,    77,    18,    // Safe Knight Square Count
//    17,    138,   141,   18,    // Safe Knight Square Count
//    45,    156,   150,   18,    // Safe Knight Square Count
//    46,    156,   150,   18,    // Safe Knight Square Count
//    414,   1462,  1388,  1351,  // Bishop Mobility
//    430,   1653,  1565,  1626,  // Bishop Mobility
//    563,   1844,  1774,  1848,  // Bishop Mobility
//    601,   1991,  1936,  2015,  // Bishop Mobility
//    657,   2118,  2081,  2146,  // Bishop Mobility
//    725,   2228,  2190,  2250,  // Bishop Mobility
//    754,   2318,  2295,  2313,  // Bishop Mobility
//    779,   2388,  2383,  2394,  // Bishop Mobility
//    784,   2458,  2448,  2411,  // Bishop Mobility
//    801,   2541,  2524,  2444,  // Bishop Mobility
//    903,   2614,  2619,  2444,  // Bishop Mobility
//    985,   2687,  2683,  2444,  // Bishop Mobility
//    992,   2694,  2741,  2444,  // Bishop Mobility
//    1198,  2783,  2742,  2444,  // Bishop Mobility
//    -862,  2978,  3157,  2691,  // Rook Mobility
//    -797,  3061,  3276,  2928,  // Rook Mobility
//    -787,  3141,  3337,  3053,  // Rook Mobility
//    -712,  3216,  3405,  3191,  // Rook Mobility
//    -708,  3310,  3459,  3324,  // Rook Mobility
//    -638,  3413,  3562,  3426,  // Rook Mobility
//    -577,  3498,  3650,  3523,  // Rook Mobility
//    -505,  3610,  3758,  3607,  // Rook Mobility
//    -452,  3706,  3854,  3715,  // Rook Mobility
//    -405,  3798,  3951,  3790,  // Rook Mobility
//    -376,  3889,  4022,  3837,  // Rook Mobility
//    -297,  3969,  4095,  3883,  // Rook Mobility
//    -229,  4066,  4194,  3910,  // Rook Mobility
//    -224,  4107,  4284,  3910,  // Rook Mobility
//    -221,  4231,  4341,  3910,  // Rook Mobility
//    5583,  6269,  5363,  6088,  // Queen Mobility
//    5717,  6410,  5538,  6250,  // Queen Mobility
//    5731,  6583,  5540,  6250,  // Queen Mobility
//    5773,  6682,  5584,  6258,  // Queen Mobility
//    5774,  6746,  5621,  6258,  // Queen Mobility
//    5836,  6818,  5665,  6363,  // Queen Mobility
//    5858,  6903,  5723,  6364,  // Queen Mobility
//    5880,  6963,  5769,  6370,  // Queen Mobility
//    5902,  7029,  5814,  6415,  // Queen Mobility
//    5946,  7098,  5845,  6500,  // Queen Mobility
//    5947,  7131,  5876,  6528,  // Queen Mobility
//    5947,  7188,  5915,  6569,  // Queen Mobility
//    5948,  7207,  5939,  6569,  // Queen Mobility
//    5948,  7207,  5961,  6569,  // Queen Mobility
//    5948,  7207,  5978,  6569,  // Queen Mobility
//    5948,  7207,  5992,  6569,  // Queen Mobility
//    5948,  7207,  5992,  6569,  // Queen Mobility
//    5948,  7207,  5993,  6569,  // Queen Mobility
//    5948,  7207,  5994,  6569,  // Queen Mobility
//    5949,  7207,  5994,  6569,  // Queen Mobility
//    5949,  7207,  5994,  6569,  // Queen Mobility
//    5956,  7207,  5994,  6569,  // Queen Mobility
//    5956,  7207,  5994,  6569,  // Queen Mobility
//    5977,  7207,  5994,  6569,  // Queen Mobility
//    5993,  7207,  5994,  6569,  // Queen Mobility
//    5997,  7207,  5994,  6569,  // Queen Mobility
//    6009,  7207,  5994,  6574,  // Queen Mobility
//    6010,  7207,  5994,  6574,  // Queen Mobility
//    10,    78,    159,   58,    // Safe Checks
//    29,    47,    70,    76,    // Unsafe Checks
//    -64,   -71,   -102,  -17,   // Unprotected Pieces
//    181,   82,    144,   241,   // Bishop's Position Relative To King
//    27,    167,   154,   -77,   // Bishop's Position Relative To King
//    85,    60,    75,    85,    // Bishop's Position Relative To King
//    3,     123,   -22,   59,    // Bishop's Position Relative To King
//    -90,   125,   5,     126,   // Bishop's Position Relative To King
//    -50,   34,    -21,   103,   // Bishop's Position Relative To King
//    -58,   69,    74,    105,   // Bishop's Position Relative To King
//    -143,  21,    0,     91,    // Bishop's Position Relative To King
//    104,   255,   71,    123,   // Bishop's Position Relative To King
//    -3,    131,   -4,    31,    // Bishop's Position Relative To King
//    110,   115,   25,    53,    // Bishop's Position Relative To King
//    -139,  7,     -106,  54,    // Bishop's Position Relative To King
//    -34,   57,    40,    87,    // Bishop's Position Relative To King
//    -74,   -41,   27,    45,    // Bishop's Position Relative To King
//    -27,   -6,    94,    51,    // Bishop's Position Relative To King
//    128,   211,   78,    102,   // Bishop's Position Relative To King
//    7,     63,    -39,   37,    // Bishop's Position Relative To King
//    47,    57,    -33,   56,    // Bishop's Position Relative To King
//    -26,   -46,   -58,   39,    // Bishop's Position Relative To King
//    -27,   0,     46,    81,    // Bishop's Position Relative To King
//    -104,  -92,   28,    37,    // Bishop's Position Relative To King
//    81,    48,    -11,   63,    // Bishop's Position Relative To King
//    18,    -23,   -37,   21,    // Bishop's Position Relative To King
//    -25,   -17,   -4,    46,    // Bishop's Position Relative To King
//    -130,  -117,  -96,   23,    // Bishop's Position Relative To King
//    26,    78,    92,    80,    // Bishop's Position Relative To King
//    -18,   98,    100,   56,    // Bishop's Position Relative To King
//    -121,  -92,   19,    10,    // Bishop's Position Relative To King
//    41,    45,    57,    30,    // Bishop's Position Relative To King
//    -88,   -59,   -12,   30,    // Bishop's Position Relative To King
//    371,   539,   739,   183,   // Bishop's Position Relative To King
//    249,   32,    289,   12,    // Bishop's Position Relative To King
//    361,   222,   326,   -2,    // Bishop's Position Relative To King
//    0,     0,     0,     0,     // Bishop's Position Relative To King
//    21,    -25,   51,    -44,   // Bishop's Position Relative To King
//    0,     0,     0,     0      // Bishop's Position Relative To King
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

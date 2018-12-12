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
 * parse.h
 *
 *  Created on: Nov 1, 2016
 *      Author: Jonathan Rosenthal
 */

#ifndef GENERAL_PARSE_H_
#define GENERAL_PARSE_H_

#include "types.h"
#include <string>
#include <vector>
#include <sstream>

/**
 * The unicode namspace contains all the useful unicode characters which can be
 * used to print pretty unicode chess boards.
 */
namespace unicode {
const static std::string s_e_vec = "\u250C", s_w_vec = "\u2510",
    s_e_w_vec = "\u252c", n_s_w_vec = "\u2524", n_s_e_vec = "\u251C",
    n_s_e_w_vec = "\u253c", n_e_vec = "\u2514", n_e_w_vec = "\u2534",
    n_w_vec = "\u2518", n_s_vec = "\u2502", e_w_vec = "\u2500";
const static std::string e_box = "\u2590", w_box = "\u258C",
    full_box = "\u2588", empty_box = " ";
const static std::string full_empty_box[2] = { full_box, empty_box };

const static std::string w_king = "\u2654", w_queen = "\u2655",
    w_rook = "\u2656", w_bishop = "\u2657", w_knight = "\u2658",
    w_pawn = "\u2659";
const static std::string b_king = "\u265A", b_queen = "\u265B",
    b_rook = "\u265C", b_bishop = "\u265D", b_knight = "\u265E",
    b_pawn = "\u265F";
const static std::string no_piece = " ";

const static std::string pieces[2][7] = {
    { w_pawn, w_knight, w_bishop, w_rook, w_queen, w_king, no_piece },
    { b_pawn, b_knight, b_bishop, b_rook, b_queen, b_king, no_piece }
};

}

namespace parse {

//Translates the name of a square to its index representation
Square StringToSquare(std::string square_name);
//Retrieves the name of a square from its index representation
std::string SquareToString(Square square);
BitBoard StringToBitBoard(std::string square_name);
std::string MoveToString(Move move);
//StringToMove only translates a move string to move format but cannot
//recognize MoveTypes aside from promotions.
Move StringToMove(std::string move);
std::vector<std::string> split(const std::string &s, char delim);
template <typename T>
void Save2dVecToCSV(const std::vector< std::vector<T> > &vec, const std::string filename);

void PrintBitboard(const BitBoard bitboard);
int CountChars(int i);

}

#endif /* GENERAL_PARSE_H_ */

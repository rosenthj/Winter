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
 * parse.cc
 *
 *  Created on: Nov 2, 2016
 *      Author: Jonathan Rosenthal
 */

#include "parse.h"
#include <fstream>
#include <iostream>

namespace {

const std::string piece_abreviations[8] = {"n", "b", "r", "q", "N", "B", "R", "Q"};

std::vector<std::string> &splits(const std::string &s, char delimeter,
    std::vector<std::string> &elements) {
    std::stringstream string_stream(s);
    std::string item;
    while (std::getline(string_stream, item, delimeter)) {
        elements.push_back(item);
    }
    return elements;
}

}

namespace parse {

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    splits(s, delim, elems);
    return elems;
}

void PrintStandardRow(std::string first_delim, std::string mid_delim, std::string last_delim) {
  for (int column = 0; column < kBoardLength; column++) {
    if (column == 0) {
      std::cout << std::string(2,' ') << first_delim;
    }
    else {
      std::cout << mid_delim;
    }
      std::cout << unicode::e_w_vec << unicode::e_w_vec << unicode::e_w_vec;
    if (column == kBoardLength-1) {
      std::cout << last_delim;
    }
  }
  std::cout << std::endl;
}


void PrintBitboard(const BitBoard bitboard) {
  PrintStandardRow(unicode::s_e_vec, unicode::s_e_w_vec, unicode::s_w_vec);
  for (int row = kBoardLength; row > 0; row--) {
    std::cout << row << ' ' << unicode::n_s_vec;
    for (int column = 0; column < kBoardLength; column++) {
      std::cout << ' ' <<
          ((bitboard>>((row-1)*kBoardLength + column)) & 0x1)
          << ' ' << unicode::n_s_vec;
    }
    std::cout << std::endl;
    if (row > 1) {
      PrintStandardRow(unicode::n_s_e_vec, unicode::n_s_e_w_vec,
                       unicode::n_s_w_vec);
    }
  }
  PrintStandardRow(unicode::n_e_vec, unicode::n_e_w_vec, unicode::n_w_vec);
  std::cout << " ";
  for (char column = 'a'; column <= 'h'; column++) {
    std::cout << "   " << column;
  }
  std::cout << std::endl;
}

Square StringToSquare(std::string square_name) {
  return (square_name[0]-'a') + (square_name[1]-'1') * 8;
}

std::string SquareToString(Square square) {
  return std::string(1,(square % 8) + 'a') + std::string(1,(square / 8) + '1');
}

BitBoard StringToBitBoard(std::string square_name) {
  return GetSquareBitBoard(StringToSquare(square_name));
}

std::string MoveToString(Move move) {
  if (GetMoveType(move) >= kKnightPromotion) {
    return SquareToString(GetMoveSource(move))
        + SquareToString(GetMoveDestination(move))
        + piece_abreviations[GetMoveType(move)-kKnightPromotion];
  }
  return SquareToString(GetMoveSource(move))
      + SquareToString(GetMoveDestination(move));
}

Move StringToMove(std::string move) {
  Square source = (move[0] - 'a') + (move[1] - '1') * 8;
  Square destination = (move[2] - 'a') + (move[3] - '1') * 8;
  if (move.size() == 4) {
    return GetMove(source, destination);
  }
  for (int i = 0; i < 4; i++) {
    if (piece_abreviations[i][0] == move[4]
     || piece_abreviations[i+4][0] == move[4]) {
      return GetMove(source, destination, kKnightPromotion + i);
    }
  }
  //debug::Error("illegal promotion piece string! Parsed move length: "
  //    + std::to_string(move.size()) +" Parsed move: " + move);
  return 0;
}

int CountChars(int i) {
  int c = 1;
  if (i < 0) {
    i = -i;
    c++;
  }
  while (i > 9) {
    i /= 10;
    c++;
  }
  return c;
}

template <typename T>
void Save2dVecToCSV(const std::vector< std::vector<T> > &vec, const std::string filename) {
  std::ofstream file(filename);
  for (size_t j = 0; j < vec[0].size(); j++) {
    file << "X" << j;
    if (j < vec[0].size() - 1) {
      file << ", ";
    }
  }
  file << std::endl;
  for (size_t i = 0; i < vec.size(); i++) {
    for (size_t j = 0; j < vec[i].size(); j++) {
      file << vec[i][j];
      if (j < vec[i].size() - 1) {
        file << ", ";
      }
    }
    if (i < vec.size() - 1)
      file << std::endl;
  }
  file.flush();
  file.close();
}

template void Save2dVecToCSV<long>
  (const std::vector< std::vector<long> > &vec, const std::string filename);
template void Save2dVecToCSV<double>
  (const std::vector< std::vector<double> > &vec, const std::string filename);
template void Save2dVecToCSV<int>
  (const std::vector< std::vector<int> > &vec, const std::string filename);

}


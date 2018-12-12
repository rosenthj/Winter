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
 * data.h
 *
 *  Created on: Feb 12, 2017
 *      Author: Jonathan Rosenthal
 */

#ifndef DATA_H_
#define DATA_H_

#include "general/types.h"
#include "board.h"
#include <vector>
#include <string>

struct Game{
  Board board;
  std::vector<Move> moves;
  double result;
  void forward() {
    if (board.get_num_made_moves() < moves.size()) {
      board.Make(moves[board.get_num_made_moves()]);
    }
  }
  void set_to_position_after(size_t move) {
    while (move > board.get_num_made_moves()) {
      board.Make(moves[board.get_num_made_moves()]);
    }
    while (move < board.get_num_made_moves()) {
      board.UnMake();
    }
  }
};

namespace data {

std::vector<Game> LoadGames(size_t max_games = 1200000, std::string game_file = "data/CCRL.ucig");
void SaveBoardFens(std::string filename, std::vector<Board> boards);
std::vector<Board> LoadBoardFens(std::string filename = "data/sample_evals.fen");

void SetGameToRandom(Game &game);
void SetGamesToRandom(std::vector<Game> &games);
bool SetGameToRandomQuiescent(Game &game);
void SetGamesToRandomQuiescent(std::vector<Game> &games);

}

#endif /* DATA_H_ */

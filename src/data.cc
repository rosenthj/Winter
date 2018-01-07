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
 * data.cc
 *
 *  Created on: Feb 13, 2017
 *      Author: Jonathan
 */

#include "data.h"
#include <fstream>
#include <random>

namespace {
std::mt19937_64 rng;
}

namespace data {

std::vector<Game> LoadGames(size_t max_games, std::string game_file) {
  std::vector<Game> games;
  games.reserve(max_games);
  std::ifstream file(game_file);
  std::string in;
  while (std::getline(file, in) && games.size() < max_games) {
    std::vector<std::string> tokens = parse::split(in, ' ');
    Game game;
    for (size_t i = 0; i < tokens.size()-1; i++) {
      Move move = parse::StringToMove(tokens[i]);
      std::vector<Move> moves = game.board.GetMoves<kNonQuiescent>();
      for (size_t j = 0; j < moves.size(); j++) {
        if (GetMoveSource(moves[j]) == GetMoveSource(move)
            && GetMoveDestination(moves[j]) == GetMoveDestination(move)
            && (GetMoveType(moves[j]) < kKnightPromotion
                || GetMoveType(moves[j]) == GetMoveType(move))) {
          game.board.Make(moves[j]);
          game.moves.emplace_back(moves[j]);
          break;
        }
      }
      if (i == game.board.get_num_made_moves()) {
        std::cout << "read error!" << std::endl;
      }
    }
    if (tokens[tokens.size()-1].compare("1-0") == 0) {
      game.result = 1;
    }
    else if (tokens[tokens.size()-1].compare("0-1") == 0) {
      game.result = 0;
    }
    else if (tokens[tokens.size()-1].compare("1/2-1/2") == 0) {
      game.result = 0.5;
    }
    else {
      std::cout << "read error: result!" << std::endl;
    }
    games.emplace_back(game);
    if (games.size() % 10000 == 0) {
      std::cout << "loaded " << games.size() << " games!" << std::endl;
    }
    std::getline(file, in);
  }
  file.close();
  return games;
}

void SetGameToRandom(Game &game) {
  int index = (rng() % (game.moves.size()-3)) + rng() % 2;
  game.set_to_position_after(index);
}

}

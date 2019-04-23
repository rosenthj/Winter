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
 *      Author: Jonathan Rosenthal
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
      game.result = 0.5;
    }
    games.emplace_back(game);
    if (games.size() % 10000 == 0) {
      std::cout << "\rloaded " << games.size() << " games!" << std::flush;
    }
    std::getline(file, in);
  }
  std::cout << "\rfinished loading " << games.size() << " games!" << std::endl;
  file.close();
  return games;
}

void SetGameToRandom(Game &game) {
  int index = 4 + (rng() % (game.moves.size()-7)) + rng() % 2;
  game.set_to_position_after(index);
}

void SetGamesToRandom(std::vector<Game> &games) {
  std::cout << "Sampling Games!" << std::endl;
  for (size_t i = 0; i < games.size(); i++) {
    SetGameToRandom(games[i]);
  }
  std::cout << "Finished sampling!" << std::endl;
}

bool SetGameToRandomQuiescent(Game &game) {
  int failed_attempts = 0;
  if (game.moves.size() < 10) {
    return false;
  }
  while (failed_attempts < 100) {
    int index = (rng() % 2) + (rng() % (game.moves.size()-4));
    if (GetMoveType(game.moves[index]) < kCapture
        && GetMoveType(game.moves[index+1]) < kCapture) {
        //&& GetMoveType(game.moves[index+2]) < kCapture) {
        //&& GetMoveType(game.moves[index+3]) < kCapture) {
      game.set_to_position_after(index);
      if (!game.board.InCheck()) {
        break;
      }
    }
    failed_attempts++;
  }
  if (failed_attempts >= 100) {
    return false;
  }
  return true;
}

void SetGamesToRandomQuiescent(std::vector<Game> &games) {
  std::cout << "Sampling Games!" << std::endl;
  for (size_t i = 0; i < games.size(); i++) {
    SetGameToRandomQuiescent(games[i]);
  }
  std::cout << "Finished sampling!" << std::endl;
}

void SaveBoardFens(std::string filename, std::vector<Board> boards) {
  std::ofstream file(filename);
  for (Board board : boards) {
    std::vector<std::string> fen_tokens = board.GetFen();
    file << fen_tokens[0];
    for (size_t i = 1; i < fen_tokens.size(); i++) {
      file << " " << fen_tokens[i];
    }
    file << std::endl;
  }
  std::cout << "Saved Board FENs to " << filename << std::endl;
  file.close();
}

std::vector<Board> LoadBoardFens(std::string filename) {
  std::vector<Board> boards;
  std::ifstream file(filename);
  std::string in;
  while (std::getline(file, in)) {
    std::vector<std::string> fen_tokens = parse::split(in, ' ');
    boards.emplace_back();
    boards.back().SetBoard(fen_tokens);
    if (boards.size() % 10000 == 0) {
      std::cout << "\rloaded " << boards.size() << " positions!" << std::flush;
    }
  }
  std::cout << "\rfinished loading " << boards.size() << " positions!" << std::endl;
  file.close();
  return boards;
}

}

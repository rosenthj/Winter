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
 * test.cc
 *
 *  Created on: Dec 25, 2016
 *      Author: Jonathan
 */

#include "board.h"
#include "data.h"
#include "search.h"
#include "transposition.h"
#include "evaluation.h"
#include "general/parse.h"
#include "general/settings.h"
#include "benchmark.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>

namespace {

bool is_mate_score(Score score) {
  return (score < kMinScore + 2000) || (score > kMaxScore - 2000);
}

double Sigmoid(const Score score) {
  return 1 / (1 + std::exp(-score / 1024.0));
}

double SigmoidCrossEntropyLoss(Score score, double target) {
  if (is_mate_score(score)) {
    if (score < 0) {
      score = kMinScore;
    }
    else {
      score = kMaxScore;
    }
  }
  double prediction = Sigmoid(score);
  prediction = std::min(std::max(prediction, 0.0001), 0.9999);
  return target * std::log(prediction) + (1 - target) * std::log(1 - prediction);
}

const double kMinLoss = SigmoidCrossEntropyLoss(kMinScore, 0);
const double kMinDrawLoss = SigmoidCrossEntropyLoss(0, 0.5);

struct PerftTestSet {
  Board board;
  std::vector<std::pair<Depth, long> > depth_results;
  PerftTestSet(std::string line) {
    std::vector<std::string> tokens = parse::split(line, '|');
    board.SetBoard(parse::split(tokens[0], ' '));
    for (size_t i = 1; i < tokens.size(); i++) {
      std::vector<std::string> pair = parse::split(tokens[i], ',');
      depth_results.emplace_back(std::stoi(pair[0]), std::stol(pair[1]));
    }
  }
};

struct SymmetryTest {
  Board board1, board2;
  std::vector<std::pair<Depth, long> > depth_results;
  SymmetryTest(std::string line) {
    std::vector<std::string> tokens = parse::split(line, '|');
    board1.SetBoard(parse::split(tokens[0], ' '));
    board2.SetBoard(parse::split(tokens[1], ' '));
  }
};

}

namespace benchmark {

void SymmetrySuite() {
  std::vector<SymmetryTest> test_sets;
  std::string line;
  std::ifstream file("./tests/symmetry.test");
  while(std::getline(file, line)) {
    test_sets.emplace_back(line);
  }
  //Time start = now();
  long test_sets_passed = 0;
  for (SymmetryTest test_set : test_sets) {
    if (evaluation::ScoreBoard(test_set.board1)
        == evaluation::ScoreBoard(test_set.board2))
      test_sets_passed++;
  }
  if (test_sets_passed == test_sets.size()) {
    std::cout << "\033[32mPassed " << test_sets_passed << "/" << test_sets.size()
        << " test sets\033[0m"<< std::endl;
  }
  else {
    std::cout << "\033[31mPassed " << test_sets_passed << "/" << test_sets.size()
        << " test sets\033[0m"<< std::endl;
  }
}

void PerftSuite() {
  std::vector<PerftTestSet> test_sets;
  std::string line;
  std::ifstream file("./tests/perft.test");
  while(std::getline(file, line)) {
    test_sets.emplace_back(line);
  }
  Time start = now();
  long test_sets_passed = 0;
  for (int i = 0; i < test_sets.size(); i++) {
    PerftTestSet test_set = test_sets[i];
    bool passed = true;
    for (std::pair<Depth, long> depth_result : test_set.depth_results) {
      if (search::Perft(test_set.board, depth_result.first) != depth_result.second) {
        std::cout << "\033[31mFailed set " << i << " on input ("
            << depth_result.first << "," << depth_result.second << ")\033[0m"<< std::endl;
        passed = false;
        break;
      }
    }
    if (passed) {
      test_sets_passed++;
      std::cout << "\033[32mPassed set " << i << "\033[0m"<< std::endl;
    }
    else {
    }
  }
  Time end = now();
  auto total_time = std::chrono::duration_cast<Milliseconds>(end-start);

  if (test_sets_passed == test_sets.size()) {
    std::cout << "\033[32mPassed " << test_sets_passed << "/" << test_sets.size()
        << " test sets\033[0m"<< std::endl;
  }
  else {
    std::cout << "\033[31mPassed " << test_sets_passed << "/" << test_sets.size()
        << " test sets\033[0m"<< std::endl;
  }

  std::cout << "Elapsed Time: " << total_time.count() << std::endl;
}

int TimeToDepthSuite() {
  std::vector<Board> boards;
  std::vector<Depth> depths;
  std::ifstream file("./tests/time_to_depth.csv");
  std::string line;
  while (std::getline(file, line)) {
    std::vector<std::string> test_tokens = parse::split(line, ',');
    std::vector<std::string> fen_tokens = parse::split(test_tokens[0], ' ');
    Depth depth = atoi(test_tokens[1].c_str());
    Board board;
    board.SetBoard(fen_tokens);
    boards.push_back(board);
    depths.push_back(depth);
  }
  Time start = now();
  for (unsigned int t = 0; t < boards.size(); t++) {
    Time test_start = now();
    search::DepthSearch(boards[t], depths[t]);
    Time test_end = now();
    auto test_time = std::chrono::duration_cast<Milliseconds>(test_end-test_start);
    std::cout << "test " << t << " completed in " << test_time.count() << std::endl;
    search::clear_killers();
    table::ClearTable();
  }
  Time end = now();
  auto total_time = std::chrono::duration_cast<Milliseconds>(end-start);
  std::cout << "all tests completed in " << total_time.count() << std::endl;
  return total_time.count();
}

double EntropyLossTimedSuite(Milliseconds time_per_position) {
  std::cout << "Running benchmark!" << std::endl;
  std::vector<Game> games = data::LoadGames(6000, settings::kCEGTPath);
  //We want to count the number of drawn games in order to calculate the
  //minimal achievable error our engine can achieve on the data.
  double total_error = 0, decisive_error = 0, draw_error = 0;
  int draws = 0;
  int l_margin = games.size() / 10;
  int u_margin = l_margin * 2;
  search::set_print_info(false);
  for (int idx = 0; idx < games.size(); idx++) {
    int num_made_moves = games[idx].moves.size();
    games[idx].set_to_position_after(((l_margin+idx) * num_made_moves)
                                        / (games.size() + u_margin));
    double target = games[idx].result;
    if (target == 0.5) {
      draws++;
    }
    if (games[idx].board.get_turn() == kBlack) {
      target = 1 - target;
    }
    table::ClearTable();
    search::TimeSearch(games[idx].board, time_per_position);
    total_error += SigmoidCrossEntropyLoss(search::get_last_search_score(), target);
    if (target == 0.5) {
      draw_error +=
          SigmoidCrossEntropyLoss(search::get_last_search_score(), target) - kMinDrawLoss;
    }
    else {
      decisive_error += SigmoidCrossEntropyLoss(search::get_last_search_score(), target);
    }
    if ((idx + 1) % 100 == 0) {
      std::cout << (idx + 1) << std::endl
          << "Total Error: " << ((total_error - kMinLoss * (idx + 1 - draws)
              - kMinDrawLoss * draws) / (idx + 1)) << std::endl;
      if (idx + 1 > draws) {
        std::cout << "Decisive Error: " << ((decisive_error) / (idx + 1 - draws)) << std::endl;
      }
      if (draws > 0) {
        std::cout << "Draw Error: " << ((draw_error) / (draws)) << std::endl;
      }
    }
  }
  search::set_print_info(true);
  std::cout << "Final Avg Error: " << ((total_error - kMinLoss * (games.size() - draws)
      - kMinDrawLoss * draws) / games.size()) << std::endl;
  return (total_error - kMinLoss * (games.size() - draws)
                     - kMinDrawLoss * draws) / games.size();
}

double EntropyLossNodeSuite(size_t nodes_per_position) {
  std::cout << "Running benchmark!" << std::endl;
  std::vector<Game> games = data::LoadGames(6000, settings::kCEGTPath);
  //We want to count the number of drawn games in order to calculate the
  //minimal achievable error our engine can achieve on the data.
  double total_error = 0, decisive_error = 0, draw_error = 0;
  int draws = 0;
  int l_margin = games.size() / 10;
  int u_margin = l_margin * 2;
  search::set_print_info(false);
  for (int idx = 0; idx < games.size(); idx++) {
    int num_made_moves = games[idx].moves.size();
    games[idx].set_to_position_after(((l_margin+idx) * num_made_moves)
                                        / (games.size() + u_margin));
    double target = games[idx].result;
    if (target == 0.5) {
      draws++;
    }
    if (games[idx].board.get_turn() == kBlack) {
      target = 1 - target;
    }
    table::ClearTable();
    search::NodeSearch(games[idx].board, nodes_per_position);
    total_error += SigmoidCrossEntropyLoss(search::get_last_search_score(), target);
    if (target == 0.5) {
      draw_error +=
          SigmoidCrossEntropyLoss(search::get_last_search_score(), target) - kMinDrawLoss;
    }
    else {
      decisive_error += SigmoidCrossEntropyLoss(search::get_last_search_score(), target);
    }
    if ((idx + 1) % 100 == 0) {
      std::cout << (idx + 1) << std::endl
          << "Total Error: " << ((total_error - kMinLoss * (idx + 1 - draws)
              - kMinDrawLoss * draws) / (idx + 1)) << std::endl;
      if (idx + 1 > draws) {
        std::cout << "Decisive Error: " << ((decisive_error) / (idx + 1 - draws)) << std::endl;
      }
      if (draws > 0) {
        std::cout << "Draw Error: " << ((draw_error) / (draws)) << std::endl;
      }
    }
  }
  search::set_print_info(true);
  std::cout << "Final Avg Error: " << ((total_error - kMinLoss * (games.size() - draws)
      - kMinDrawLoss * draws) / games.size()) << std::endl;
  return (total_error - kMinLoss * (games.size() - draws)
                     - kMinDrawLoss * draws) / games.size();
}

}



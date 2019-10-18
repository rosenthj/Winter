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
 *      Author: Jonathan Rosenthal
 */

#include "board.h"
#include "data.h"
#include "search.h"
#include "transposition.h"
#include "net_evaluation.h"
#include "general/parse.h"
#include "general/settings.h"
#include "general/types.h"
#include "benchmark.h"
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <iostream>
#include <cmath>

namespace {

double SigmoidCrossEntropyLossV2(double score, double target) {
  return std::max(score, 0.0) - score * target + std::log(1 + std::exp(-std::abs(score)));
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
  double prediction = score_to_wpct(score);
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

using EvaluationTest = std::tuple<Board, double>;

}

namespace benchmark {

double MoveOrderTest() {
  std::cout << "Running move order benchmark!" << std::endl;
  std::vector<Game> games = data::LoadGames(6000, settings::kCEGTPath);
  double count = 0, top_1 = 0, top_2 = 0, top_3 = 0, top_5 = 0, top_half = 0;
  double top_1r = 0, top_2r = 0, top_3r = 0, top_5r = 0, top_halfr = 0;
  search::clear_killers_and_counter_moves();

  for (size_t idx = 0; idx < games.size(); idx++) {
    Game game = games[idx];
    game.set_to_position_after(0);
    while (game.board.get_num_made_moves() < game.moves.size() - 1) {
      int move_num = game.board.get_num_made_moves();
      std::vector<Move> moves = search::GetSortedMovesML(game.board);
      if (moves.size() < 10) {
        game.forward();
        continue;
      }
      size_t i;
      for (i = 0; i < moves.size(); i++) {
        if (game.moves[move_num] == moves[i])
          break;
      }
      if (i == 0)
        top_1++;
      if (i <= 1)
        top_2++;
      if (i <= 2)
        top_3++;
      if (i <= 5)
        top_5++;
      if (i <= moves.size() / 2)
        top_half++;
      count++;

      top_1r += 1.0 / moves.size();
      top_2r += 2.0 / moves.size();
      top_3r += 3.0 / moves.size();
      top_5r += 5.0 / moves.size();
      top_halfr += (moves.size() / 2) / ((double)moves.size());

      game.forward();
    }
    //total_error += SigmoidCrossEntropyLoss(search::get_last_search_score(), target);

    if ((idx + 1) % 10000 == 0) {
      std::cout << "Stats after " << (idx+1) << " games:"
          << "\nTop 1: " << (top_1 / count) << "\nTop 2: " << (top_2 / count)
          << "\nTop 3: " << (top_3 / count) << "\nTop 5: " << (top_5 / count)
          << "\nTop Half: " << (top_half / count) << "\nCount: " << count << std::endl;
    }
  }

  std::cout << "\nStats after " << games.size() << " games:"
      << "\nTop 1: " << (top_1 / count) << "\nTop 2: " << (top_2 / count)
      << "\nTop 3: " << (top_3 / count) << "\nTop 5: " << (top_5 / count)
      << "\nTop Half: " << (top_half / count) << "\nCount: " << count << std::endl;

  return top_1 / count;
}

void SymmetrySuite() {
  std::vector<SymmetryTest> test_sets;
  std::string line;
  std::ifstream file("./tests/symmetry.test");
  while(std::getline(file, line)) {
    test_sets.emplace_back(line);
  }
  //Time start = now();
  size_t test_sets_passed = 0;
  for (SymmetryTest test_set : test_sets) {
    if (net_evaluation::ScoreBoard(test_set.board1)
        == net_evaluation::ScoreBoard(test_set.board2))
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
  size_t test_sets_passed = 0;
  for (size_t i = 0; i < test_sets.size(); i++) {
    PerftTestSet test_set = test_sets[i];
    bool passed = true;
    for (std::pair<Depth, size_t> depth_result : test_set.depth_results) {
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
  for (size_t t = 0; t < boards.size(); t++) {
    Time test_start = now();
    search::DepthSearch(boards[t], depths[t]);
    Time test_end = now();
    auto test_time = std::chrono::duration_cast<Milliseconds>(test_end-test_start);
    std::cout << "test " << t << " completed in " << test_time.count() << std::endl;
    search::clear_killers_and_counter_moves();
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
  size_t draws = 0;
  int l_margin = games.size() / 10;
  int u_margin = l_margin * 2;
  search::set_print_info(false);
  for (size_t idx = 0; idx < games.size(); idx++) {
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
  size_t draws = 0;
  int l_margin = games.size() / 10;
  int u_margin = l_margin * 2;
  search::set_print_info(false);
  for (size_t idx = 0; idx < games.size(); idx++) {
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

//void GenerateDatasetFromEPD() {
//  std::string line;
//  std::ifstream file("quiet-labeled.epd");
//  std::ofstream dfile("zurichess_epd_data.csv");
//  size_t samples = 0;
//  while(std::getline(file, line)) {
//    std::vector<std::string> tokens = parse::split(line, ' ');
//    std::string fen = tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + tokens[3];
//    Board board(fen);
//    double result = -1;
//    if (tokens[5].compare("\"1-0\";") == 0) {
//      result = 1;
//    }
//    else if (tokens[5].compare("\"0-1\";") == 0) {
//      result = 0;
//    }
//    else if (tokens[5].compare("\"1/2-1/2\";") == 0) {
//      result = 0.5;
//    }
//    else {
//      std::cout << "Error detected! Line:" << std::endl;
//      std::cout << line << std::endl;
//      file.close();
//      return;
//    }
//    if (board.get_turn() == kBlack) {
//      result = 1 - result;
//    }
//    std::vector<int> features = evaluation::ScoreBoard<std::vector<int> >(board);
//    Vec<double, settings::kNumClusters> component_probs = evaluation::BoardMixtureProbability(board);
//    if (samples == 0) {
//      dfile << "res";
//      for (size_t i = 0; i < component_probs.size(); i++) {
//        dfile << ", cp" << i;
//      }
//      for (size_t i = 0; i < features.size(); i++) {
//        dfile << ", fe" << i;
//      }
//      dfile << std::endl;
//    }
//    dfile << result;
//    for (size_t i = 0; i < component_probs.size(); i++) {
//      dfile << ", " << component_probs[i];
//    }
//    for (int feature : features) {
//      dfile << ", " << feature;
//    }
//    dfile << std::endl;
//    samples++;
//    if (samples % 10000 == 0) {
//      std::cout << "Processed " << samples << " samples!" << std::endl;
//    }
//  }
//  file.close();
//  dfile.flush();
//  dfile.close();
//}

double RunEvalTestSet(const std::vector<EvaluationTest> &test_set, double div) {
  double error_sum = 0;
  for (const EvaluationTest sample : test_set) {
    Score score = net_evaluation::ScoreBoard(std::get<0>(sample));
    double target = std::get<1>(sample);
    //error_sum += std::abs(Sigmoid(evaluation::ScoreBoard(std::get<0>(sample)) / div)-std::get<1>(sample));
    error_sum += SigmoidCrossEntropyLossV2(score / div, target);
  }
  return error_sum / test_set.size();
}

double ZuriChessDatasetLoss() {
  std::string line;
  std::ifstream file("quiet-labeled.epd");
  std::vector<EvaluationTest> eval_test_set;
  while(std::getline(file, line)) {
    std::vector<std::string> tokens = parse::split(line, ' ');
    std::string fen = tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + tokens[3];
    Board board(fen);
    if (board.InCheck()) {
      continue;
    }
    double result = -1;
    if (tokens[5].compare("\"1-0\";") == 0) {
      result = 1;
    }
    else if (tokens[5].compare("\"0-1\";") == 0) {
      result = 0;
    }
    else if (tokens[5].compare("\"1/2-1/2\";") == 0) {
      result = 0.5;
    }
    else {
      std::cout << "Error detected! Line:" << std::endl;
      std::cout << line << std::endl;
      file.close();
      return 0;
    }
    if (board.get_turn() == kBlack) {
      result = 1 - result;
    }
    eval_test_set.emplace_back(board, result);

    if (eval_test_set.size() % 10000 == 0) {
      std::cout << "Processed " << (eval_test_set.size()) << " samples!" << std::endl;
    }
  }
  file.close();
  std::cout << "Processed " << (eval_test_set.size()) << " samples!" << std::endl;
  for (double div = 0.2; div < 1; div += 0.05) {
    std::cout << "Error with div=" << div << ": " << RunEvalTestSet(eval_test_set, div) << std::endl;
  }
  for (double div = 1; div < 32; div *= 2) {
    std::cout << "Error with div=" << div << ": " << RunEvalTestSet(eval_test_set, div) << std::endl;
  }
  for (double div = 24; div <= 1524; div += 50) {
    std::cout << "Error with div=" << div << ": " << RunEvalTestSet(eval_test_set, div) << std::endl;
  }
  return 0;
}

}

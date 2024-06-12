/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2016 Jonas Kuratli, Jonathan Maurer, Jonathan Rosenthal
 *  Copyright (C) 2017-2021 Jonathan Rosenthal
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
#include "search_thread.h"
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
#include <chrono>

namespace {

double ResultAbsLoss(Score x, Score y) {
  if (y.is_draw()) {
    return 0.5 * (1.0 - x.get_draw_probability());
  }
  if (y.is_disadvantage()) {
    return 1.0 * x.get_win_probability() + 0.5 * x.get_draw_probability();
  }
  return 1.0 * x.get_loss_probability() + 0.5 * x.get_draw_probability();
}

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

using EvaluationTest = std::tuple<Board, WDLScore>;

std::vector<std::string> &splits(const std::string &s, char delimeter,
    std::vector<std::string> &elements) {
    std::stringstream string_stream(s);
    std::string item;
    while (std::getline(string_stream, item, delimeter)) {
        elements.push_back(item);
    }
    return elements;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    splits(s, delim, elems);
    return elems;
}

struct PerftTest {
  std::string fen;
  std::vector<Depth> depths;
  std::vector<size_t> results;
};

PerftTest string_to_perft_test(const std::string input) {
  std::vector<std::string> parts = split(input, ';');
  PerftTest output;
  output.fen = parts[0];
  for (size_t i = 1; i < parts.size(); ++i) {
    std::vector<std::string> tokens = split(parts[i], ' ');
    tokens[0].erase(0,1);
    output.depths.emplace_back(std::stoi(tokens[0]));
    std::string target = tokens[1];
    //target.erase(std::remove_if(target.begin(), target.end(), std::isspace), target.end());
    output.results.emplace_back(std::stol(target));
  }
  return output;
}


}

namespace benchmark {

void SymmetrySuite() {
  std::vector<SymmetryTest> test_sets;
  std::string line;
  std::ifstream file("./tests/symmetry.test");
  while(std::getline(file, line)) {
    test_sets.emplace_back(line);
  }
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

void PerftSuite(std::string filename) {
  std::vector<PerftTest> test_sets;
  std::string line; 
  std::ifstream file(filename);
  while(std::getline(file, line)) {
    test_sets.emplace_back(string_to_perft_test(line));
  }
  Time start = now();
  size_t test_sets_passed = 0;
  for (size_t i = 0; i < test_sets.size(); i++) {
    bool passed = true;
    for (size_t idx = 0; idx < test_sets[i].depths.size(); ++idx) {
      Board board(test_sets[i].fen);
      if (search::Perft(board, test_sets[i].depths[idx]) != test_sets[i].results[idx]) {
        std::cout << "\033[31mFailed set " << i << " on input ("
            << test_sets[i].depths[idx] << "," << test_sets[i].results[idx] << ")\033[0m"<< std::endl;
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

double RunEvalTestSet(const std::vector<EvaluationTest> &test_set) {
  double error_sum = 0;
  for (const EvaluationTest &sample : test_set) {
    Score score = net_evaluation::ScoreBoard(std::get<0>(sample));
    Score target = std::get<1>(sample);
    error_sum += ResultAbsLoss(score, target);
  }
  return error_sum / test_set.size();
}

std::array<std::string, 50> kBenchmarkCommandPositions {
  "r3k2r/2pb1ppp/2pp1q2/p7/1nP1B3/1P2P3/P2N1PPP/R2QK2R w KQkq a6 0 14",
  "4rrk1/2p1b1p1/p1p3q1/4p3/2P2n1p/1P1NR2P/PB3PP1/3R1QK1 b - - 2 24",
  "r3qbrk/6p1/2b2pPp/p3pP1Q/PpPpP2P/3P1B2/2PB3K/R5R1 w - - 16 42",
  "6k1/1R3p2/6p1/2Bp3p/3P2q1/P7/1P2rQ1K/5R2 b - - 4 44",
  "8/8/1p2k1p1/3p3p/1p1P1P1P/1P2PK2/8/8 w - - 3 54",
  "7r/2p3k1/1p1p1qp1/1P1Bp3/p1P2r1P/P7/4R3/Q4RK1 w - - 0 36",
  "r1bq1rk1/pp2b1pp/n1pp1n2/3P1p2/2P1p3/2N1P2N/PP2BPPP/R1BQ1RK1 b - - 2 10",
  "3r3k/2r4p/1p1b3q/p4P2/P2Pp3/1B2P3/3BQ1RP/6K1 w - - 3 87",
  "2r4r/1p4k1/1Pnp4/3Qb1pq/8/4BpPp/5P2/2RR1BK1 w - - 0 42",
  "4q1bk/6b1/7p/p1p4p/PNPpP2P/KN4P1/3Q4/4R3 b - - 0 37",
  "2q3r1/1r2pk2/pp3pp1/2pP3p/P1Pb1BbP/1P4Q1/R3NPP1/4R1K1 w - - 2 34",
  "1r2r2k/1b4q1/pp5p/2pPp1p1/P3Pn2/1P1B1Q1P/2R3P1/4BR1K b - - 1 37",
  "r3kbbr/pp1n1p1P/3ppnp1/q5N1/1P1pP3/P1N1B3/2P1QP2/R3KB1R b KQkq b3 0 17",
  "8/6pk/2b1Rp2/3r4/1R1B2PP/P5K1/8/2r5 b - - 16 42",
  "1r4k1/4ppb1/2n1b1qp/pB4p1/1n1BP1P1/7P/2PNQPK1/3RN3 w - - 8 29",
  "8/p2B4/PkP5/4p1pK/4Pb1p/5P2/8/8 w - - 29 68",
  "3r4/ppq1ppkp/4bnp1/2pN4/2P1P3/1P4P1/PQ3PBP/R4K2 b - - 2 20",
  "5rr1/4n2k/4q2P/P1P2n2/3B1p2/4pP2/2N1P3/1RR1K2Q w - - 1 49",
  "1r5k/2pq2p1/3p3p/p1pP4/4QP2/PP1R3P/6PK/8 w - - 1 51",
  "q5k1/5ppp/1r3bn1/1B6/P1N2P2/BQ2P1P1/5K1P/8 b - - 2 34",
  "r1b2k1r/5n2/p4q2/1ppn1Pp1/3pp1p1/NP2P3/P1PPBK2/1RQN2R1 w - - 0 22",
  "r1bqk2r/pppp1ppp/5n2/4b3/4P3/P1N5/1PP2PPP/R1BQKB1R w KQkq - 0 5",
  "r1bqr1k1/pp1p1ppp/2p5/8/3N1Q2/P2BB3/1PP2PPP/R3K2n b Q - 1 12",
  "r1bq2k1/p4r1p/1pp2pp1/3p4/1P1B3Q/P2B1N2/2P3PP/4R1K1 b - - 2 19",
  "r4qk1/6r1/1p4p1/2ppBbN1/1p5Q/P7/2P3PP/5RK1 w - - 2 25",
  "r7/6k1/1p6/2pp1p2/7Q/8/p1P2K1P/8 w - - 0 32",
  "r3k2r/ppp1pp1p/2nqb1pn/3p4/4P3/2PP4/PP1NBPPP/R2QK1NR w KQkq - 1 5",
  "3r1rk1/1pp1pn1p/p1n1q1p1/3p4/Q3P3/2P5/PP1NBPPP/4RRK1 w - - 0 12",
  "5rk1/1pp1pn1p/p3Brp1/8/1n6/5N2/PP3PPP/2R2RK1 w - - 2 20",
  "8/1p2pk1p/p1p1r1p1/3n4/8/5R2/PP3PPP/4R1K1 b - - 3 27",
  "8/4pk2/1p1r2p1/p1p4p/Pn5P/3R4/1P3PP1/4RK2 w - - 1 33",
  "8/5k2/1pnrp1p1/p1p4p/P6P/4R1PK/1P3P2/4R3 b - - 1 38",
  "8/8/1p1kp1p1/p1pr1n1p/P6P/1R4P1/1P3PK1/1R6 b - - 15 45",
  "8/8/1p1k2p1/p1prp2p/P2n3P/6P1/1P1R1PK1/4R3 b - - 5 49",
  "8/8/1p4p1/p1p2k1p/P2npP1P/4K1P1/1P6/3R4 w - - 6 54",
  "8/8/1p4p1/p1p2k1p/P2n1P1P/4K1P1/1P6/6R1 b - - 6 59",
  "8/5k2/1p4p1/p1pK3p/P2n1P1P/6P1/1P6/4R3 b - - 14 63",
  "8/1R6/1p1K1kp1/p6p/P1p2P1P/6P1/1Pn5/8 w - - 0 67",
  "1rb1rn1k/p3q1bp/2p3p1/2p1p3/2P1P2N/PP1RQNP1/1B3P2/4R1K1 b - - 4 23",
  "4rrk1/pp1n1pp1/q5p1/P1pP4/2n3P1/7P/1P3PB1/R1BQ1RK1 w - - 3 22",
  "r2qr1k1/pb1nbppp/1pn1p3/2ppP3/3P4/2PB1NN1/PP3PPP/R1BQR1K1 w - - 4 12",
  "2r2k2/8/4P1R1/1p6/8/P4K1N/7b/2B5 b - - 0 55",
  "6k1/5pp1/8/2bKP2P/2P5/p4PNb/B7/8 b - - 1 44",
  "2rqr1k1/1p3p1p/p2p2p1/P1nPb3/2B1P3/5P2/1PQ2NPP/R1R4K w - - 3 25",
  "r1b2rk1/p1q1ppbp/6p1/2Q5/8/4BP2/PPP3PP/2KR1B1R b - - 2 14",
  "6r1/5k2/p1b1r2p/1pB1p1p1/1Pp3PP/2P1R1K1/2P2P2/3R4 w - - 1 36",
  "rnbqkb1r/pppppppp/5n2/8/2PP4/8/PP2PPPP/RNBQKBNR b KQkq c3 0 2",
  "2rr2k1/1p4bp/p1q1p1p1/4Pp1n/2PB4/1PN3P1/P3Q2P/2RR2K1 w - f6 0 20",
  "3br1k1/p1pn3p/1p3n2/5pNq/2P1p3/1PN3PP/P2Q1PB1/4R1K1 w - - 0 23",
  "2r2b2/5p2/5k2/p1r1pP2/P2pB3/1P3P2/K1P3R1/7R w - - 23 93"
};

// This function and the list of FENs above is heavily based on code found in Ethereal by Adrew Grant
void RunBenchCommand(int argc, char **argv) {
  Board board;

  std::array<Score, 256> scores;
  std::array<Milliseconds, 256> times;

  std::array<int64_t, 256> nodes;
  std::array<Move, 256> bestMoves;

  int depth     = argc > 2 ? atoi(argv[2]) : 9;
  int num_threads  = argc > 3 ? atoi(argv[3]) :  1;
  int megabytes = argc > 4 ? atoi(argv[4]) : 16;

  table::SetTableSize(megabytes);
  Time initial_time = now();
  search::Threads.set_num_threads(num_threads);

  for (int i = 0; i < kBenchmarkCommandPositions.size(); i++) {

    // Perform the search on the position
    Time start = now();
    board.SetBoard(split(kBenchmarkCommandPositions[i],' '));
    bestMoves[i] = search::DepthSearch(board, depth);

    // Stat collection for later printing
    scores[i] = search::get_last_search_score();
    times[i] = std::chrono::duration_cast<Milliseconds>(now() - start);
    nodes[i] = search::get_num_nodes();

    table::ClearTable(); // Reset TT between searches
    search::clear_killers_and_counter_moves();
  }

  printf("\n================================================================\n");

  for (int i = 0; i < kBenchmarkCommandPositions.size(); i++) {

    // Convert moves to typical UCI notation
    std::string bestStr = parse::MoveToString(bestMoves[i]);

    // Log all collected information for the current position
    printf("Bench [# %2d] %5d cp  Best:%6s %12d nodes %8d nps\n", i + 1, (scores[i].to_cp() / 8),
           bestStr.c_str(), (int)nodes[i], (int)(1000 * nodes[i] / (times[i].count() + 1)));
  }

  printf("==================================================================\n");

  // Report the overall statistics
  auto total_time = std::chrono::duration_cast<Milliseconds>(now() - initial_time);
  int64_t totalNodes = 0;
  for (int i = 0; i < kBenchmarkCommandPositions.size(); i++) totalNodes += nodes[i];
  printf("OVERALL: %38d nodes %8d nps\n", (int)(totalNodes - (totalNodes % 100)), (int)(1000 * totalNodes / (total_time.count() + 1)));
}

}

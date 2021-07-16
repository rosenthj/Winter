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
 * uci.cc
 *
 *  Created on: Nov 4, 2016
 *      Author: Jonathan Rosenthal
 */

#include "benchmark.h"
#include "uci.h"
#include "general/settings.h"
#include "general/types.h"
#include "board.h"
#include "net_evaluation.h"
#include "search.h"
#include "transposition.h"
#include "search_thread.h"
#include <array>
#include <cstdint>
#include <vector>
#include <sstream>
#include <iostream>
#include <thread>

namespace {

std::vector<std::string> &splits(const std::string &s, char delimeter,
  std::vector<std::string> &elements) {
  std::stringstream string_stream(s);
  std::string item;
  while (std::getline(string_stream, item, delimeter)) {
    elements.push_back(item);
  }
  return elements;
}

// Returns a maximum time with margin of error for param clock time
int32_t  maxtime(int32_t time) {
  return std::max((8 * time) / 10, time - 100);
}


int32_t  get_base_time(int32_t  base, int32_t  inc, int32_t  to_go=22) {
  return std::min(maxtime(base),((58 * base) / std::min(50, to_go) + 58 * inc) / 50);
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  splits(s, delim, elems);
  return elems;
}

bool Equals(std::string string_a, std::string string_b) {
  return string_a.compare(string_b) == 0;
}

bool IsTrue(std::string s) {
  return Equals(s, "true") || Equals(s, "True") || Equals(s, "TRUE") || Equals(s, "1");
}

struct UCIOption {
	std::string name;
	void (*func)(int32_t value);
	int32_t default_value;
	int32_t lower_bound;
	int32_t upper_bound;
  std::string to_string() const {
    return "option name " + name
              + " type spin default " + std::to_string(default_value)
              + " min " + std::to_string(lower_bound)
              + " max " + std::to_string(upper_bound);
  }
};

struct UCICheck {
  std::string name;
  void (*func)(bool value);
  bool default_value;
  std::string to_string() const {
    if (default_value)
      return "option name " + name + " type check default true";
    return "option name " + name + " type check default false";
  }
};

const std::vector<UCIOption> uci_options {
  {"Hash", table::SetTableSize, 32, 1, 104576},
  {"Threads", search::SetNumThreads, 1, 1, 256},
  {"Contempt", search::SetContempt, 0, -100, 100},
#ifdef TUNE
  {"AspirationDelta", search::SetInitialAspirationDelta, 40, 10, 800},
  {"Futility", search::SetFutilityMargin, 560, 400, 1500},
  {"SNMPMargin", search::SetSNMPMargin, 790, 500, 1500},
  {"LMROffset", search::SetLMROffset, 16, 0, 100},
  {"LMRMultiplier", search::SetLMRMultiplier, 74, 0, 250},
  {"LMROffsetCap", search::SetLMROffsetCap, 33, -55, 100},
  {"LMRMultiplierCap", search::SetLMRMultiplierCap, 30, 0, 100},
  {"LMROffsetPV", search::SetLMROffsetPV, 72, -55, 100},
  {"LMRMultiplierPV", search::SetLMRMultiplierPV, 76, 0, 100},
  {"LMROffsetPVCap", search::SetLMROffsetPVCap, 50, -155, 100},
//  {"LMRMultiplierPVCap", search::SetLMRMultiplierPVCap, 23, 0, 100},
  {"LMPBaseNW", search::SetLMPBaseNW, 3, 1, 20},
  {"LMPBasePV", search::SetLMPBasePV, 5, 1, 20},
  {"LMPScalar", search::SetLMPScalar, 12, 0, 24},
  {"LMPQuadratic", search::SetLMPQuadratic, 4, 0, 16},
#endif
};

const std::vector<UCICheck> uci_check_options {
  {"Armageddon", search::SetArmageddon, false},
  {"UCI_ShowWDL", search::SetUCIShowWDL, true},
  {"UCI_Chess960", settings::set_chess960_mode, false},
};

const std::string kEngineIsReady = "readyok";
const std::string kEngineNamePrefix = "id name ";
const std::string kEngineAuthorPrefix = "id author ";
const std::string kOk = "uciok";

struct Timer {
  std::array<int, 2> time;
  std::array<int, 2> inc;
  int movetime;
  size_t nodes;
  Depth moves_to_go;
  Depth search_depth;
};

void Go(Board *board, Timer timer) {
  Move move = 0;
  if (timer.search_depth != 0) {
    move = search::DepthSearch((*board), timer.search_depth);
  }
  else if (timer.movetime != 0) {
    Milliseconds duration = Milliseconds(maxtime(timer.movetime));
    move = search::FixedTimeSearch((*board), duration);
  }
  else if (timer.nodes != 0) {
    move = search::NodeSearch((*board), timer.nodes);
  }
  else {
    int time;
    const Color color = board->get_turn();
    if (timer.moves_to_go > 0) {
      time = get_base_time(timer.time[color], timer.inc[color], timer.moves_to_go);
    }
    else {
      time = get_base_time(timer.time[color], timer.inc[color]);
    }
    Milliseconds duration = Milliseconds(time);
    move = search::TimeSearch((*board), duration);
  }
  std::cout << "bestmove " << parse::MoveToString(move) << std::endl;
  search::Threads.is_searching = false;
}

void Reply(std::string message) {
  std::cout << message << std::endl;
}

struct UCICommand {
  std::string name;
  void (*func)(Board &board, const std::vector<std::string> args);
};

using StrArgs = std::vector<std::string>;

bool quit_flag = false;

void UCIQuit(Board &board, const StrArgs args) {
  search::end_search();
  while (search::Threads.is_searching) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  quit_flag = true;
}

void UCIIsReady(Board &board, const StrArgs) {
  Reply(kEngineIsReady);
}

void UCIGo(Board &board, const StrArgs tokens) {
  int index = 1;
  search::end_search();
  Timer timer {};
  if (tokens.size() >= index+2) {
    while (tokens.size() >= index+2) {
      std::string arg = tokens[index++];
      if(Equals(arg, "depth")){
        timer.search_depth = atoi(tokens[index++].c_str());
      }
      else if(Equals(arg, "movetime")){
        timer.movetime = atoi(tokens[index++].c_str());
      }
      else if(Equals(arg, "wtime")){
        timer.time[kWhite] = atoi(tokens[index++].c_str());
      }
      else if(Equals(arg, "btime")){
        timer.time[kBlack] = atoi(tokens[index++].c_str());
      }
      else if(Equals(arg, "winc")){
        timer.inc[kWhite] = atoi(tokens[index++].c_str());
      }
      else if(Equals(arg, "binc")){
        timer.inc[kBlack] = atoi(tokens[index++].c_str());
      }
      else if(Equals(arg, "movestogo")){
        timer.moves_to_go = atoi(tokens[index++].c_str());
      }
      else if(Equals(arg, "nodes")){
        timer.nodes = atol(tokens[index++].c_str());
      }
    }
  }
  else if (tokens.size() == index+1 && Equals(tokens[index], "infinite")) {
    std::string arg = tokens[index++];
    timer.movetime = 24 * 60 * 60 * 1000;
  }
  else{
    timer.search_depth = 6;
  }
  std::thread t(Go, &board, timer);
  t.detach();
}

void UCINewGame(Board &board, const StrArgs) {
  board = Board();
}

void UCISetOption(Board &board, const StrArgs tokens) {
  int index = 2;
  std::string command = tokens[index++];
  for (const UCIOption &option : uci_options) {
    if (Equals(command, option.name)) {
      index++;
      int value = atoi(tokens[index++].c_str());
      option.func(value);
      break;
    }
  }
  for (const UCICheck &option : uci_check_options) {
    if (Equals(command, option.name)) {
      index++;
      bool value = IsTrue(tokens[index++].c_str());
      option.func(value);
      break;
    }
  }
}

void UCIPosition(Board &board, const StrArgs tokens) {
  int index = 1;
  if (index < tokens.size()) {
    std::string arg = tokens[index++];
    if (Equals(arg, "startpos")) {
      board.SetStartBoard();
      if (index < tokens.size()){
        arg = tokens[index++];
      }
    }
    else if (Equals(arg, "fen")) {
      std::vector<std::string> fen_tokens;
      while (index < tokens.size()) {
        arg = tokens[index++];
        if (Equals(arg, "moves")) {
          break;
        }
        fen_tokens.emplace_back(arg);
      }
      board.SetBoard(fen_tokens);
    }
    if (Equals(arg, "moves")) {
      while (index < tokens.size()) {
        Move move = parse::StringToMove(tokens[index++]);
        std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
        for (unsigned int i = 0; i < moves.size(); i++) {
          if (GetMoveSource(moves[i]) == GetMoveSource(move)
              && GetMoveDestination(moves[i]) == GetMoveDestination(move)
              && (GetMoveType(moves[i]) < kKnightPromotion
                  || GetMoveType(moves[i]) == GetMoveType(move))) {
            board.Make(moves[i]);
            break;
          }
        }
      }
    }
  }
}

void UCIStop(Board &board , const StrArgs) {
  search::end_search();
}

void UCIUci(Board &board, const StrArgs) {
  Reply(kEngineNamePrefix + settings::engine_name + " "
        + settings::engine_version + " " + settings::compile_arch);
  Reply(kEngineAuthorPrefix + settings::engine_author);
  for (const UCIOption &option : uci_options) {
    Reply(option.to_string());
  }
  for (const UCICheck &option : uci_check_options) {
    Reply(option.to_string());
  }
  Reply(kOk);
}

void PrintBoard(Board &board, const StrArgs) {
  board.Print();
}

void PrintMoves(Board &board, const StrArgs) {
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  for (unsigned int i = 0; i < moves.size(); i++) {
    std::cout << parse::MoveToString(moves[i]) << std::endl;
  }
}

void PrintMovesSorted(Board &board, const StrArgs) {
  std::vector<Move> moves = search::GetSortedMovesML(board);
  for (unsigned int i = 0; i < moves.size(); i++) {
    std::cout << parse::MoveToString(moves[i]) << std::endl;
  }
}

void CheckIfDraw(Board &board, const StrArgs) {
  std::cout << board.IsDraw() << std::endl;
}

void PerftTest(Board &board, const StrArgs) {
  benchmark::PerftSuite();
}

void SymmetryTest(Board &board, const StrArgs) {
  benchmark::SymmetrySuite();
}

void Benchmark(Board &board, const StrArgs tokens) {
  int ms = atoi(tokens[1].c_str());
  benchmark::EntropyLossTimedSuite(Milliseconds(ms));
}

void BenchmarkMoveOrder(Board &board, const StrArgs) {
  benchmark::MoveOrderTest();
}

void BenchmarkNode(Board &board, const StrArgs tokens) {
  long n = atol(tokens[1].c_str());
  benchmark::EntropyLossNodeSuite(n);
}

void SEE(Board &board, const StrArgs tokens) {
  Move move = parse::StringToMove(tokens[1]);
  std::cout << board.NonNegativeSEE(move) << std::endl;
}

void Perft(Board &board, const StrArgs tokens) {
  int index = 1;
  Depth depth = atoi(tokens[index++].c_str());
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  uint64_t sum = 0;
  Time begin = now();
  for (Move move : moves) {
    board.Make(move);
    long perft_result = search::Perft(board, depth-1);
    board.UnMake();
    std::cout << parse::MoveToString(move) << " depth: " << (depth-1)
        << " perft: " << perft_result << std::endl;
    sum += perft_result;
  }
  std::cout << "Ended perft" << std::endl;
  Time end = now();
  auto time_used = std::chrono::duration_cast<Milliseconds>(end-begin);
  std::cout << "depth: " << depth << " perft: " << sum << " time: " << time_used.count()
      << " nps: " << ((sum * 1000) / (time_used.count() + 1)) << std::endl;
}

void GetFEN(Board &board, const StrArgs) {
  std::vector<std::string> fen = board.GetFen();
  for (std::string fen_token : fen) {
    std::cout << fen_token << " ";
  }
  std::cout << std::endl;
}

void CheckIfRepetitionPossible(Board &board, const StrArgs) {
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  if (board.MoveInListCanRepeat(moves)) {
    std::cout << "yes" << std::endl;
  }
  else {
    std::cout << "no" << std::endl;
  }
}

void GenEvalCSV(Board &board, const StrArgs) {
#ifdef EVAL_TRAINING
  if (tokens.size() < 2 || tokens.size() > 3) {
    std::cout << "invalid number of arguments, expected 1 or 2 got " << (tokens.size()-1) << std::endl;
  }
  if (tokens.size() == 2) {
    net_evaluation::GenerateDatasetFromUCIGames(tokens[index++]);
  }
  if (tokens.size() == 3) {
    std::string filename = tokens[index++];
    std::string out = tokens[index++];
    net_evaluation::GenerateDatasetFromUCIGames(filename, out);
  }
#else
  std::cout << "Command not supported in this build. Recompile with -DEVAL_TRAINING" << std::endl;
#endif
}

void PrintBitboards(Board &board, const StrArgs) {
  board.PrintBitBoards();
}

void EvaluateBoard(Board &board, const StrArgs) {
  std::cout << net_evaluation::ScoreBoard(board).to_nscore() << std::endl;
}

const std::vector<UCICommand> uci_commands {
  // UCI Commands
  {"go", UCIGo},
  {"quit", UCIQuit},
  {"isready", UCIIsReady},
  {"position", UCIPosition},
  {"setoption", UCISetOption},
  {"stop", UCIStop},
  {"uci", UCIUci},
  {"ucinewgame", UCINewGame},
  // Non-Standard Commands
  {"benchmark", Benchmark},
  {"benchmark_move_order", BenchmarkMoveOrder},
  {"benchmark_node", BenchmarkNode},
  {"can_repeat", CheckIfRepetitionPossible},
  {"evaluate", EvaluateBoard},
  {"fen", GetFEN},
  {"gen_eval_csv", GenEvalCSV},
  {"perft", Perft},
  {"perft_test", PerftTest}, 
  {"print", PrintBoard},
  {"print_bitboards", PrintBitboards},
  {"print_moves", PrintMoves},
  {"print_moves_sorted", PrintMovesSorted},
  {"see", SEE},
  {"symmetry_test", SymmetryTest},
  {"isdraw", CheckIfDraw},
};

}

namespace uci {

void Loop() {
  Board board;
  std::string in;
  while (!quit_flag && std::getline(std::cin, in)) {
    std::vector<std::string> args = split(in, ' ');
    std::string command = args[0];
    bool command_found = false;
    for (UCICommand uci_command : uci_commands) {
      if (Equals(command, uci_command.name)) {
        command_found = true;
        uci_command.func(board, args);
        break;
      }
    }
    if (!command_found) {
      Reply("Received unknown command: " + command);
    }
  }
}

}

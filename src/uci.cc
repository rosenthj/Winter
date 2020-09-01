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

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  splits(s, delim, elems);
  return elems;
}

//bool IsTrue(std::string s) {
//  return Equals(s, "true") || Equals(s, "True") || Equals(s, "TRUE") || Equals(s, "1");
//}

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

std::vector<UCIOption> uci_options {
  {"Hash", table::SetTableSize, 32, 1, 104576},
  {"Threads", search::SetNumThreads, 1, 1, 256},
// "\noption name Contempt type spin default 0 min -100 max 100"
//   search::SetContempt(contempt);
// "\noption name Armageddon type check default false"
//   bool armageddon_setting = IsTrue(tokens[index++]);
//   search::SetArmageddon(armageddon_setting);
#ifdef TUNE
  {"AspirationDelta", search::SetInitialAspirationDelta, 40, 10, 800},
  {"Futility", search::SetFutilityMargin, 900, 400, 1500},
  {"SNMPMargin", search::SetSNMPMargin, 700, 0, 2000},
  {"LMROffset", search::SetLMROffset, 73, 0, 100},
  {"LMRMultiplier", search::SetLMRMultiplier, 74, 0, 250},
  {"LMROffsetCap", search::SetLMROffsetCap, -13, -55, 100},
  {"LMRMultiplierCap", search::SetLMRMultiplierCap, 30, 0, 100},
  {"LMROffsetPV", search::SetLMROffsetPV, 56, -55, 100},
  {"LMRMultiplierPV", search::SetLMRMultiplierPV, 76, 0, 100},
  {"LMROffsetPVCap", search::SetLMROffsetPVCap, -50, -155, 100},
//  {"LMRMultiplierPVCap", search::SetLMRMultiplierPVCap, 23, 0, 100},
#endif
};

const std::string kEngineIsReady = "readyok";
const std::string kEngineNamePrefix = "id name ";
const std::string kEngineAuthorPrefix = "id author ";
const std::string kOk = "uciok";

struct Timer {
  Timer() {
    for (Color color = kWhite; color <= kBlack; color++) {
      time[color] = 0;
      inc[color] = 0;
    }
    movetime = 0;
    moves_to_go = 0;
    search_depth = 0;
    nodes = 0;
  }
  int time[2], inc[2], movetime;
  size_t nodes;
  Depth moves_to_go, search_depth;
};

int maxtime(int time) {
  return std::max((8 * time) / 10, time - 100);
}

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
    if (timer.moves_to_go == 0) {
      timer.moves_to_go = 40;
    }
    Color color = board->get_turn();
    int time = timer.time[color] + ((timer.time[color] * board->get_phase()) / kMaxPhase);
    time = (6 * time) / 5;
    time /= timer.moves_to_go + 8;
    time = std::max(time, timer.time[color] / timer.moves_to_go + 4);
    time += timer.inc[color];
    Milliseconds duration = Milliseconds(maxtime(time));
    move = search::TimeSearch((*board), duration);
  }
  std::cout << "bestmove " << parse::MoveToString(move) << std::endl;
}

bool Equals(std::string string_a, std::string string_b) {
  return string_a.compare(string_b) == 0;
}

void Reply(std::string message) {
  std::cout << message << std::endl;
}

}

namespace uci {

void Loop() {
  Board board;
  std::string in;
  while (std::getline(std::cin, in)) {
    std::vector<std::string> tokens = split(in, ' ');
    unsigned int index = 0;
    std::string command = tokens[index++];
    if (Equals(command, "quit")) {
      //Resynchronise search threads:
      search::end_search();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      break;
    }
    else if (Equals(command, "gen_eval_csv")) {
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
    else if (Equals(command, "isready")) {
      Reply(kEngineIsReady);
    }
    else if (Equals(command, "print")) {
      board.Print();
    }
    else if (Equals(command, "print_bitboards")) {
      board.PrintBitBoards();
    }
    else if (Equals(command, "isdraw")) {
      std::cout << board.IsDraw() << std::endl;
    }
    else if (Equals(command, "evaluate")) {
      std::cout << net_evaluation::ScoreBoard(board).to_nscore() << std::endl;
    }
    else if (Equals(command, "uci")) {
      Reply(kEngineNamePrefix + settings::engine_name + " "
          + settings::engine_version + " " + settings::compile_arch);
      Reply(kEngineAuthorPrefix + settings::engine_author);
      for (const UCIOption &option : uci_options) {
        Reply(option.to_string());
      }
      Reply(kOk);
    }
    else if (Equals(command, "stop")) {
      search::end_search();
    }
    else if (Equals(command, "ucinewgame")) {
      board = Board();
    }
    else if (Equals(command, "setoption")) {
      index++;
      command = tokens[index++];
      for (UCIOption &option : uci_options) {
        if (Equals(command, option.name)) {
          index++;
          int value = atoi(tokens[index++].c_str());
          option.func(value);
          break;
        }
      }
    }
    else if (Equals(command, "print_moves")) {
      std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
      for (unsigned int i = 0; i < moves.size(); i++) {
        std::cout << parse::MoveToString(moves[i]) << std::endl;
      }
    }
    else if (Equals(command, "print_moves_sorted")) {
      std::vector<Move> moves = search::GetSortedMovesML(board);
      for (unsigned int i = 0; i < moves.size(); i++) {
        std::cout << parse::MoveToString(moves[i]) << std::endl;
      }
    }
    else if (Equals(command, "position")) {
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
    else if (Equals(command, "go")) {
      search::end_search();
      Timer timer;
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
    else if (Equals(command, "see")) {
      Move move = parse::StringToMove(tokens[index]);
      std::cout << board.NonNegativeSEE(move) << std::endl;
    }
    else if (Equals(command, "perft")) {
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
    else if (Equals(command, "perft_test")) {
      benchmark::PerftSuite();
    }
    else if (Equals(command, "symmetry_test")) {
      benchmark::SymmetrySuite();
    }
    else if (Equals(command, "benchmark")) {
      int ms = atoi(tokens[index++].c_str());
      benchmark::EntropyLossTimedSuite(Milliseconds(ms));
    }
    else if (Equals(command, "benchmark_node")) {
      long n = atol(tokens[index++].c_str());
      benchmark::EntropyLossNodeSuite(n);
    }
    else if (Equals(command, "benchmark_move_order")) {
      benchmark::MoveOrderTest();
    }
    else if (Equals(command, "fen")) {
      std::vector<std::string> fen = board.GetFen();
      for (std::string fen_token : fen) {
        std::cout << fen_token << " ";
      }
      std::cout << std::endl;
    }
    else if (Equals(command, "can_repeat")) {
      std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
      if (board.MoveInListCanRepeat(moves)) {
        std::cout << "yes" << std::endl;
      }
      else {
        std::cout << "no" << std::endl;
      }
    }
    else {
      Reply("Received unknown command: " + command);
    }
  }
}

}

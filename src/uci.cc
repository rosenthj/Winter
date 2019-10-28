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
#include "general/bookkeeping.h"
#include "board.h"
#include "net_evaluation.h"
#include "search.h"
#include "transposition.h"
#include "general/debug.h"
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

const std::string kEngineIsReady = "readyok";
const std::string kEngineNamePrefix = "id name ";
const std::string kEngineAuthorPrefix = "id author ";
const std::string kOk = "uciok";
const std::string kUCIHashOptionString =
    "option name Hash type spin default 32 min 1 max 104576"
    "\noption name Threads type spin default 1 min 1 max 256"
    "\noption name Contempt type spin default 0 min -100 max 100"
#ifndef TUNE
    "\noption name Armageddon type check default false";
#else
    "\noption name Armageddon type check default false"
    "\noption name AspirationDelta type spin default 120 min 10 max 800"
    "\noption name Futility type spin default 1274 min 400 max 1500"
    "\noption name SNMPMargin type spin default 588 min 0 max 2000"
    "\noption name LMRDivisor type spin default 134 min 60 max 250";
#endif

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

bool IsTrue(std::string s) {
  return Equals(s, "true") || Equals(s, "True") || Equals(s, "TRUE") || Equals(s, "1");
}


}

namespace uci {

void Loop() {
  debug::EnterFunction(debug::kUci, "uci::Loop", "");
  Board board;
  while (true) {
    std::string in;
    std::getline(std::cin, in);
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
    }
    else if (Equals(command, "isready")) {
      Reply(kEngineIsReady);
    }
    else if (Equals(command, "print")) {
      board.Print();
    }
//    else if (Equals(command, "train_csv")) {
//      if (tokens.size() != 2) {
//        std::cout << "invalid number of arguments, expected 1 got " << (tokens.size()-1) << std::endl;
//      }
//      train::TrainCSV(tokens[index++]);
//    }
    else if (Equals(command, "print_bitboards")) {
      board.PrintBitBoards();
    }
//    else if (Equals(command, "print_features")) {
//      TODO replace with function from net_eval
//      evaluation::PrintFeatureValues(board);
//    }
    else if (Equals(command, "isdraw")) {
      std::cout << board.IsDraw() << std::endl;
    }
    else if (Equals(command, "evaluate")) {
      std::cout << net_evaluation::ScoreBoard(board) << std::endl;
    }
    else if (Equals(command, "uci")) {
      Reply(kEngineNamePrefix + settings::engine_name + " "
          + settings::engine_version + " " + settings::compile_arch);
      Reply(kEngineAuthorPrefix + settings::engine_author);
      Reply(kUCIHashOptionString);
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
      if (Equals(command, "Hash")) {
        index++;
        int MB = atoi(tokens[index++].c_str());
        table::SetTableSize(MB);
      }
      if (Equals(command, "Threads")) {
        index++;
        int num_threads = atoi(tokens[index++].c_str());
        search::Threads.set_num_threads(num_threads);
      }
      if (Equals(command, "Contempt")) {
        index++;
        int contempt = atoi(tokens[index++].c_str());
        search::SetContempt(contempt);
      }
      if (Equals(command, "Armageddon")) {
        index++;
        bool armageddon_setting = IsTrue(tokens[index++]);
        search::SetArmageddon(armageddon_setting);
      }
#ifdef TUNE
      if (Equals(command, "AspirationDelta")) {
        index++;
        Score delta = atoi(tokens[index++].c_str());
        search::SetInitialAspirationDelta(delta);
      }
      if (Equals(command, "Futility")) {
        index++;
        int futility = atoi(tokens[index++].c_str());
        search::SetFutilityMargin(futility);
      }
      if (Equals(command, "SNMPMargin")) {
        index++;
        int margin = atoi(tokens[index++].c_str());
        search::SetSNMPMargin(margin);
      }
      if (Equals(command, "LMRDivisor")) {
        index++;
        int div = atoi(tokens[index++].c_str());
        search::SetLMRDiv(div * 0.01);
      }
#endif
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
      HashType hash = board.get_hash();
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
      if (board.get_hash() != hash) {
        debug::Error("Hash after perft function is changed!", false);
      }
    }
//    else if (Equals(command, "estimate_variable_influence")) {
//      evaluation::CheckVariableInfluence();
//    }
//    else if (Equals(command, "train_params")) {
//      train::Train(false);
//    }
    else if (Equals(command, "bookkeeping_reset")) {
      bookkeeping::reset_counters();
    }
    else if (Equals(command, "bookkeeping_absolute")) {
      bookkeeping::print_counters();
    }
    else if (Equals(command, "bookkeeping_relative")) {
      bookkeeping::print_relative_counters();
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
  debug::ExitFunction(debug::kUci);
}

}

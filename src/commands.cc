#include "benchmark.h"
#include "board.h"
#include "commands.h"
#include "net_evaluation.h"
#include "search.h"

#include <vector>
#include <string>

namespace commands {

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

void PrintBitboards(Board &board, const StrArgs) {
  board.PrintBitBoards();
}

void EvaluateBoard(Board &board, const StrArgs) {
  std::cout << net_evaluation::ScoreBoard(board).to_nscore() << std::endl;
}

}

#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

/**
 * This header file in conjunction with the source file and namespace
 * exist in order to handle commands which are not standard to the
 * UCI protocol.
 */

#include <string>
#include <vector>
#include "board.h"

using StrArgs = std::vector<std::string>;

namespace commands {

void PrintBoard(Board &board, const StrArgs);
void PrintMoves(Board &board, const StrArgs);
void PrintMovesSorted(Board &board, const StrArgs);
void CheckIfDraw(Board &board, const StrArgs);
void PerftTest(Board &board, const StrArgs);
void SymmetryTest(Board &board, const StrArgs);
void Benchmark(Board &board, const StrArgs tokens);
void BenchmarkMoveOrder(Board &board, const StrArgs);
void BenchmarkNode(Board &board, const StrArgs tokens);
void SEE(Board &board, const StrArgs tokens);
void Perft(Board &board, const StrArgs tokens);
void GetFEN(Board &board, const StrArgs);
void CheckIfRepetitionPossible(Board &board, const StrArgs);
void PrintBitboards(Board &board, const StrArgs);
void EvaluateBoard(Board &board, const StrArgs);

}

#endif // COMMANDS_H_INCLUDED

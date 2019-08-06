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
 * board.h
 *
 *  Created on: Nov 1, 2016
 *      Author: Jonathan Rosenthal
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "general/types.h"
#include "general/parse.h"
#include "general/bit_operations.h"
#include "learning/linear_algebra.h"
#include <vector>
#include <iostream>

/**
 * This type is used to store information which is necessary in order to
 * implement the unmake move function such as ep.
 */
typedef int32_t MoveHistoryInformation;

class Board{
public:
  //Board constructor initializes the board to the starting position.
  Board();
  Board(std::string fen);
  //Sets the board to position defined by the argument FEN code.
  //Previous state information is reset.
  void SetBoard(std::vector<std::string> fen_tokens);
  void evaluate_castling_rights(std::string fen_code);
  template<int Quiescent>
  std::vector<Move> GetMoves();
  void Make(const Move move);
  void UnMake();
  void SetStartBoard();
  HashType get_hash() const {
    return hash ^ (castling_rights | GetSquareBitBoard(en_passant + 8));
  }
  //This needs to be public for the evaluation function.
  //PieceBitboardSet get_piece_bitboards() const;
  Piece get_piece(const Square square) const { return pieces[square]; }
  Color get_turn() const { return turn; }
  Color get_not_turn() const { return turn ^ 0x1; }
  BitBoard get_all_pieces() const {
    return color_bitboards[kWhite] | color_bitboards[kBlack];
  }
  BitBoard get_piece_bitboard(const Color color, const PieceType piece_type)
    const { return pt_bitboards[piece_type] & color_bitboards[color]; }
  BitBoard get_color_bitboard(const Color color)
    const { return color_bitboards[color]; }
  BitBoard get_piecetype_bitboard(const PieceType piece_type)
    const { return pt_bitboards[piece_type]; }
  bool has_non_pawn_material(Color color) const {
    return (pt_bitboards[kKnight] | pt_bitboards[kBishop] |
        pt_bitboards[kRook] | pt_bitboards[kQueen]) & color_bitboards[color];
  }
  int8_t get_num_pieces() const {
    return bitops::PopCount(color_bitboards[kWhite] | color_bitboards[kBlack]);
  }
  size_t get_num_made_moves() const { return move_history.size(); }
  int32_t get_piece_count(const Color color, const PieceType piece_type) const {
    return piece_counts[color][piece_type];
  }
  int32_t get_piecetype_count(const PieceType piece_type) const {
    return piece_counts[kWhite][piece_type] + piece_counts[kBlack][piece_type];
  }
  CastlingRights get_castling_rights() const { return castling_rights; }
  int get_phase() const { return phase; }
  //Print unicode chess board.
  bool IsMoveLegal(const Move move) const;
  bool IsTriviallyDrawnEnding() const;
  bool IsFiftyMoveDraw() const { return fifty_move_count >= 100; }
  bool IsDraw() const;
  std::vector<std::string> GetFen() const;
  void Print() const;
  void PrintBitBoards() const {
    for (PieceType pt = kPawn; pt <= kKing; pt++) {
      std::cout << "Piece " << pt << std::endl;
      parse::PrintBitboard(pt_bitboards[pt]);
    }
    std::cout << "White" << std::endl;
    parse::PrintBitboard(color_bitboards[kWhite]);
    std::cout << "Black" << std::endl;
    parse::PrintBitboard(color_bitboards[kBlack]);
  }
  void PrintMadeMoves() const {
    for (size_t i = 0; i < move_history.size(); i++) {
      std::cout << parse::MoveToString(move_history[i]) << " ";
    }
    std::cout << std::endl;
  }
  bool InCheck() const;
  Vec<BitBoard, 6> GetDirectCheckingSquares() const;
  Vec<BitBoard, 6> GetTabooSquares() const;
  bool GivesCheck(const Move move);
  void SetToSamePosition(const Board &board);
  bool NonNegativeSEE(const Move move) const;
  bool NonNegativeSEESquare(const Square target) const;

  Board copy() const;
  Move get_last_move() const { return move_history.back(); }
  BitBoard PlayerBitBoardControl(Color color, BitBoard all_pieces) const;
  bool MoveInListCanRepeat(const std::vector<Move> moves);
  int CountRepetitions(size_t min_ply = 0) const;

private:
  template<int Quiescent, int MoveGenerationType>
  std::vector<Move> GetMoves(BitBoard critical = 0);
  void SwapTurn();
  void AddPiece(const Square square, const Piece piece);
  Piece RemovePiece(const Square square);
  Piece MovePiece(const Square source, const Square destination);
  template<int piece_type>
  PieceType next_see_attacker(const Color color, const Square target,
                              BitBoard &attackers, BitBoard &all_pieces) const;
  //BitBoard piece_bitboards[kNumPlayers][kNumPieceTypes];
  BitBoard pt_bitboards[kNumPieceTypes - 1];
  BitBoard color_bitboards[kNumPlayers];
  int8_t piece_counts[kNumPlayers][kNumPieceTypes - 1];
  Piece pieces[kBoardLength*kBoardLength];
  std::vector<Move> move_history;
  std::vector<HashType> previous_hashes;
  //Includes information necessary to restore a previous state.
  std::vector<MoveHistoryInformation> move_history_information;
  //4 bits are set representing white and black, queen- and kingside castling
  CastlingRights castling_rights;
  int phase;
  Square en_passant;
  Color turn;
  //Ply refers to the number of played halfmoves
  HashType hash;
  int32_t fifty_move_count;
};

#endif /* BOARD_H_ */

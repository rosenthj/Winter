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
 * board.cc
 *
 *  Created on: Nov 1, 2016
 *      Author: Jonathan
 */

#include "board.h"
#include "general/debug.h"
#include "general/settings.h"
#include "general/magic.h"
#include "general/settings.h"
#include "general/bit_operations.h"

#include <random>
#include <iostream>
#include <array>
#include <cassert>

namespace hash {

std::mt19937_64 rng;

const std::array<std::array<std::array<HashType, 64>, 7>, 2> init_pieces() {
  std::array<std::array<std::array<HashType, 64>, 7>, 2> pieces;
  for (Color color = kWhite; color <= kBlack; color++) {
    for (PieceType piece_type = kPawn; piece_type < kNumPieceTypes; piece_type++) {
      for (Square square = 0; square < 64; square++) {
        pieces[color][piece_type][square] = 0;
      }
    }
  }

  for (Color color = kWhite; color <= kBlack; color++) {
    for (PieceType piece_type = kPawn; piece_type <= kKing; piece_type++) {
      for (Square square = 0; square < 64; square++) {
        pieces[color][piece_type][square] = rng();
      }
    }
  }
  return pieces;
}

const std::array<std::array<std::array<HashType, 64>, 7>, 2> pieces = init_pieces();
const HashType color_hash = rng();

inline HashType get_piece(const Color color, const PieceType piece_type, const Square square) {
  return pieces[color][piece_type][square];
}
inline HashType get_piece(const Piece piece,const Square square) {
  return pieces[GetPieceColor(piece)][GetPieceType(piece)][square];
}
inline HashType get_color_hash() {
  return color_hash;
}

}

namespace {

template<Color pawn_owner>
inline bool clearly_drawn_pawn_ending(const BitBoard pawn_bb, const BitBoard defending_king_bb) {
  Square defending_king = bitops::NumberOfTrailingZeros(defending_king_bb);
  Square pawn = bitops::NumberOfTrailingZeros(pawn_bb);
  if ((pawn_owner == kWhite && GetSquareY(pawn) >= GetSquareY(defending_king))
      || (pawn_owner == kBlack && GetSquareY(pawn) <= GetSquareY(defending_king))) {
    return false;
  }
  if (pawn_bb & bitops::rook_file) {
    return std::abs(GetSquareX(pawn)-GetSquareX(defending_king)) <= 1;
  }
  if (GetSquareX(pawn)-GetSquareX(defending_king)) {
    return false;
  }
  if (pawn_owner == kWhite) {
    return GetSquareY(defending_king - pawn) <= 2 && GetSquareY(defending_king) < 7;
  }
  return GetSquareY(pawn - defending_king) <= 2 && GetSquareY(defending_king) > 0;
}

inline bool clearly_drawn_pawn_ending(const BitBoard pawn_bb, const BitBoard kings, const BitBoard w_pieces) {
  if (w_pieces & pawn_bb) {
    return clearly_drawn_pawn_ending<kWhite>(pawn_bb, kings & (~w_pieces));
  }
  return clearly_drawn_pawn_ending<kBlack>(pawn_bb, w_pieces & kings);
}

}

namespace {

const Score see_values[7] = { 100, 300, 300, 450, 900, 10000, 0 };

enum MoveGenerationType {
  kFastMoveGen = 0, kNormalMoveGen = 1, kInCheckMoveGen = 2
};

const BitBoard fourth_row = parse::StringToBitBoard("a4") | parse::StringToBitBoard("b4")
                      | parse::StringToBitBoard("c4") | parse::StringToBitBoard("d4")
                      | parse::StringToBitBoard("e4") | parse::StringToBitBoard("f4")
                      | parse::StringToBitBoard("g4") | parse::StringToBitBoard("h4");

const BitBoard fifth_row = fourth_row << 8;

const BitBoard castling_relevant_bbs[] = {
    parse::StringToBitBoard("h1") | parse::StringToBitBoard("e1"),
    parse::StringToBitBoard("a1") | parse::StringToBitBoard("e1"),
    parse::StringToBitBoard("h8") | parse::StringToBitBoard("e8"),
    parse::StringToBitBoard("a8") | parse::StringToBitBoard("e8")
};
const BitBoard castling_check_bbs[] = {
    parse::StringToBitBoard("e1") | parse::StringToBitBoard("f1") | parse::StringToBitBoard("g1"),
    parse::StringToBitBoard("c1") | parse::StringToBitBoard("d1") | parse::StringToBitBoard("e1"),
    parse::StringToBitBoard("e8") | parse::StringToBitBoard("f8") | parse::StringToBitBoard("g8"),
    parse::StringToBitBoard("c8") | parse::StringToBitBoard("d8") | parse::StringToBitBoard("e8")
};
const BitBoard castling_empty_bbs[] = {
    parse::StringToBitBoard("f1") | parse::StringToBitBoard("g1"),
    parse::StringToBitBoard("b1") | parse::StringToBitBoard("c1") | parse::StringToBitBoard("d1"),
    parse::StringToBitBoard("f8") | parse::StringToBitBoard("g8"),
    parse::StringToBitBoard("b8") | parse::StringToBitBoard("c8") | parse::StringToBitBoard("d8")
};

const BitBoard all_castling_squares = parse::StringToBitBoard("h1") | parse::StringToBitBoard("e1")
                                    | parse::StringToBitBoard("a1") | parse::StringToBitBoard("e1")
                                    | parse::StringToBitBoard("h8") | parse::StringToBitBoard("e8")
                                    | parse::StringToBitBoard("a8") | parse::StringToBitBoard("e8");

//Define helper functions for move history information
void SaveEnPassant(MoveHistoryInformation &info, Square ep) {
  info |= (ep << 4);
}
Square GetEnPassant(const MoveHistoryInformation info) {
  return (info >> 4) & 0x3F;
}
void SaveMovingPiece(MoveHistoryInformation &info, Piece piece) {
  info |= piece;
}
Piece GetMovingPiece(const MoveHistoryInformation info) {
  return info & 0xF;
}
void SaveCastlingRights(MoveHistoryInformation &info, CastlingRights rights) {
  info |= rights << 10;
}
CastlingRights GetCastlingRights(const MoveHistoryInformation info) {
  return (info >> 10) & 0xF;
}
void SaveFiftyMoveCount(MoveHistoryInformation &info, int32_t count) {
  info |= count << 14;
}
int32_t GetFiftyMoveCount(const MoveHistoryInformation info) {
  return (info >> 14) & 0x7F;
}

template<int Quiescent>
void AddMoves(std::vector<Move> &move_list, Square source_square, BitBoard destinations,
    BitBoard enemy_pieces) {
  BitBoard captures = destinations & enemy_pieces;
  BitBoard regulars = destinations ^ captures;
  while (captures) {
    Square destination_square = bitops::NumberOfTrailingZeros(captures);
    move_list.emplace_back(GetMove(source_square, destination_square, kCapture));
    bitops::PopLSB(captures);
  }
  if (!Quiescent) {
    while (regulars) {
      Square destination_square = bitops::NumberOfTrailingZeros(regulars);
      move_list.emplace_back(GetMove(source_square, destination_square, kNormalMove));
      bitops::PopLSB(regulars);
    }
  }
}

template<int Quiescent, int MoveGenType, int PieceType>
void AddMoves(std::vector<Move> &moves, std::vector<Move> &legal_moves, BitBoard piece_bitboard,
    const BitBoard own_pieces, const BitBoard enemy_pieces,
    const BitBoard all_pieces, const BitBoard critical) {
  if (MoveGenType == kNormalMoveGen) {
    BitBoard not_pinned = piece_bitboard & ~critical;
    piece_bitboard ^= not_pinned;
    while (not_pinned) {
      Square piece_square = bitops::NumberOfTrailingZeros(not_pinned);
      BitBoard destinations = magic::GetAttackMap<PieceType>(piece_square, all_pieces);
      destinations &= ~own_pieces;
      AddMoves<Quiescent>(legal_moves, piece_square, destinations, enemy_pieces);
      bitops::PopLSB(not_pinned);
    }
  }
  while (piece_bitboard) {
    Square piece_square = bitops::NumberOfTrailingZeros(piece_bitboard);
    BitBoard destinations = magic::GetAttackMap<PieceType>(piece_square, all_pieces);
    if (MoveGenType == kInCheckMoveGen) {
      destinations &= critical;
    }
    destinations &= ~own_pieces;
    AddMoves<Quiescent>(moves, piece_square, destinations, enemy_pieces);
    bitops::PopLSB(piece_bitboard);
  }
}

template<int Quiescent>
inline void AddPromotionMoves(const Square src, const Square des, std::vector<Move> &moves) {
  moves.emplace_back(GetMove(src, des, kQueenPromotion));
  moves.emplace_back(GetMove(src, des, kKnightPromotion));
  if (Quiescent == kNonQuiescent) {
    moves.emplace_back(GetMove(src, des, kRookPromotion));
    moves.emplace_back(GetMove(src, des, kBishopPromotion));
  }
}

template<int Quiescent, int MoveGenType, int PointOfView>
inline void ConditionalAddPromotionMoves(const Square src, const Square des,
                                         std::vector<Move> &moves, const MoveType move_type) {
  const int back_rank = PointOfView == kWhite ? 7 : 0;
  if (MoveGenType >= kNormalMoveGen || GetSquareY(des) != back_rank) {
    moves.emplace_back(GetMove(src, des, move_type));
  }
  else {
    AddPromotionMoves<Quiescent>(src, des, moves);
  }
}

inline void AddNonPromotionMovesLoop(BitBoard &des, const Square square_dif,
                             std::vector<Move> &moves, const MoveType move_type) {
  while (des) {
    const Square destination = bitops::NumberOfTrailingZeros(des);
    moves.emplace_back(GetMove(destination - square_dif, destination, move_type));
    bitops::PopLSB(des);
  }
}

template<int Quiescent, int MoveGenType, int PointOfView>
inline void AddPawnMovesLoop(BitBoard &des, const Square square_dif,
                             std::vector<Move> &moves, const MoveType move_type) {
  while (des) {
    const Square destination = bitops::NumberOfTrailingZeros(des);
    ConditionalAddPromotionMoves<Quiescent, MoveGenType, PointOfView>(
        destination - square_dif, destination, moves, move_type);
    bitops::PopLSB(des);
  }
}

template<int Quiescent, int MoveGenType, Color PointOfView>
inline void AddPawnMoves(const BitBoard pawn_bb, const BitBoard empty,
                         const BitBoard enemy_pieces, const Square en_passant,
                         std::vector<Move> &moves, const BitBoard critical) {
  const BitBoard double_push_row = PointOfView == kWhite ? fourth_row : fifth_row;
  const int f_east = PointOfView == kWhite ? kNorthEast : kSouthEast;
  const int f_west = PointOfView == kWhite ? kNorthWest : kSouthWest;
  const int b_east = PointOfView == kWhite ? kSouthEast : kNorthEast;
  const int b_west = PointOfView == kWhite ? kSouthWest : kNorthWest;
  const int forward = PointOfView == kWhite ? kNorth : kSouth;
  const int d_east = PointOfView == kWhite ? 9 : -7;
  const int d_west = PointOfView == kWhite ? 7 : -9;
  const int d_forward = PointOfView == kWhite ? 8 : -8;

  if (!Quiescent) {
    BitBoard single_push = bitops::Dir<forward>(pawn_bb) & empty;
    BitBoard double_push = bitops::Dir<forward>(single_push) & empty & double_push_row;
    if (MoveGenType == kInCheckMoveGen) {
      single_push &= critical;
    }
    AddPawnMovesLoop<Quiescent, MoveGenType, PointOfView>(single_push, d_forward, moves, kNormalMove);
    if (MoveGenType == kInCheckMoveGen) {
      double_push &= critical;
    }
    AddNonPromotionMovesLoop(double_push, 2 * d_forward, moves, kDoublePawnMove);
  }
  BitBoard e_captures = bitops::Dir<f_east>(pawn_bb) & enemy_pieces;
  if (MoveGenType == kInCheckMoveGen) {
    e_captures &= critical;
  }
  AddPawnMovesLoop<Quiescent, MoveGenType, PointOfView>(e_captures, d_east, moves, kCapture);
  BitBoard w_captures = bitops::Dir<f_west>(pawn_bb) & enemy_pieces;
  if (MoveGenType == kInCheckMoveGen) {
    w_captures &= critical;
  }
  AddPawnMovesLoop<Quiescent, MoveGenType, PointOfView>(w_captures, d_west, moves, kCapture);
  if (en_passant) {
    BitBoard ep_bitboard = GetSquareBitBoard(en_passant);
    BitBoard ep_captures = (bitops::Dir<b_west>(ep_bitboard) | bitops::Dir<b_east>(ep_bitboard))
                               &  pawn_bb;
    while (ep_captures) {
      const Square source = bitops::NumberOfTrailingZeros(ep_captures);
      moves.emplace_back(GetMove(source, en_passant, kEnPassant));
      bitops::PopLSB(ep_captures);
    }
  }
}

}

void PrintStandardRow(std::string first_delim, std::string mid_delim, std::string last_delim) {
  for (int column = 0; column < kBoardLength; column++) {
    if (column == 0) {
      std::cout << std::string(2,' ') << first_delim;
    }
    else {
      std::cout << mid_delim;
    }
      std::cout << unicode::e_w_vec << unicode::e_w_vec << unicode::e_w_vec;
    if (column == kBoardLength-1) {
      std::cout << last_delim;
    }
  }
  std::cout << std::endl;
}

Board::Board() {
  hash = 0;
  en_passant = 0;
  fifty_move_count = 0;
  phase = 0;
  for (int player = kWhite; player <= kBlack; player++) {
    color_bitboards[player] = 0;
    for (int piece_type = 0; piece_type < kNumPieceTypes - 1; piece_type++) {
      piece_counts[player][piece_type] = 0;
    }
  }
  for (int piece_type = 0; piece_type < kNumPieceTypes - 1; piece_type++) {
    pt_bitboards[piece_type] = 0;
  }

  for (Square square = parse::StringToSquare("a1");
      square <= parse::StringToSquare("h8"); square++) {
    pieces[square] = kNoPiece;
  }
  for (Color color = kWhite; color <= kBlack; color++) {
    AddPiece(parse::StringToSquare("a1") + (56*color), GetPiece(color, kRook));
    AddPiece(parse::StringToSquare("b1") + (56*color), GetPiece(color, kKnight));
    AddPiece(parse::StringToSquare("c1") + (56*color), GetPiece(color, kBishop));
    AddPiece(parse::StringToSquare("d1") + (56*color), GetPiece(color, kQueen));
    AddPiece(parse::StringToSquare("e1") + (56*color), GetPiece(color, kKing));
    AddPiece(parse::StringToSquare("f1") + (56*color), GetPiece(color, kBishop));
    AddPiece(parse::StringToSquare("g1") + (56*color), GetPiece(color, kKnight));
    AddPiece(parse::StringToSquare("h1") + (56*color), GetPiece(color, kRook));
    for (int i = 0; i < 8; i++) {
      AddPiece(parse::StringToSquare("a2") + i + (40*color), GetPiece(color, kPawn));
    }
  }
  castling_rights = 15;
  turn = kWhite;
}

std::vector<std::string> Board::GetFen() const {
  std::vector<std::string> fen;
  std::string board_fen = "";
  for (int row = 7; row >= 0; row--) {
    int empty_in_a_row = 0;
    for (int col = 0; col < 8; col++) {
      Square square = GetSquare(col, row);
      if (get_piece(square) == kNoPiece) {
        empty_in_a_row++;
      }
      else {
        if (empty_in_a_row > 0) {
          board_fen.append(std::to_string(empty_in_a_row));
          empty_in_a_row = 0;
        }
        switch (get_piece(square)) {
          case GetPiece(kWhite, kPawn): board_fen.append("P"); break;
          case GetPiece(kBlack, kPawn): board_fen.append("p"); break;
          case GetPiece(kWhite, kKnight): board_fen.append("N"); break;
          case GetPiece(kBlack, kKnight): board_fen.append("n"); break;
          case GetPiece(kWhite, kBishop): board_fen.append("B"); break;
          case GetPiece(kBlack, kBishop): board_fen.append("b"); break;
          case GetPiece(kWhite, kRook): board_fen.append("R"); break;
          case GetPiece(kBlack, kRook): board_fen.append("r"); break;
          case GetPiece(kWhite, kQueen): board_fen.append("Q"); break;
          case GetPiece(kBlack, kQueen): board_fen.append("q"); break;
          case GetPiece(kWhite, kKing): board_fen.append("K"); break;
          case GetPiece(kBlack, kKing): board_fen.append("k"); break;
        }
      }
    }
    if (empty_in_a_row) {
      board_fen.append(std::to_string(empty_in_a_row));
    }
    if (row > 0) {
      board_fen.append("/");
    }
  }
  fen.emplace_back(board_fen);
  if (get_turn() == kWhite) {
    fen.emplace_back("w");
  }
  else {
    fen.emplace_back("b");
  }

  if (!castling_rights) {
    fen.emplace_back("-");
  }
  else {
    std::string castling_fen = "";
    if (castling_rights & kWSCastle)
      castling_fen.append("K");
    if (castling_rights & kWLCastle)
      castling_fen.append("Q");
    if (castling_rights & kBSCastle)
      castling_fen.append("k");
    if (castling_rights & kBLCastle)
      castling_fen.append("q");
    fen.emplace_back(castling_fen);
  }

  if (en_passant) {
    fen.emplace_back(parse::SquareToString(en_passant));
  }
  else {
    fen.emplace_back("-");
  }

  return fen;
}

void Board::SetBoard(std::vector<std::string> fen_tokens){
  move_history.clear();
  move_history_information.clear();
  previous_hashes.clear();
  hash = 0;
  en_passant = 0;
  fifty_move_count = 0;
  phase = 0;
  for (int player = kWhite; player <= kBlack; player++) {
    color_bitboards[player] = 0;
    for (int piece_type = 0; piece_type < kNumPieceTypes - 1; piece_type++) {
      piece_counts[player][piece_type] = 0;
    }
  }
  for (int piece_type = 0; piece_type < kNumPieceTypes - 1; piece_type++) {
    pt_bitboards[piece_type] = 0;
  }
  for (Square square = parse::StringToSquare("a1");
       square <= parse::StringToSquare("h8"); square++) {
    pieces[square] = kNoPiece;
  }

  std::vector<std::string> fen_position_rows = parse::split(fen_tokens[0], '/');
  for (int row = 0; row < kBoardLength; row++) {
    Square square = GetSquare(0, 7 - row);
    for (int char_idx = 0; char_idx < fen_position_rows[row].length(); char_idx++) {
      char c = fen_position_rows[row][char_idx];
      switch (c){
        case 'K': AddPiece(square, GetPiece(kWhite, kKing)); break;
        case 'Q': AddPiece(square, GetPiece(kWhite, kQueen)); break;
        case 'R': AddPiece(square, GetPiece(kWhite, kRook)); break;
        case 'B': AddPiece(square, GetPiece(kWhite, kBishop)); break;
        case 'N': AddPiece(square, GetPiece(kWhite, kKnight)); break;
        case 'P': AddPiece(square, GetPiece(kWhite, kPawn)); break;

        case 'k': AddPiece(square, GetPiece(kBlack, kKing)); break;
        case 'q': AddPiece(square, GetPiece(kBlack, kQueen)); break;
        case 'r': AddPiece(square, GetPiece(kBlack, kRook)); break;
        case 'b': AddPiece(square, GetPiece(kBlack, kBishop)); break;
        case 'n': AddPiece(square, GetPiece(kBlack, kKnight)); break;
        case 'p': AddPiece(square, GetPiece(kBlack, kPawn)); break;

        case '1': square += 0; break;
        case '2': square += 1; break;
        case '3': square += 2; break;
        case '4': square += 3; break;
        case '5': square += 4; break;
        case '6': square += 5; break;
        case '7': square += 6; break;
        case '8': square += 7; break;

        default: break;
      }
      square++;
    }
  }

  // Whose turn is it?
  turn = kWhite;
  if(fen_tokens[1] == "b"){
    SwapTurn();
  }

  if (fen_tokens.size() == 2){
    return;
  }
  evaluate_castling_rights(fen_tokens[2]);

  if (fen_tokens[3] != "-") {
    en_passant = parse::StringToSquare(fen_tokens[3]);
  }
}

void Board::evaluate_castling_rights(std::string fen_code){
  castling_rights = 0;

  int len = fen_code.length();
  for(int i = 0; i < len; i++){
    char c = fen_code[i];
    switch (c){
      case 'K': castling_rights |= kWSCastle; break;
      case 'Q': castling_rights |= kWLCastle; break;
      case 'k': castling_rights |= kBSCastle; break;
      case 'q': castling_rights |= kBLCastle; break;

      default: break;
    }
  }
}

void Board::SetStartBoard() {
  SetBoard({"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", "w", "KQkq", "-"});
}

void Board::SetToSamePosition(const Board &board) {
  hash = board.hash;
  en_passant = board.en_passant;
  fifty_move_count = board.fifty_move_count;
  move_history = board.move_history;
  move_history_information = board.move_history_information;
  previous_hashes = board.previous_hashes;
  phase = board.phase;
  for (int player = kWhite; player <= kBlack; player++) {
    color_bitboards[player] = board.color_bitboards[player];
    for (int piece_type = 0; piece_type < kNumPieceTypes - 1; piece_type++) {
      piece_counts[player][piece_type] = board.piece_counts[player][piece_type];
    }
  }
  for (int piece_type = 0; piece_type < kNumPieceTypes - 1; piece_type++) {
    pt_bitboards[piece_type] = board.pt_bitboards[piece_type];
  }
  for (Square square = parse::StringToSquare("a1");
      square <= parse::StringToSquare("h8"); square++) {
    pieces[square] = board.pieces[square];
  }
  castling_rights = board.castling_rights;
  turn = board.turn;
}

void Board::AddPiece(const Square square, const Piece piece) {
  pt_bitboards[GetPieceType(piece)] |= GetSquareBitBoard(square);
  color_bitboards[GetPieceColor(piece)] |= GetSquareBitBoard(square);
  piece_counts[GetPieceColor(piece)][GetPieceType(piece)]++;
  phase += piece_phases[GetPieceType(piece)];
  pieces[square] = piece;
  hash ^= hash::get_piece(piece, square);
}

Piece Board::RemovePiece(const Square square) {
  Piece piece = pieces[square];
  if (GetPieceType(piece) != kNoPiece) {
    pieces[square] = kNoPiece;
    pt_bitboards[GetPieceType(piece)] ^= GetSquareBitBoard(square);
    color_bitboards[GetPieceColor(piece)] ^= GetSquareBitBoard(square);
    piece_counts[GetPieceColor(piece)][GetPieceType(piece)]--;
    phase -= piece_phases[GetPieceType(piece)];
    hash ^= hash::get_piece(piece, square);
  }
  return piece;
}

Piece Board::MovePiece(const Square source, const Square destination) {
  Piece piece = RemovePiece(destination);
  AddPiece(destination, RemovePiece(source));
  assert(pieces[source] == kNoPiece);
  assert(pieces[destination] != kNoPiece);
  return piece;
}

void Board::SwapTurn() {
  turn ^= 0x1;
  hash ^= hash::get_color_hash();
}

void Board::Make(const Move move) {
  MoveHistoryInformation information = 0;
  previous_hashes.emplace_back(get_hash());
  if (!settings::kUseNullMoves || move != kNullMove) {
    SaveMovingPiece(information, RemovePiece(GetMoveDestination(move)));
    MovePiece(GetMoveSource(move),GetMoveDestination(move));
  }
  SaveEnPassant(information, en_passant);
  SaveCastlingRights(information, castling_rights);
  SaveFiftyMoveCount(information, fifty_move_count);
  //We default our ep square to a place the opponent will never be able to ep.
  en_passant = 0;
  fifty_move_count++;
  if (GetPieceType(pieces[GetMoveDestination(move)]) == kPawn
      || GetMoveType(move) == kCapture) {
    fifty_move_count = 0;
  }
  switch (GetMoveType(move)) {
  case kDoublePawnMove:
    en_passant = GetMoveDestination(move) - 8 + (2*8) * get_turn();
    break;
  case kCastle:
    if (GetMoveDestination(move) < GetMoveSource(move)) {
      //Queen-side castling
      MovePiece(GetMoveSource(move)-4, GetMoveSource(move)-1);
    }
    else {
      //King-side castling
      MovePiece(GetMoveSource(move)+3, GetMoveSource(move)+1);
    }
    break;
  case kEnPassant:
    RemovePiece(GetMoveDestination(move) - 8 + (2*8) * get_turn());
    break;
  default:
    if (GetMoveType(move) >= kKnightPromotion) {
      RemovePiece(GetMoveDestination(move));
      AddPiece(GetMoveDestination(move),
          GetPiece(get_turn(), GetMoveType(move) - kKnightPromotion + kKnight));
    }
    break;
  }
  if (!settings::kUseNullMoves || move != kNullMove) {
    BitBoard srcdes = GetSquareBitBoard(GetMoveSource(move))
        | GetSquareBitBoard(GetMoveDestination(move));
    if (srcdes & all_castling_squares) {
      for (int right = 0; right < 4; right++) {
        if (castling_relevant_bbs[right] & srcdes) {
          castling_rights &= ~(0x1 << right);
        }
      }
    }
  }
  move_history_information.emplace_back(information);
  move_history.emplace_back(move);
  SwapTurn();
  search::inc_node_count();
}

void Board::UnMake() {
  SwapTurn();
  Move move = move_history.back();
  move_history.pop_back();
  MoveHistoryInformation info = move_history_information.back();
  move_history_information.pop_back();
  previous_hashes.pop_back();
  if (!settings::kUseNullMoves || move != kNullMove) {
    AddPiece(GetMoveSource(move), RemovePiece(GetMoveDestination(move)));
    Piece piece = GetMovingPiece(info);
    if (GetPieceType(piece) != kNoPiece) {
      AddPiece(GetMoveDestination(move), piece);
    }
  }
  en_passant = GetEnPassant(info);
  castling_rights = GetCastlingRights(info);
  fifty_move_count = GetFiftyMoveCount(info);
  switch(GetMoveType(move)) {
  case kEnPassant:
    AddPiece(GetMoveDestination(move) - 8 + (2*8) * get_turn(), GetPiece(get_not_turn(), kPawn));
    break;
  case kCastle:
    if (GetMoveDestination(move) < GetMoveSource(move)) {
      //Queen-side castling
      MovePiece(GetMoveSource(move)-1, GetMoveSource(move)-4);
    }
    else {
      //King-side castling
      MovePiece(GetMoveSource(move)+1, GetMoveSource(move)+3);
    }
    break;
  default:
    if (GetMoveType(move) >= kKnightPromotion) {
      RemovePiece(GetMoveSource(move));
      AddPiece(GetMoveSource(move), GetPiece(get_turn(), kPawn));
    }
    break;
  }
}

void Board::Print() const {
  PrintStandardRow(unicode::s_e_vec, unicode::s_e_w_vec, unicode::s_w_vec);
  for (int row = kBoardLength; row > 0; row--) {
    std::cout << row << ' ' << unicode::n_s_vec;
    for (int column = 0; column < kBoardLength; column++) {
      Square square = (row-1)*kBoardLength + column;
      std::cout << ' ' <<
          (unicode::pieces[GetPieceColor(pieces[square])][GetPieceType(pieces[square])])
          << ' ' << unicode::n_s_vec;
    }
    std::cout << std::endl;
    if (row > 1) {
      PrintStandardRow(unicode::n_s_e_vec, unicode::n_s_e_w_vec,
                       unicode::n_s_w_vec);
    }
  }
  PrintStandardRow(unicode::n_e_vec, unicode::n_e_w_vec, unicode::n_w_vec);
  std::cout << " ";
  for (char column = 'a'; column <= 'h'; column++) {
    std::cout << "   " << column;
  }
  std::cout << std::endl;
}

BitBoard Board::PlayerBitBoardControl(Color color, BitBoard all_pieces) const {
  BitBoard under_control = 0;

  if (color == kWhite) {
    under_control |= bitops::NE(get_piece_bitboard(color, kPawn))
                   | bitops::NW(get_piece_bitboard(color, kPawn));
  }
  else {
    under_control |= bitops::SE(get_piece_bitboard(color, kPawn))
                   | bitops::SW(get_piece_bitboard(color, kPawn));
  }

  BitBoard knights = get_piece_bitboard(color, kKnight);
  while (knights) {
    Square knight_square = bitops::NumberOfTrailingZeros(knights);
    under_control |= magic::GetAttackMap<kKnight>(knight_square, all_pieces);
    bitops::PopLSB(knights);
  }

  BitBoard pieces = get_piece_bitboard(color, kBishop);
  while (pieces) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    under_control |= magic::GetAttackMap<kBishop>(piece_square, all_pieces);
    bitops::PopLSB(pieces);
  }

  pieces = get_piece_bitboard(color, kRook);
  while (pieces) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    under_control |= magic::GetAttackMap<kRook>(piece_square, all_pieces);
    bitops::PopLSB(pieces);
  }

  pieces = get_piece_bitboard(color, kQueen);
  while (pieces) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    under_control |= magic::GetAttackMap<kQueen>(piece_square, all_pieces);
    bitops::PopLSB(pieces);
  }

  Square king_square = bitops::NumberOfTrailingZeros(get_piece_bitboard(color, kKing));
  under_control  |= magic::GetAttackMap<kKing>(king_square, all_pieces);
  return under_control;
}

template<int Quiescent, int MoveGenType>
std::vector<Move> Board::GetMoves(const BitBoard critical) {
  std::vector<Move> moves;
  std::vector<Move> legal_moves;

  const int expected_num_moves = Quiescent == kQuiescent ? 16 : 48;
  if (MoveGenType == kInCheckMoveGen) {
    legal_moves.reserve(expected_num_moves / 4);
    moves.reserve(expected_num_moves / 2);
  }
  else if (MoveGenType == kFastMoveGen) {
    moves.reserve(expected_num_moves);
  }
  else {
    legal_moves.reserve(expected_num_moves);
    moves.reserve(expected_num_moves);
  }

  const BitBoard own_pieces = color_bitboards[get_turn()];
  const BitBoard enemy_pieces = color_bitboards[get_not_turn()];
  const BitBoard all_pieces = own_pieces | enemy_pieces;
  const BitBoard empty = ~all_pieces;
  AddMoves<Quiescent, MoveGenType, kKnight>(moves, legal_moves,
      get_piece_bitboard(get_turn(), kKnight), own_pieces, enemy_pieces,
      all_pieces, critical);
  AddMoves<Quiescent, MoveGenType, kBishop>(moves, legal_moves,
      get_piece_bitboard(get_turn(), kBishop), own_pieces, enemy_pieces,
      all_pieces, critical);
  AddMoves<Quiescent, MoveGenType, kRook>(moves, legal_moves,
      get_piece_bitboard(get_turn(), kRook), own_pieces, enemy_pieces,
      all_pieces, critical);
  AddMoves<Quiescent, MoveGenType, kQueen>(moves, legal_moves,
      get_piece_bitboard(get_turn(), kQueen), own_pieces, enemy_pieces,
      all_pieces, critical);

  //Pawns
  if (get_turn() == kWhite) {
    AddPawnMoves<Quiescent, MoveGenType, kWhite>(
        get_piece_bitboard(kWhite, kPawn), empty, enemy_pieces, en_passant, moves, critical);
  }
  else {
    AddPawnMoves<Quiescent, MoveGenType, kBlack>(
        get_piece_bitboard(kBlack, kPawn), empty, enemy_pieces, en_passant, moves, critical);
  }

  //King
  BitBoard king = get_piece_bitboard(get_turn(), kKing);
  const Square king_square = bitops::NumberOfTrailingZeros(king);
  king |= bitops::E(king) | bitops::W(king);
  king |= bitops::N(king) | bitops::S(king);
  BitBoard in_check = PlayerBitBoardControl(get_not_turn(), all_pieces);
  king &= ~(own_pieces | in_check);
  if (MoveGenType != kNormalMoveGen) {
    AddMoves<Quiescent>(moves, king_square, king, enemy_pieces);
  }
  else {
    AddMoves<Quiescent>(legal_moves, king_square, king, enemy_pieces);
  }
  if (!Quiescent) {
    // Add castling moves
    for (int right = 0 + 2*get_turn(); right < 2+2*get_turn(); right++) {
      if ((castling_rights & (0x1 << right))
          && !(castling_check_bbs[right] & in_check)
          && !(castling_empty_bbs[right] & all_pieces)) {
        Square source = bitops::NumberOfTrailingZeros(get_piece_bitboard(get_turn(), kKing));
        Square destination = source + 2 - (right%2)*4;
        if (MoveGenType == kNormalMoveGen) {
          legal_moves.emplace_back(GetMove(source, destination, kCastle));
        }
        else {
          moves.emplace_back(GetMove(source, destination, kCastle));
        }
      }
    }
  }

  if (MoveGenType == kFastMoveGen) {
    return moves;
  }

  //Now we need to remove illegal moves.
  BitBoard dangerous_sources = magic::GetAttackMap<kQueen>(king_square, all_pieces)
      | get_piece_bitboard(get_turn(), kKing);
  for (Move move : moves) {
    BitBoard move_src = GetSquareBitBoard(GetMoveSource(move));
    bool add = false;
    if (MoveGenType != kInCheckMoveGen) {
      if (!(dangerous_sources & move_src) && GetMoveType(move) != kEnPassant) {
        add = true;
      }
      else {
        Make(move);
        SwapTurn();
        if (!InCheck()) {
          add = true;
        }
        SwapTurn();
        UnMake();
        search::dec_node_count();
      }
    }
    else {
      Make(move);
      SwapTurn();
      if (!InCheck()) {
        add = true;
      }
      SwapTurn();
      UnMake();
      search::dec_node_count();
    }
    if (add) {
      if (GetPieceType(pieces[GetMoveSource(move)]) == kPawn
          && (GetSquareY(GetMoveDestination(move)) - 7*(get_not_turn())) == 0) {
        Square source = GetMoveSource(move);
        Square destination = GetMoveDestination(move);
        AddPromotionMoves<Quiescent>(source, destination, legal_moves);
      }
      else {
        legal_moves.emplace_back(move);
      }
    }

  }
  return legal_moves;
}

template<int Quiescent>
std::vector<Move> Board::GetMoves() {
  Square king_square = bitops::NumberOfTrailingZeros(get_piece_bitboard(get_turn(), kKing));
  BitBoard danger = magic::GetAttackMap<kKnight>(king_square, 0) & get_piece_bitboard(get_not_turn(), kKnight);
  danger |= magic::GetAttackMap<kRook>(king_square, 0)
      & (get_piece_bitboard(get_not_turn(), kRook) | get_piece_bitboard(get_not_turn(), kQueen));
  danger |= magic::GetAttackMap<kBishop>(king_square, 0)
      & (get_piece_bitboard(get_not_turn(), kBishop) | get_piece_bitboard(get_not_turn(), kQueen));
  BitBoard enemy_pawns = get_piece_bitboard(get_not_turn(), kPawn);
  danger |= ((bitops::SE(enemy_pawns) | bitops::SW(enemy_pawns)) << (16 * get_turn()))
      & get_piece_bitboard(get_turn(), kKing);
  if (!danger) {
    return GetMoves<Quiescent, kFastMoveGen>();
  }


  danger = magic::GetAttackMap<kKnight>(king_square, 0) & get_piece_bitboard(get_not_turn(), kKnight);
  BitBoard king = get_piece_bitboard(get_turn(), kKing);
  if (get_turn() == kWhite) {
    danger |= (bitops::NE(king) | bitops::NW(king)) & enemy_pawns;
  }
  else {
    danger |= (bitops::SE(king) | bitops::SW(king)) & enemy_pawns;
  }
  if (danger) {
    return GetMoves<kNonQuiescent, kInCheckMoveGen>(danger);
  }

  BitBoard own_pieces = color_bitboards[get_turn()];
  BitBoard enemy_pieces = color_bitboards[get_not_turn()];
  BitBoard all_pieces = own_pieces | enemy_pieces;

  danger |= magic::GetAttackMap<kRook>(king_square, all_pieces)
      & (get_piece_bitboard(get_not_turn(), kRook) | get_piece_bitboard(get_not_turn(), kQueen));
  if (danger) {
    return GetMoves<kNonQuiescent, kInCheckMoveGen>(
        magic::GetAttackMap<kRook>(king_square, all_pieces));
  }
  danger |= magic::GetAttackMap<kBishop>(king_square, all_pieces)
      & (get_piece_bitboard(get_not_turn(), kBishop) | get_piece_bitboard(get_not_turn(), kQueen));

  if (danger) {
    return GetMoves<kNonQuiescent, kInCheckMoveGen>(
        magic::GetAttackMap<kBishop>(king_square, all_pieces));
  }
  BitBoard possibly_pinned = 0;
  const BitBoard rook_attacks = magic::GetAttackMap<kRook>(king_square, 0);
  if (rook_attacks & (get_piece_bitboard(get_not_turn(), kRook)
                    | get_piece_bitboard(get_not_turn(), kQueen))) {
    possibly_pinned |= rook_attacks & own_pieces;
  }
  const BitBoard bishop_attacks = magic::GetAttackMap<kBishop>(king_square, 0);
  if (bishop_attacks & (get_piece_bitboard(get_not_turn(), kBishop)
                    | get_piece_bitboard(get_not_turn(), kQueen))) {
    possibly_pinned |= bishop_attacks & own_pieces;
  }
  return GetMoves<Quiescent, kNormalMoveGen>(possibly_pinned);
}

template std::vector<Move> Board::GetMoves<kNonQuiescent>();
template std::vector<Move> Board::GetMoves<kQuiescent>();

bool Board::InCheck() const {
  BitBoard bitBoard = get_piece_bitboard(get_turn(), kKing);
  BitBoard p, targeted;
  BitBoard all_pieces = color_bitboards[kWhite] | color_bitboards[kBlack];
  p = get_piece_bitboard(get_not_turn(), kKing);
  targeted = bitops::E(p) | p | bitops::W(p);
  targeted |= bitops::N(targeted) | bitops::S(targeted);
  p = get_piece_bitboard(get_not_turn(), kPawn);
  targeted |= (bitops::SE(p) | bitops::SW(p)) << (16 * get_turn());
  targeted &= bitBoard;

  int index = bitops::NumberOfTrailingZeros(bitBoard);
  targeted |= magic::GetAttackMap<kKnight>(index, 0)
      & get_piece_bitboard(get_not_turn(), kKnight);
  targeted |= magic::GetAttackMap<kBishop>(index, all_pieces)
      & (get_piece_bitboard(get_not_turn(), kBishop) | get_piece_bitboard(get_not_turn(), kQueen));
  targeted |= magic::GetAttackMap<kRook>(index, all_pieces)
      & (get_piece_bitboard(get_not_turn(), kRook) | get_piece_bitboard(get_not_turn(), kQueen));

  return targeted;
}

bool Board::InTwoFoldRepetition() const {
  int repetitions = 0;
  int min_index = previous_hashes.size() - fifty_move_count;
  if (min_index < 0) {
    min_index = 0;
  }
  HashType cur_hash = get_hash();
  for (int index = previous_hashes.size()-2; index >= min_index; index-=2) {
    repetitions += (cur_hash == previous_hashes[index]);
  }
  return repetitions != 0;
}

template<int piece_type>
PieceType Board::next_see_attacker(const Color color, const Square target,
                               BitBoard &attackers, BitBoard &all_pieces) const {

  if (piece_type >= kNoPiece) {
    return kNoPiece;
  }

  BitBoard pt_bitboard = attackers & get_piece_bitboard(color, piece_type);
  if (!pt_bitboard) {
    if (piece_type < kKing) {
      return next_see_attacker<piece_type+1>(color, target, attackers, all_pieces);
    }
    return kNoPiece;
  }

  BitBoard next_attacker = bitops::GetLSB(pt_bitboard);
  attackers ^= next_attacker;
  all_pieces ^= next_attacker;

  if (piece_type == kPawn || piece_type == kBishop || piece_type == kQueen) {
    BitBoard diagonals = magic::GetAttackMap<kBishop>(target, all_pieces)
                            & (pt_bitboards[kBishop] | pt_bitboards[kQueen])
                            & all_pieces;
    attackers |= diagonals;
  }

  if (piece_type == kRook || piece_type == kQueen) {
    BitBoard verticals = magic::GetAttackMap<kRook>(target, all_pieces)
                            & (pt_bitboards[kRook] | pt_bitboards[kQueen])
                            & all_pieces;
    attackers |= verticals;
  }

  return piece_type;
}

template<>
PieceType Board::next_see_attacker<kNoPiece>(const Color color, const Square target,
                               BitBoard &attackers, BitBoard &all_pieces) const {
  return kNoPiece;
}

bool Board::NonNegativeSEE(const Move move) const {
  Color cturn = turn^0x1;
  Square target = GetMoveDestination(move);
  BitBoard all_pieces = get_all_pieces();
  Score score = -see_values[GetPieceType(get_piece(target))];
  Score victim = see_values[GetPieceType(get_piece(GetMoveSource(move)))];
  all_pieces ^= GetSquareBitBoard(GetMoveSource(move));

  BitBoard targetBB = GetSquareBitBoard(target);
  BitBoard attackers = (bitops::SE(targetBB) | bitops::SW(targetBB)) & get_piece_bitboard(kWhite, kPawn);
  attackers |= (bitops::NE(targetBB) | bitops::NW(targetBB)) & get_piece_bitboard(kBlack, kPawn);
  attackers |= magic::GetAttackMap<kKnight>(target, all_pieces) & pt_bitboards[kKnight];
  attackers |= magic::GetAttackMap<kBishop>(target, all_pieces) & (pt_bitboards[kBishop] | pt_bitboards[kQueen]);
  attackers |= magic::GetAttackMap<kRook>(target, all_pieces) & (pt_bitboards[kRook] | pt_bitboards[kQueen]);
  attackers |= magic::GetAttackMap<kKing>(target, all_pieces) & (pt_bitboards[kKing]);
  attackers &= all_pieces;
  while ((attackers & color_bitboards[cturn]) && score <= 0) {
    if (cturn == turn && score == 0) {
      return true;
    }
    PieceType pt = next_see_attacker<kPawn>(cturn, target, attackers, all_pieces);
    score = -(score + victim);
    victim = see_values[pt];
    cturn ^= 0x1;
  }

  return (score >= 0 && cturn == get_turn()) || (score <= 0 && cturn == get_not_turn());
}

bool Board::IsMoveLegal(const Move move) const {
  debug::Error("Use of not yet implemented function Board::IsMoveLegal");
  if (move == kNullMove) {
    return false;
  }
  Piece piece = get_piece(GetMoveSource(move));
  if (GetPieceType(piece) == kNoPiece || GetPieceColor(piece) != get_turn()) {
    return false;
  }
  switch (GetMoveType(move)) {
    case kEnPassant:
      if (!en_passant || en_passant != GetMoveDestination(move) || GetPieceType(piece) != kPawn) {
        return false;
      }
      return true;
  }
  return true;
}

Vec<BitBoard, 6> Board::GetDirectCheckingSquares() const {
  Vec<BitBoard, 6> direct_checks;
  BitBoard king_bb = get_piece_bitboard(get_not_turn(), kKing);
  if (get_turn() == kWhite) {
    direct_checks[kPawn] = bitops::SW(king_bb) | bitops::SE(king_bb);
  }
  else {
    direct_checks[kPawn] = bitops::NW(king_bb) | bitops::NE(king_bb);
  }
  Square square = bitops::NumberOfTrailingZeros(king_bb);
  BitBoard all_pieces = get_all_pieces();
  direct_checks[kKnight] = magic::GetAttackMap<kKnight>(square, all_pieces);
  direct_checks[kBishop] = magic::GetAttackMap<kBishop>(square, all_pieces);
  direct_checks[kRook] = magic::GetAttackMap<kRook>(square, all_pieces);
  direct_checks[kQueen] = direct_checks[kBishop] | direct_checks[kRook];
  direct_checks[kKing] = 0;
  return direct_checks;
}

Vec<BitBoard, 6> Board::GetTabooSquares() const {
  const Color not_turn = get_not_turn();
  const BitBoard all_pieces = get_all_pieces();
  Vec<BitBoard, 6> taboo_squares;
  BitBoard taboo;
  if (get_turn() == kWhite) {
    BitBoard bb = get_piece_bitboard(not_turn, kPawn);
    taboo = bitops::SE(bb) | bitops::SW(bb);
  }
  else {
    BitBoard bb = get_piece_bitboard(not_turn, kPawn);
    taboo = bitops::NE(bb) | bitops::NW(bb);
  }
  taboo_squares[kPawn] = 0;
  taboo_squares[kKnight] = taboo;
  taboo_squares[kBishop] = taboo;

  BitBoard piece_bb = get_piece_bitboard(not_turn, kKnight);
  while (piece_bb) {
    taboo |= magic::GetAttackMap<kKnight>(bitops::NumberOfTrailingZeros(piece_bb),
                                          all_pieces);
    bitops::PopLSB(piece_bb);
  }
  piece_bb = get_piece_bitboard(not_turn, kBishop);
  while (piece_bb) {
    taboo |= magic::GetAttackMap<kBishop>(bitops::NumberOfTrailingZeros(piece_bb),
                                          all_pieces);
    bitops::PopLSB(piece_bb);
  }
  taboo_squares[kRook] = taboo;

  piece_bb = get_piece_bitboard(not_turn, kRook);
  while (piece_bb) {
    taboo |= magic::GetAttackMap<kRook>(bitops::NumberOfTrailingZeros(piece_bb),
                                          all_pieces);
    bitops::PopLSB(piece_bb);
  }
  taboo_squares[kQueen] = taboo;
  taboo_squares[kKing] = 0;
  return taboo_squares;
}


bool Board::GivesCheck(const Move move) {
  Make(move);
  bool gives_check = InCheck();
  UnMake();
  search::dec_node_count();
  return gives_check;
}

bool Board::IsDraw() const {
  return (fifty_move_count >= 100) || InTwoFoldRepetition()
      || (get_num_pieces() == 3 && ((get_piecetype_bitboard(kKnight) | get_piecetype_bitboard(kBishop))
          || (get_piecetype_bitboard(kPawn) && clearly_drawn_pawn_ending(get_piecetype_bitboard(kPawn),
                                                                         get_piecetype_bitboard(kKing),
                                                                         get_color_bitboard(kWhite)))))
      || get_num_pieces() == 2;
}

Board Board::copy() const {
  Board result;
  result.SetToSamePosition((*this));
  return result;
}


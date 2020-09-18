#include "pyrrhic/tbprobe.h"
#include "general/types.h"
#include "board.h"

#include <iostream>

extern int TB_LARGEST;

namespace {

// This is slow but should only get called from root
Move pyrrhic_move_to_regular(std::vector<Move> &moves, PyrrhicMove move) {
  Square src = TB_GET_FROM(move);
  Square des = TB_GET_TO(move);
  int32_t promotion = TB_GET_PROMOTES(move);
  if (promotion) {
    MoveType movetype = kKnightPromotion + 4 - promotion;
    return GetMove(src, des, movetype);
  }
  for (Move candidate : moves) {
    if (GetMoveSource(candidate) == src && GetMoveDestination(candidate) == des) {
      return candidate;
    }
  }
  return GetMove(src, des, kNormalMove);
}

}

namespace egtb {

WDLScore ProbeWDL(const Board &board) {
  
  if (board.get_fifty_move_count() > 0
      || board.get_castling_rights() != 0
      || board.get_num_pieces() > TB_LARGEST) {
    return kNoScore;
  }
    
  uint32_t res = tb_probe_wdl(
    board.get_color_bitboard(kWhite), board.get_color_bitboard(kBlack),
    board.get_piecetype_bitboard(kKing), board.get_piecetype_bitboard(kQueen),
    board.get_piecetype_bitboard(kRook), board.get_piecetype_bitboard(kBishop),
    board.get_piecetype_bitboard(kKnight), board.get_piecetype_bitboard(kPawn),
    board.get_en_passant(), board.get_turn() == kWhite);
  
  if (res == TB_RESULT_FAILED) {
    std::cout << "Failed" << std::endl;
  }
  
  if (res == TB_WIN) {
    if (board.get_turn() == kWhite) {
      //board.Print();
      //std::cout << "Black to move" << std::endl;
    }
    return GetTBWinOnMoveScore(board.get_num_made_moves());
  }
  else if (res == TB_LOSS) {
    if (board.get_turn() == kBlack) {
      //board.Print();
      //for (auto token : board.GetFen()) {
      //  std::cout << token << " ";
      //}
      //std::cout << std::endl;
      //std::cout << "Black to move" << std::endl;
    }
    return GetTBLossOnMoveScore(board.get_num_made_moves());
  }
  return kDrawScore;
}

std::vector<Move> ProbOptimalMoves(Board &board) {
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  if (board.get_castling_rights() != 0
      || board.get_num_pieces() > TB_LARGEST
      || moves.size() == 0) {
    std::cout << "Coming home early" << std::endl;
    return moves;
  }
  
  std::cout << TB_RESULT_FAILED << std::endl;
  
  unsigned code = tb_probe_root(
    board.get_color_bitboard(kWhite), board.get_color_bitboard(kBlack),
    board.get_piecetype_bitboard(kKing), board.get_piecetype_bitboard(kQueen),
    board.get_piecetype_bitboard(kRook), board.get_piecetype_bitboard(kBishop),
    board.get_piecetype_bitboard(kKnight), board.get_piecetype_bitboard(kPawn),
    board.get_fifty_move_count(), board.get_en_passant(),
    board.get_turn() == kWhite, nullptr);
  
  if (code == TB_RESULT_FAILED) {
    std::cout << "Result failed" << std::endl;
  }
  
  TbRootMoves root_moves;
  int ncode = tb_probe_root_wdl(
    board.get_color_bitboard(kWhite), board.get_color_bitboard(kBlack),
    board.get_piecetype_bitboard(kKing), board.get_piecetype_bitboard(kQueen),
    board.get_piecetype_bitboard(kRook), board.get_piecetype_bitboard(kBishop),
    board.get_piecetype_bitboard(kKnight), board.get_piecetype_bitboard(kPawn),
    board.get_fifty_move_count(), board.get_en_passant(),
    board.get_turn() == kWhite, true, &root_moves);
  
  if (!ncode) {
    std::cout << "Result 2 failed" << std::endl;
  }
  
  for (size_t i = 0; i < root_moves.size; ++i) {
    TbRootMove rm = root_moves.moves[i];
    Move move = pyrrhic_move_to_regular(moves, rm.move);
    std::cout << parse::MoveToString(move) << " score " << rm.tbScore << " rank " << rm.tbRank << std::endl;
  }
  
  return moves;
  /*TbRootMoves root_moves;
  
  int32_t code = tb_probe_root_dtz(
    board.get_color_bitboard(kWhite), board.get_color_bitboard(kBlack),
    board.get_piecetype_bitboard(kKing), board.get_piecetype_bitboard(kQueen),
    board.get_piecetype_bitboard(kRook), board.get_piecetype_bitboard(kBishop),
    board.get_piecetype_bitboard(kKnight), board.get_piecetype_bitboard(kPawn),
    board.get_fifty_move_count(), board.get_en_passant(),
    board.get_turn() == kWhite, board.CountRepetitions() > 1,
    true, &root_moves);
  
  for (size_t i = 0; i < root_moves.size; ++i) {
    TbRootMove rm = root_moves.moves[i];
    Move move = pyrrhic_move_to_regular(moves, rm.move);
    std::cout << parse::MoveToString(move) << " score: " << rm.tbScore << " rank: " << rm.tbRank << std::endl;
  }
  
  if (!code) {
    std::cout << "Some of the queries failed" << std::endl;
  }
  return moves;*/
}

void InitTB(std::string path) {
  tb_init(path.c_str());
}

int32_t MaxPieces() {
  return TB_LARGEST;
}

}

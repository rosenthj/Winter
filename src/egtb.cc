#include "pyrrhic/tbprobe.h"
#include "general/types.h"
#include "board.h"

#include <iostream>

extern int TB_LARGEST;

namespace {

// This is slow but should only get called from root
Move pyrrhic_move_to_regular(std::vector<Move> &moves, uint32_t move) {
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
  return kNullMove;
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
  
  if (board.get_turn() == kBlack) {
    res = 4 - res;
  }
  
  if (res == TB_WIN) {
    return GetTBWinOnMoveScore(board.get_num_made_moves());
  }
  else if (res == TB_LOSS) {
    return GetTBLossOnMoveScore(board.get_num_made_moves());
  }
  return kDrawScore;
}

std::vector<Move> ProbOptimalMoves(Board &board) {
  std::vector<Move> moves = board.GetMoves<kNonQuiescent>();
  if (board.get_castling_rights() != 0
      || board.get_num_pieces() > TB_LARGEST
      || moves.size() == 0) {
    return moves;
  }
  
  uint32_t results[TB_MAX_MOVES];
  
  uint32_t result = tb_probe_root(
    board.get_color_bitboard(kWhite), board.get_color_bitboard(kBlack),
    board.get_piecetype_bitboard(kKing), board.get_piecetype_bitboard(kQueen),
    board.get_piecetype_bitboard(kRook), board.get_piecetype_bitboard(kBishop),
    board.get_piecetype_bitboard(kKnight), board.get_piecetype_bitboard(kPawn),
    board.get_fifty_move_count(), board.get_en_passant(),
    board.get_turn() == kWhite, results);
  
  if (result == TB_RESULT_FAILED
      || TB_GET_WDL(result) == TB_LOSS) {
    return moves;
  }
  else if (TB_GET_WDL(result) == TB_DRAW) {
    std::vector<Move> best_moves;
    for (size_t i = 0; i < TB_MAX_MOVES && results[i] != TB_RESULT_FAILED; ++i) {
      if (TB_GET_WDL(results[i]) == TB_DRAW) {
        best_moves.emplace_back(pyrrhic_move_to_regular(moves, results[i]));
      }
    }
    return best_moves;
  }
  std::vector<Move> best_move;
  best_move.emplace_back(pyrrhic_move_to_regular(moves, result));
  return best_move;
}

void InitTB(std::string path) {
  tb_init(path.c_str());
}

int32_t MaxPieces() {
  return TB_LARGEST;
}

}

#pragma once
#include "./../model/Board/board.hpp"
#include "./../model/Types/MoveStruct.hpp"
#include "./../model/game.hpp"

class SimpleAI {
public:
  static Move getBestMove(const Board &board, PieceColor color, int depth);

private:
  static int minimax(Board board, int depth, int alpha, int beta,
                     bool maximizingPlayer, PieceColor myColor);

  static int evaluateBoard(const Board &board, PieceColor myColor);

  static int getPieceValue(PieceType type);
};
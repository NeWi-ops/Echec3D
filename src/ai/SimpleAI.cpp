#include "SimpleAI.hpp"
#include "./../model/Arbiter/arbiter.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include "../model/Pieces/Base/queen.hpp"

int SimpleAI::getPieceValue(PieceType type) {
  switch (type) {
  case PieceType::Pawn:
    return 10;
  case PieceType::Knight:
    return 30;
  case PieceType::Bishop:
    return 30;
  case PieceType::Rook:
    return 50;
  case PieceType::Queen:
    return 90;
  case PieceType::King:
    return 9000;
  default:
    return 0;
  }
}

static int pawnTable[64] = {
    0,  0,  0,  0,   0,   0,  0,  0,  50, 50, 50,  50, 50, 50,  50, 50,
    10, 10, 20, 30,  30,  20, 10, 10, 5,  5,  10,  25, 25, 10,  5,  5,
    0,  0,  0,  20,  20,  0,  0,  0,  5,  -5, -10, 0,  0,  -10, -5, 5,
    5,  10, 10, -20, -20, 10, 10, 5,  0,  0,  0,   0,  0,  0,   0,  0};

static int knightTable[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0,   0,   0,
    0,   -20, -40, -30, 0,   10,  15,  15,  10,  0,   -30, -30, 5,
    15,  20,  20,  15,  5,   -30, -30, 0,   15,  20,  20,  15,  0,
    -30, -30, 5,   10,  15,  15,  10,  5,   -30, -40, -20, 0,   5,
    5,   0,   -20, -40, -50, -40, -30, -30, -30, -30, -40, -50};

static int bishopTable[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, -10, 0,   0,   0,   0,
    0,   0,   -10, -10, 0,   5,   10,  10,  5,   0,   -10, -10, 5,
    5,   10,  10,  5,   5,   -10, -10, 0,   10,  10,  10,  10,  0,
    -10, -10, 10,  10,  10,  10,  10,  10,  -10, -10, 5,   0,   0,
    0,   0,   5,   -10, -20, -10, -10, -10, -10, -10, -10, -20};

int SimpleAI::evaluateBoard(const Board &board, PieceColor myColor) {
  int score = 0;

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      const Piece *p = board.getPiece(x, y);
      if (p) {

        int val = getPieceValue(p->getType());

        int positionBonus = 0;
        int index = y * 8 + x;

        int tableIndex =
            (p->getColor() == PieceColor::White) ? index : ((7 - y) * 8 + x);

        switch (p->getType()) {
        case PieceType::Pawn:
          positionBonus = pawnTable[tableIndex];
          break;
        case PieceType::Knight:
          positionBonus = knightTable[tableIndex];
          break;
        case PieceType::Bishop:
          positionBonus = bishopTable[tableIndex];
          break;
        default:
          break;
        }

        int pieceScore = val + positionBonus;

        if (p->getColor() == myColor) {
          score += pieceScore;
        } else {
          score -= pieceScore;
        }
      }
    }
  }
  return score;
}

Move SimpleAI::getBestMove(const Board &board, PieceColor color, int depth) {
  std::cout << "IA: Début réflexion (Profondeur " << depth << ")..."
            << std::endl;

  std::vector<Move> allMoves;

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      Coords pos = {x, y};
      const Piece *p = board.getPiece(pos);

      if (p && p->getColor() == color) {
        std::vector<Move> pieceMoves = Arbiter::getLegalMoves(board, pos);
        allMoves.insert(allMoves.end(), pieceMoves.begin(), pieceMoves.end());
      }
    }
  }

  if (allMoves.empty()) {
    std::cout << "IA: Aucun coup possible (Mat ou Pat)" << std::endl;
    return Move{Coords{-1, -1}, Coords{-1, -1}, PieceType::Pawn};
  }

  std::cout << "IA: " << allMoves.size() << " coups possibles à analyser."
            << std::endl;

  Move bestMove = allMoves[0];
  int maxEval = -std::numeric_limits<int>::max();

  int progress = 0;
  for (const auto &move : allMoves) {

    Board copy = board;
    copy.movePiece(move.from, move.to);

    if (move.isPromotion) {
      auto q = std::make_unique<Queen>(color);
      copy.setPieceAt(move.to, *q);
    }

    int eval = minimax(copy, depth - 1, -std::numeric_limits<int>::max(),
                       std::numeric_limits<int>::max(), false, color);

    if (eval > maxEval) {
      maxEval = eval;
      bestMove = move;
    }

    else if (eval == maxEval) {
      if (rand() % 2 == 0) {
        bestMove = move;
      }
    }
  }

  std::cout << std::endl
            << "IA: Terminé. Score final: " << maxEval << std::endl;
  return bestMove;
}

int SimpleAI::minimax(Board board, int depth, int alpha, int beta,
                      bool maximizingPlayer, PieceColor myColor) {

  if (depth == 0) {
    return evaluateBoard(board, myColor);
  }

  PieceColor currentPlayer =
      maximizingPlayer ? myColor : Piece::opposite(myColor);

  std::vector<Move> allMoves;
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      const Piece *p = board.getPiece(x, y);
      if (p && p->getColor() == currentPlayer) {
        auto m = Arbiter::getLegalMoves(board, {x, y});
        allMoves.insert(allMoves.end(), m.begin(), m.end());
      }
    }
  }

  if (allMoves.empty()) {
    return evaluateBoard(board, myColor);
  }

  if (maximizingPlayer) {
    int maxEval = -std::numeric_limits<int>::max();
    for (const auto &move : allMoves) {
      Board copy = board;
      copy.movePiece(move.from, move.to);
      if (move.isPromotion) {
        auto q = std::make_unique<Queen>(currentPlayer);
        copy.setPieceAt(move.to, *q);
      }

      int eval = minimax(copy, depth - 1, alpha, beta, false, myColor);
      maxEval = std::max(maxEval, eval);
      alpha = std::max(alpha, eval);
      if (beta <= alpha)
        break;
    }
    return maxEval;
  } else {
    int minEval = std::numeric_limits<int>::max();
    for (const auto &move : allMoves) {
      Board copy = board;
      copy.movePiece(move.from, move.to);
      if (move.isPromotion) {
        auto q = std::make_unique<Queen>(currentPlayer);
        copy.setPieceAt(move.to, *q);
      }

      int eval = minimax(copy, depth - 1, alpha, beta, true, myColor);
      minEval = std::min(minEval, eval);
      beta = std::min(beta, eval);
      if (beta <= alpha)
        break;
    }
    return minEval;
  }
}
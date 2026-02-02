#include "arbiter.hpp"
#include "./../game.hpp"
#include <algorithm>

std::vector<Move> Arbiter::getLegalMoves(const Board &originalBoard,
                                         Coords start) {
  std::vector<Move> legalMoves;

  const Piece *piece = originalBoard.getPiece(start);

  if (!piece) {
    return {};
  }

  std::vector<Coords> candidates =
      piece->getPseudoLegalMoves(originalBoard, start);

  for (const Coords &target : candidates) {

    std::optional<PieceType> capturedType = std::nullopt;
    if (const Piece *targetPiece = originalBoard.getPiece(target)) {
      capturedType = targetPiece->getType();
    }

    bool isProm = false;
    if (piece->getType() == PieceType::Pawn) {

      bool whiteProm =
          (piece->getColor() == PieceColor::White && target.y == 0);

      bool blackProm =
          (piece->getColor() == PieceColor::Black && target.y == 7);

      if (whiteProm || blackProm) {
        isProm = true;
      }
    }

    Move candidateMove{.from = start,
                       .to = target,
                       .movingPiece = piece->getType(),
                       .capturedPiece = capturedType,
                       .isPromotion = isProm};

    auto tempBoard = originalBoard.clone();

    tempBoard.movePiece(start, target);

    if (!isKingInCheck(tempBoard, piece->getColor())) {
      legalMoves.push_back(candidateMove);
    }
  }

  return legalMoves;
}

bool Arbiter::isKingInCheck(const Board &board, PieceColor kingColor) {

  Coords kingPos = {-1, -1};

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      if (const Piece *p = board.getPiece(x, y)) {
        if (p->getType() == PieceType::King && p->getColor() == kingColor) {
          kingPos = {x, y};
          goto found;
        }
      }
    }
  }

  return true;

found:

  PieceColor enemyColor =
      (kingColor == PieceColor::White) ? PieceColor::Black : PieceColor::White;

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      const Piece *enemy = board.getPiece(x, y);

      if (enemy && enemy->getColor() == enemyColor) {

        Coords enemyPos = {x, y};

        std::vector<Coords> attacks =
            enemy->getPseudoLegalMoves(board, enemyPos);

        if (std::find(attacks.begin(), attacks.end(), kingPos) !=
            attacks.end()) {
          return true;
        }
      }
    }
  }

  return false;
}

bool Arbiter::hasLegalMoves(const Board &board, PieceColor color) {

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      const Piece *p = board.getPiece(x, y);

      if (p && p->getColor() == color) {

        if (!getLegalMoves(board, {x, y}).empty()) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Arbiter::isInsufficientMaterial(const Board &board) {
  int whitePieces = 0;
  int blackPieces = 0;
  int whiteBishopsOrKnights = 0;
  int blackBishopsOrKnights = 0;

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      const Piece *p = board.getPiece(x, y);
      if (p) {

        if (p->getType() == PieceType::Pawn ||
            p->getType() == PieceType::Rook ||
            p->getType() == PieceType::Queen) {
          return false;
        }

        if (p->getColor() == PieceColor::White) {
          whitePieces++;
          if (p->getType() == PieceType::Bishop ||
              p->getType() == PieceType::Knight)
            whiteBishopsOrKnights++;
        } else {
          blackPieces++;
          if (p->getType() == PieceType::Bishop ||
              p->getType() == PieceType::Knight)
            blackBishopsOrKnights++;
        }
      }
    }
  }

  if (whitePieces == 1 && blackPieces == 1)
    return true;

  if (whitePieces == 2 && blackPieces == 1 && whiteBishopsOrKnights == 1)
    return true;
  if (whitePieces == 1 && blackPieces == 2 && blackBishopsOrKnights == 1)
    return true;

  return false;
}
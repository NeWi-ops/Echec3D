#include "./game.hpp"
#include "./Arbiter/arbiter.hpp"
#include "./Fen/FenConverter.hpp"
#include "./Pieces/bishop.hpp"
#include "./Pieces/king.hpp"
#include "./Pieces/knight.hpp"
#include "./Pieces/pawn.hpp"
#include "./Pieces/queen.hpp"
#include "./Pieces/rook.hpp"
#include "./Types/PieceStruct.hpp"
#include <array>
#include <iostream>

Game::Game() {
  this->board = std::make_unique<Board>();
  FenConverter::load(
      *this, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - - 0 1");
}

void Game::reset() {
  this->board = std::make_unique<Board>();
  FenConverter::load(
      *this, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - - 0 1");
  this->m_currentTurn = PieceColor::White;
  this->m_turnCount = 1;
  this->m_state = GameState::InProgress;
  this->m_history.clear();
}

bool Game::playMove(const Move &moveInput) {

  const Piece *p = board->getPiece(moveInput.from);
  if (!p || p->getColor() != m_currentTurn)
    return false;

  std::vector<Move> legalMoves = Arbiter::getLegalMoves(*board, moveInput.from);

  bool isLegal = false;
  Move moveExecuter = moveInput;

  for (const auto &leg : legalMoves) {
    if (leg.to == moveInput.to) {
      isLegal = true;

      moveExecuter.capturedPiece = leg.capturedPiece;
      moveExecuter.isPromotion = leg.isPromotion;

      if (leg.isPromotion && moveInput.promoteTo.has_value()) {
        moveExecuter.promoteTo = moveInput.promoteTo;
      }

      break;
    }
  }

  if (!isLegal)
    return false;

  board->movePiece(moveExecuter.from, moveExecuter.to);

  if (moveExecuter.isPromotion && moveExecuter.promoteTo.has_value()) {

    PieceColor color = board->getPiece(moveExecuter.to)->getColor();
    PieceType typeChoisi = moveExecuter.promoteTo.value();

    std::cout << "Promotion en cours vers le type ID : " << (int)typeChoisi
              << std::endl;

    std::unique_ptr<Piece> promotedPiece;
    switch (typeChoisi) {
    case PieceType::Queen:
      promotedPiece = std::make_unique<Queen>(color);
      break;
    case PieceType::Rook:
      promotedPiece = std::make_unique<Rook>(color);
      break;
    case PieceType::Bishop:
      promotedPiece = std::make_unique<Bishop>(color);
      break;
    case PieceType::Knight:
      promotedPiece = std::make_unique<Knight>(color);
      break;
    default:
      promotedPiece = std::make_unique<Queen>(color);
      break;
    }

    if (promotedPiece) {
      board->setPieceAt(moveExecuter.to, *promotedPiece);
    }
  }

  m_history.push_back(moveExecuter);

  switchTurn();
  return true;
}

void Game::switchTurn() {
  m_currentTurn = Piece::opposite(m_currentTurn);

  bool inCheck = Arbiter::isKingInCheck(*board, m_currentTurn);
  bool hasMoves = Arbiter::hasLegalMoves(*board, m_currentTurn);

  bool isDrawMat = Arbiter::isInsufficientMaterial(*board);

  if (inCheck && !hasMoves) {
    m_state = GameState::Checkmate;
  } else if (inCheck) {
    m_state = GameState::Check;
  } else if (!hasMoves) {
    m_state = GameState::Stalemate;
  } else if (isDrawMat) {
    m_state = GameState::Stalemate;
  } else {
    m_state = GameState::InProgress;
  }
}

void Game::undoLastMove() {

  if (m_history.empty())
    return;

  Move lastMove = m_history.back();
  m_history.pop_back();

  switchTurn();

  m_turnCount -= 2;

  board->movePiece(lastMove.to, lastMove.from);

  if (lastMove.isPromotion) {

    auto pawn = std::make_unique<Pawn>(m_currentTurn);
    board->setPieceAt(lastMove.from, *pawn);
  }

  if (lastMove.capturedPiece.has_value()) {
    PieceType type = lastMove.capturedPiece.value();
    PieceColor enemyColor = Piece::opposite(m_currentTurn);

    std::unique_ptr<Piece> resurrected;

    switch (type) {
    case PieceType::Pawn:
      resurrected = std::make_unique<Pawn>(enemyColor);
      break;
    case PieceType::Rook:
      resurrected = std::make_unique<Rook>(enemyColor);
      break;
    case PieceType::Knight:
      resurrected = std::make_unique<Knight>(enemyColor);
      break;
    case PieceType::Bishop:
      resurrected = std::make_unique<Bishop>(enemyColor);
      break;
    case PieceType::Queen:
      resurrected = std::make_unique<Queen>(enemyColor);
      break;

    case PieceType::King:
      resurrected = std::make_unique<King>(enemyColor);
      break;
    }

    if (resurrected) {
      board->setPieceAt(lastMove.to, *resurrected);
    }
  }

  bool inCheck = Arbiter::isKingInCheck(*board, m_currentTurn);
  bool hasMoves = Arbiter::hasLegalMoves(*board, m_currentTurn);

  if (inCheck && !hasMoves)
    m_state = GameState::Checkmate;
  else if (inCheck)
    m_state = GameState::Check;
  else if (!hasMoves)
    m_state = GameState::Stalemate;
  else
    m_state = GameState::InProgress;
}
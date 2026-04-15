#include "./game.hpp"
#include "./Arbiter/arbiter.hpp"
#include "./Factory/factory.hpp"
#include "./Fen/FenConverter.hpp"
#include "./Pieces/bishop.hpp"
#include "./Pieces/king.hpp"
#include "./Pieces/knight.hpp"
#include "./Pieces/pawn.hpp"
#include "./Pieces/queen.hpp"
#include "./Pieces/rook.hpp"
#include "./Types/PieceStruct.hpp"
#include "../utils/StatsLogger.hpp"
#include <array>
#include <iostream>

Game::Game() : board(std::make_unique<Board>()), m_currentAppState(AppState::MAIN_MENU) {
  PieceFactory::registerPiece(
      "r", [](PieceColor c) { return std::make_unique<Rook>(c); });
  PieceFactory::registerPiece(
      "n", [](PieceColor c) { return std::make_unique<Knight>(c); });
  PieceFactory::registerPiece(
      "b", [](PieceColor c) { return std::make_unique<Bishop>(c); });
  PieceFactory::registerPiece(
      "q", [](PieceColor c) { return std::make_unique<Queen>(c); });
  PieceFactory::registerPiece(
      "k", [](PieceColor c) { return std::make_unique<King>(c); });
  PieceFactory::registerPiece(
      "p", [](PieceColor c) { return std::make_unique<Pawn>(c); });

  // Wait for user to click "NEW GAME". Start with a classic board.
  FenConverter::load(
      *this, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  loadConfig();
}

void Game::reset() {
  this->board = std::make_unique<Board>();
  this->m_currentTurn = PieceColor::White;
  this->m_turnCount = 1;
  this->board->setTurnCount(m_turnCount);
  this->m_state = GameState::InProgress;
  this->m_history.clear();

  // Clean Slate: Reset hazards and alerts
  this->m_cursedSquare = {-1, -1};
  this->m_curseDuration = 0;
  this->m_curseCooldown = m_cursedSquareEnabled ? RandomGenerator::generateGeometric(0.4) : 0;
  
  m_lightningManager.dismissStrike();
  m_lightningManager.resetTimer();
}

std::vector<Move> Game::getValidMoves(Coords pos) const {
    const Piece *piece = board->getPiece(pos);
    if (!piece) return {};

    if (m_curseDuration > 0 && pos.x == m_cursedSquare.x && pos.y == m_cursedSquare.y) {
        if (piece->getType() != PieceType::King) {
            return {}; // Return an empty vector/list. The piece is rooted.
        }
    }
    return Arbiter::getLegalMoves(*board, pos);
}

bool Game::playMove(const Move &moveInput) {
  m_lightningManager.dismissStrike();

  const Piece *p = board->getPiece(moveInput.from);
  if (!p || p->getColor() != m_currentTurn)
    return false;

  if (m_curseDuration > 0 && moveInput.from.x == m_cursedSquare.x && moveInput.from.y == m_cursedSquare.y) {
      if (p->getType() != PieceType::King) {
          return false;
      }
  }

  std::vector<Move> legalMoves = getValidMoves(moveInput.from);

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
  m_turnCount++;
  board->setTurnCount(m_turnCount);

  if (m_lightningEnabled) {
    m_lightningManager.update(*this);
  }

  if (m_cursedSquareEnabled) {
    if (m_curseDuration > 0) {
        // Phase 1: Curse is active
        m_curseDuration--;
        if (m_curseDuration <= 0) {
            // Curse dies. Remove it and start cooldown.
            m_cursedSquare = {-1, -1};
            m_curseCooldown = RandomGenerator::generateGeometric(0.4);
            StatsLogger::instance().logCursedSquare(1, m_curseCooldown);
        }
    } else if (m_curseCooldown > 0) {
        // Phase 2: Board is safe, waiting for next curse
        m_curseCooldown--;
        if (m_curseCooldown <= 0) {
            // Cooldown ends. Spawn new curse.
            m_cursedSquare.x = RandomGenerator::generateUniformDiscrete(8);
            m_cursedSquare.y = RandomGenerator::generateUniformDiscrete(8);
            m_curseDuration = RandomGenerator::generateGeometric(0.1);
            StatsLogger::instance().logCursedSquare(0, m_curseDuration);
        }
    }
  }

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
    
  if (m_lightningManager.hasStruckRecently())
    return;

  Move lastMove = m_history.back();
  m_history.pop_back();

  switchTurn();

  m_turnCount -= 2;
  board->setTurnCount(m_turnCount);

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

    case PieceType::Custom:
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

void Game::newGame(std::string fen) {
  this->board = std::make_unique<Board>();
  FenConverter::load(*this, fen);
  this->m_currentTurn = PieceColor::White;
  this->m_turnCount = 1;
  this->board->setTurnCount(m_turnCount);
  this->m_state = GameState::InProgress;
  this->m_history.clear();
}

void Game::loadConfig() {
  std::ifstream file("config.txt");
  if (file.is_open()) {
    int lightning = 1;
    int cursed = 1;
    file >> lightning >> cursed;
    m_lightningEnabled = (lightning != 0);
    m_cursedSquareEnabled = (cursed != 0);
    file.close();
    std::cout << "Config loaded: Lightning=" << m_lightningEnabled
              << " CursedSquare=" << m_cursedSquareEnabled << '\n';
  }

  // Generate initial cursed cooldown only if feature is enabled
  if (m_cursedSquareEnabled) {
    m_curseCooldown = RandomGenerator::generateGeometric(0.4);
  } else {
    m_cursedSquare = {-1, -1};
    m_curseDuration = 0;
    m_curseCooldown = 0;
  }
}

void Game::saveConfig() {
  std::ofstream file("config.txt");
  if (file.is_open()) {
    file << (m_lightningEnabled ? 1 : 0) << " " << (m_cursedSquareEnabled ? 1 : 0);
    file.close();
    std::cout << "Config saved." << std::endl;
  }
}
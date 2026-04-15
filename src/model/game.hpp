#pragma once

#include "./Types/ColorEnum.hpp"
#include "./Types/GameStateEnum.hpp"
#include "Board/board.hpp"
#include "../utils/LightningManager.hpp"
#include "../utils/RandomGenerator.hpp"
#include <array>
#include <iostream>
#include <fstream>
#include <string>

enum class AppState { MAIN_MENU, PLAYING };

class Game {
private:
  std::unique_ptr<Board> board;
  PieceColor m_currentTurn = PieceColor::White;
  int m_turnCount = 1;
  GameState m_state = GameState::InProgress;
  std::vector<Move> m_history;
  LightningManager m_lightningManager;
  Coords m_cursedSquare = {-1, -1};
  int m_curseDuration = 0;
  int m_curseCooldown = 0;

  AppState m_currentAppState = AppState::MAIN_MENU;

public:
  bool m_lightningEnabled = false;
  bool m_cursedSquareEnabled = false;
  bool m_isFirstLaunch = true;

  AppState getAppState() const { return m_currentAppState; }
  void setAppState(AppState state) { m_currentAppState = state; }

  bool isLightningEnabled() const { return m_lightningEnabled; }
  void setLightningEnabled(bool v) { m_lightningEnabled = v; }
  bool isCursedSquareEnabled() const { return m_cursedSquareEnabled; }
  void setCursedSquareEnabled(bool v) { m_cursedSquareEnabled = v; }

  void loadConfig();
  void saveConfig();



  Game();
  ~Game() = default;

  bool playMove(const Move &move);
  std::vector<Move> getValidMoves(Coords pos) const;

  void switchTurn();

  void reset();

  PieceColor getCurrentTurn() const { return m_currentTurn; }
  Board &getBoard() const { return *board; }
  int getTurnCount() const { return m_turnCount; }
  GameState getGameState() const { return m_state; }

  void undoLastMove();
  const std::vector<Move> &getHistory() const { return m_history; }
  void clearHistory() { m_history.clear(); }
  LightningManager& getLightningManager() { return m_lightningManager; }

  const Coords& getCursedSquare() const { return m_cursedSquare; }
  int getCurseDuration() const { return m_curseDuration; }
  int getCurseCooldown() const { return m_curseCooldown; }

  void clearCursedSquare() {
    m_cursedSquare = {-1, -1};
    m_curseDuration = 0;
    m_curseCooldown = m_cursedSquareEnabled
        ? RandomGenerator::generateGeometric(0.4)
        : 0;
  }

  void resetState(std::unique_ptr<Board> newBoard, PieceColor turn) {
    this->board = std::move(newBoard);
    this->m_currentTurn = turn;

    this->m_history.clear();

    m_currentTurn = Piece::opposite(m_currentTurn);
    switchTurn();
  }

  void newGame(std::string fen);
};

#pragma once

#include "./../types/ColorEnum.hpp"
#include "./../types/GameStateEnum.hpp"
#include "./../types/GameVarEnum.hpp"
#include "Board/board.hpp"
#include <array>
#include <iostream>

class Game {
private:
  std::unique_ptr<Board> board;
  PieceColor m_currentTurn = PieceColor::White;
  int m_turnCount = 1;
  GameState m_state = GameState::InProgress;
  std::vector<Move> m_history;

public:
  Game();
  ~Game() = default;

  bool playMove(const Move &move);

  void switchTurn();

  void reset();

  PieceColor getCurrentTurn() const { return m_currentTurn; }
  Board &getBoard() const { return *board; }
  int getTurnCount() const { return m_turnCount; }
  GameState getState() const { return m_state; }

  void undoLastMove();
  const std::vector<Move> &getHistory() const { return m_history; }

  void resetState(std::unique_ptr<Board> newBoard, PieceColor turn) {
    this->board = std::move(newBoard);
    this->m_currentTurn = turn;

    this->m_history.clear();

    m_currentTurn = Piece::opposite(m_currentTurn);
    switchTurn();
  }

  void loadVariant(GameVariant variantName);
};

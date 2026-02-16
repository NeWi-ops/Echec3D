#pragma once
#include "./../model/Arbiter/arbiter.hpp"
#include "./../model/Types/CoordsStruct.hpp"
#include "./../model/game.hpp"
#include "./Scene3D.hpp"
#include "imgui.h"
#include <optional>

class GameRenderer {
public:
  void render(Game &game, Scene3D *scene3D);

  void handleSquareClick(Coords pos, Game &game, Scene3D *scene3D);
  
  ImFont *chessFont = nullptr;

  std::optional<Coords> selectedCase = std::nullopt;

  std::vector<Coords> possibleMoves;

private:

  std::optional<Move> pendingPromotionMove = std::nullopt;
  void drawPromotionPopup(Game &game);

  void drawBoard(Game &game, Scene3D *scene3D);
  std::string getPieceLabel(const Piece *p);

  void drawControlPanel(Game &game, Scene3D *scene3D);

  void drawHistoryWindow(Game &game);
  std::string moveToString(const Move &move);
  std::string coordToString(Coords c);

  void drawStatusWindow(Game &game);

  bool shouldOpenPromotion = false;

  
};
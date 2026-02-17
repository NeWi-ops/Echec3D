#include "gameRenderer.hpp"
#include "./../ai/SimpleAI.hpp"
#include "./../customs/customsPiece.hpp"
#include "./../model/Fen/FenConverter.hpp"
#include "./../utils/scenarioRegister.hpp"
#include "./Scene3D.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

void GameRenderer::render(Game &game, Scene3D *scene3D) {
  ImGuiIO &io = ImGui::GetIO();
  float screenW = io.DisplaySize.x;
  float screenH = io.DisplaySize.y;

  float leftColWidth = screenW * 0.20f;
  float boardHeight = leftColWidth;
  float statusHeight = 240.0f;
  float controlsHeight = screenH - boardHeight - statusHeight;
  float rightColWidth = screenW * 0.15f;

  ImGuiWindowFlags staticFlags =
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(leftColWidth, boardHeight));

  if (ImGui::Begin("Plateau", nullptr,
                   staticFlags | ImGuiWindowFlags_NoTitleBar)) {

    drawBoard(game, scene3D);

    drawPromotionPopup(game);
  }
  ImGui::End();

  ImGui::SetNextWindowPos(ImVec2(0, boardHeight));
  ImGui::SetNextWindowSize(ImVec2(leftColWidth, statusHeight));

  if (ImGui::Begin("Status", nullptr, staticFlags)) {
    drawStatusWindow(game);
  }
  ImGui::End();

  ImGui::SetNextWindowPos(ImVec2(0, boardHeight + statusHeight));
  ImGui::SetNextWindowSize(ImVec2(leftColWidth, controlsHeight));

  if (ImGui::Begin("Commandes", nullptr, staticFlags)) {

    drawControlPanel(game, scene3D);
  }
  ImGui::End();

  ImGui::SetNextWindowPos(ImVec2(screenW - rightColWidth, 0));
  ImGui::SetNextWindowSize(ImVec2(rightColWidth, screenH));

  if (ImGui::Begin("Historique", nullptr, staticFlags)) {
    drawHistoryWindow(game);
  }
  ImGui::End();
}

void GameRenderer::drawBoard(Game &game, Scene3D *scene3D) {
  const Board &board = game.getBoard();
  GameState state = game.getState();
  PieceColor currentTurn = game.getCurrentTurn();

  float availX = ImGui::GetContentRegionAvail().x;
  float cellSize = availX / 8.0f;

  float fontBaseSize = 40.0f;
  float fontScale = (cellSize * 1.2f) / fontBaseSize;

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

  bool triggerPromotionPopup = false;

  if (ImGui::BeginTable("Grid", 8,
                        ImGuiTableFlags_NoPadInnerX |
                            ImGuiTableFlags_NoPadOuterX)) {
    for (int y = 0; y < 8; ++y) {
      ImGui::TableNextRow();
      ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, 0);

      for (int x = 0; x < 8; ++x) {
        ImGui::TableSetColumnIndex(x);
        Coords pos{x, y};
        int squareId = y * 8 + x;
        ImGui::PushID(squareId);

        const Piece *p = board.getPiece(pos);
        bool isDark = (x + y) % 2 != 0;
        ImVec4 bgCol = isDark ? ImVec4(0.45f, 0.45f, 0.45f, 1.0f)
                              : ImVec4(0.8f, 0.8f, 0.8f, 1.0f);

        if (p && p->getType() == PieceType::King &&
            p->getColor() == currentTurn) {
          if (state == GameState::Check || state == GameState::Checkmate)
            bgCol = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
        } else if (selectedCase.has_value() && selectedCase.value() == pos) {
          bgCol = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
        } else if (std::find(possibleMoves.begin(), possibleMoves.end(), pos) !=
                   possibleMoves.end()) {
          if (p)
            bgCol = ImVec4(0.9f, 0.4f, 0.4f, 0.8f);
          else
            bgCol = ImVec4(0.4f, 0.7f, 1.0f, 0.8f);
        }

        ImGui::PushStyleColor(ImGuiCol_Button, bgCol);
        ImGui::PushStyleColor(
            ImGuiCol_ButtonHovered,
            ImVec4(bgCol.x + 0.1f, bgCol.y + 0.1f, bgCol.z + 0.1f, 1.0f));
        ImGui::PushStyleColor(
            ImGuiCol_ButtonActive,
            ImVec4(bgCol.x - 0.1f, bgCol.y - 0.1f, bgCol.z - 0.1f, 1.0f));

        if (p) {
          if (p->getColor() == PieceColor::Black)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
          else
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
        } else {
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0));
        }

        std::string label = p ? getPieceLabel(p) : " ";
        if (chessFont)
          ImGui::PushFont(chessFont);
        ImGui::SetWindowFontScale(fontScale);

        if (ImGui::Button(label.c_str(), ImVec2(cellSize, cellSize))) {

          if (!selectedCase.has_value()) {
            if (p && p->getColor() == currentTurn) {
              selectedCase = pos;
              possibleMoves.clear();
              auto moves = Arbiter::getLegalMoves(board, pos);
              for (const auto &m : moves)
                possibleMoves.push_back(m.to);
            }
          } else {
            Coords start = selectedCase.value();
            if (start == pos) {
              selectedCase = std::nullopt;
              possibleMoves.clear();
            } else {

              PieceType type = board.getPiece(start)->getType();
              Move moveAttempt(start, pos, type);

              bool isPawn = (type == PieceType::Pawn);
              bool isLastRow = (pos.y == 0 || pos.y == 7);

              if (isPawn && isLastRow) {

                moveAttempt.isPromotion = true;
                pendingPromotionMove = moveAttempt;
                shouldOpenPromotion = true;

                triggerPromotionPopup = true;
              } else {

                if (game.playMove(moveAttempt)) {

                  if (scene3D) {
                    PieceColor movedColor =
                        (game.getCurrentTurn() == PieceColor::White)
                            ? PieceColor::Black
                            : PieceColor::White;
                    scene3D->triggerMoveAnimation(
                        moveAttempt.from, moveAttempt.to,
                        moveAttempt.movingPiece, movedColor);
                  }
                  selectedCase = std::nullopt;
                  possibleMoves.clear();
                } else {

                  if (p && p->getColor() == currentTurn) {
                    selectedCase = pos;
                    possibleMoves.clear();
                    auto moves = Arbiter::getLegalMoves(board, pos);
                    for (const auto &m : moves)
                      possibleMoves.push_back(m.to);
                  } else {
                    selectedCase = std::nullopt;
                    possibleMoves.clear();
                  }
                }
              }
            }
          }
        }

        ImGui::SetWindowFontScale(1.0f);
        if (chessFont)
          ImGui::PopFont();
        ImGui::PopStyleColor(4);
        ImGui::PopID();
      }
    }
    ImGui::EndTable();
  }
  ImGui::PopStyleVar(2);

  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) &&
      ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {

    selectedCase = std::nullopt;
    possibleMoves.clear();
  }

  if (triggerPromotionPopup) {
    ImGui::OpenPopup("Choix Promotion");
  }
}

std::string GameRenderer::getPieceLabel(const Piece *p) {
  if (!p)
    return "";

  if (p->getType() == PieceType::Custom) {
    // On convertit le pointeur générique en pointeur spécifique
    const auto *customP = dynamic_cast<const CustomPiece *>(p);

    if (customP) {
      return customP->getIcon(); // On a accès à la méthode !
    }
    return "?"; // Sécurité si le cast échoue
  }

  switch (p->getType()) {
  case PieceType::Pawn:
    return "♟";
  case PieceType::Rook:
    return "♜";
  case PieceType::Knight:
    return "♞";
  case PieceType::Bishop:
    return "♝";
  case PieceType::Queen:
    return "♛";
  case PieceType::King:
    return "♚";
  }
  return "?";
}

void GameRenderer::drawPromotionPopup(Game &game) {

  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

  if (ImGui::BeginPopupModal("Choix Promotion", &shouldOpenPromotion,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("En quoi voulez-vous transformer votre pion ?");
    ImGui::Separator();

    auto selectPiece = [&](PieceType type, const char *label) {
      if (ImGui::Button(label, ImVec2(120, 0))) {

        if (pendingPromotionMove.has_value()) {
          Move finalMove = pendingPromotionMove.value();

          finalMove.promoteTo = type;

          if (game.playMove(finalMove)) {
          }

          selectedCase = std::nullopt;
          possibleMoves.clear();
          pendingPromotionMove = std::nullopt;
          shouldOpenPromotion = false;
          ImGui::CloseCurrentPopup();
        }
      }
    };

    selectPiece(PieceType::Queen, "Reine");
    selectPiece(PieceType::Rook, "Tour");
    selectPiece(PieceType::Bishop, "Fou");
    selectPiece(PieceType::Knight, "Cavalier");

    ImGui::EndPopup();
  }
}

std::string GameRenderer::coordToString(Coords c) {

  char col = 'a' + c.x;

  int row = 8 - c.y;

  std::stringstream ss;
  ss << col << row;
  return ss.str();
}

std::string GameRenderer::moveToString(const Move &m) {
  std::stringstream ss;

  ss << coordToString(m.from);

  ss << (m.capturedPiece.has_value() ? "x" : "-");

  ss << coordToString(m.to);

  if (m.isPromotion && m.promoteTo.has_value()) {
    switch (m.promoteTo.value()) {
    case PieceType::Queen:
      ss << "=D";
      break;
    case PieceType::Rook:
      ss << "=T";
      break;
    case PieceType::Bishop:
      ss << "=F";
      break;
    case PieceType::Knight:
      ss << "=C";
      break;
    default:
      break;
    }
  }

  return ss.str();
}

void GameRenderer::drawHistoryWindow(Game &game) {

  const auto &history = game.getHistory();

  if (ImGui::BeginTable("HistoryTable", 3,
                        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_ScrollY)) {

    ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 30.0f);
    ImGui::TableSetupColumn("Blancs");
    ImGui::TableSetupColumn("Noirs");
    ImGui::TableHeadersRow();

    int turnCount = 1;
    for (size_t i = 0; i < history.size(); ++i) {

      if (i % 2 == 0) {
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d.", turnCount);

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", moveToString(history[i]).c_str());
      }

      else {
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%s", moveToString(history[i]).c_str());

        turnCount++;
      }
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
      ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndTable();
  }
}

void GameRenderer::drawControlPanel(Game &game, Scene3D *scene3D) {
  ImGui::Text("Actions :");
  ImGui::Separator();
  if (ImGui::Button("Annuler le coup", ImVec2(-1, 30))) {
    game.undoLastMove();
    selectedCase = std::nullopt;
    possibleMoves.clear();
    pendingPromotionMove = std::nullopt;
  }

  if (ImGui::Button("Faire jouer l'IA", ImVec2(-1, 30))) {

    Move bestMove =
        SimpleAI::getBestMove(game.getBoard(), game.getCurrentTurn(), 3);

    std::cout << "UI: L'IA propose le coup : " << bestMove.from.x << ","
              << bestMove.from.y << " -> " << bestMove.to.x << ","
              << bestMove.to.y << std::endl;

    if (bestMove.isPromotion) {
      bestMove.promoteTo = PieceType::Queen;
    }

    bool success = game.playMove(bestMove);

    if (success) {
      std::cout << "UI: Coup accepté et joué !" << std::endl;

      if (scene3D) {

        PieceColor movedColor = (game.getCurrentTurn() == PieceColor::White)
                                    ? PieceColor::Black
                                    : PieceColor::White;

        scene3D->triggerMoveAnimation(bestMove.from, bestMove.to,
                                      bestMove.movingPiece, movedColor);
      }

      selectedCase = std::nullopt;
      possibleMoves.clear();

    } else {
      std::cout << "UI: ERREUR - Le coup a été REJETÉ par game.playMove()"
                << std::endl;
    }
  }

  // if (ImGui::Button("Recommencer la partie", ImVec2(-1, 30))) {
  //   game.reset();
  //   game.newGame("rnbqkbnr/"
  //                "(Paladin:B)(Paladin:B)(Paladin:B)(Paladin:B)(Paladin:B)("
  //                "Paladin:B)(Paladin:B)(Paladin:B)/8/8/8/8/"
  //                "(Paladin:W)(Paladin:W)(Paladin:W)(Paladin:W)(Paladin:W)("
  //                "Paladin:W)(Paladin:W)(Paladin:W)/RNBQKBNR w KQkq - 0 1");
  //   selectedCase = std::nullopt;
  //   possibleMoves.clear();
  // }

  // --- 1. LE BOUTON DÉCLENCHEUR ---
  if (ImGui::Button("Nouvelle Partie")) {
    ImGui::OpenPopup("Choix du Mode"); // Ouvre la popup par son ID
  }

  // --- 2. LA POPUP MODALE ---
  // "AlwaysAutoResize" permet à la fenêtre de s'adapter au contenu
  bool open = true;
  if (ImGui::BeginPopupModal("Choix du Mode", &open,
                             ImGuiWindowFlags_AlwaysAutoResize)) {

    ImGui::Text("Choisissez une configuration de depart :");
    ImGui::Separator();

    // On affiche le nom en gras
    ImGui::PushStyleColor(ImGuiCol_Text,
                          ImVec4(0.4f, 0.8f, 1.0f, 1.0f)); // Vert clair
    if (ImGui::Button("Default", ImVec2(200, 0))) {        // Bouton large

      // ACTION : Charger le FEN et fermer la popup
      game.newGame("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      ImGui::CloseCurrentPopup();
    }
    ImGui::PopStyleColor();

    // On affiche la description à côté ou en dessous
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("%s", "Jeu de base");
    }

    // On boucle sur tous les presets définis plus haut
    for (const auto &preset : ScenarioRegistry::instance().getAll()) {

      // On affiche le nom en gras
      ImGui::PushStyleColor(ImGuiCol_Text,
                            ImVec4(0.4f, 1.0f, 0.4f, 1.0f));    // Vert clair
      if (ImGui::Button(preset.name.c_str(), ImVec2(200, 0))) { // Bouton large

        // ACTION : Charger le FEN et fermer la popup
        game.newGame(preset.fen);
        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor();

      // On affiche la description à côté ou en dessous
      ImGui::SameLine();
      ImGui::TextDisabled("(?)");
      if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", preset.desc.c_str());
      }
    }

    ImGui::Separator();

    // Bouton Annuler
    if (ImGui::Button("Annuler", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  // 333333333333333333333

  ImGui::Separator();

  if (ImGui::Button("Copier FEN", ImVec2(-1, 30))) {

    std::string fen = FenConverter::save(game);
    ImGui::SetClipboardText(fen.c_str());
  }

  static char fenBuffer[256] = "";
  ImGui::InputText("FEN", fenBuffer, 256);

  if (ImGui::Button("Charger FEN", ImVec2(-1, 30))) {
    std::string fenStr(fenBuffer);
    if (!fenStr.empty()) {

      FenConverter::load(game, fenStr);

      selectedCase = std::nullopt;
      possibleMoves.clear();
    }
  }
}

void GameRenderer::drawStatusWindow(Game &game) {
  GameState state = game.getState();
  PieceColor turn = game.getCurrentTurn();

  float windowWidth = ImGui::GetContentRegionAvail().x;
  float textWidth;

  std::string tourStr =
      (turn == PieceColor::White) ? "Tour : BLANCS" : "Tour : NOIRS";

  ImVec2 textSize = ImGui::CalcTextSize(tourStr.c_str());
  ImGui::SetCursorPosX((windowWidth - textSize.x) * 0.5f);
  ImGui::Text("%s", tourStr.c_str());

  ImGui::Separator();

  if (state == GameState::Check) {
    std::string txt = "!!! ECHEC !!!";
    textSize = ImGui::CalcTextSize(txt.c_str());
    ImGui::SetCursorPosX((windowWidth - textSize.x) * 0.5f);
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", txt.c_str());
  } else if (state == GameState::Checkmate) {
    ImGui::SetWindowFontScale(1.5f);
    std::string txt = "ECHEC ET MAT !";

    textSize = ImGui::CalcTextSize(txt.c_str());
    ImGui::SetCursorPosX((windowWidth - textSize.x) * 0.5f);
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", txt.c_str());

    std::string winner = (turn == PieceColor::White) ? "Les NOIRS gagnent"
                                                     : "Les BLANCS gagnent";
    textSize = ImGui::CalcTextSize(winner.c_str());
    ImGui::SetCursorPosX((windowWidth - textSize.x) * 0.5f);
    ImGui::TextColored(ImVec4(0, 1, 0, 1), "%s", winner.c_str());

    ImGui::SetWindowFontScale(1.0f);
  } else if (state == GameState::Stalemate) {
    std::string txt = "PAT (Match Nul)";
    textSize = ImGui::CalcTextSize(txt.c_str());
    ImGui::SetCursorPosX((windowWidth - textSize.x) * 0.5f);
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", txt.c_str());
  }
}
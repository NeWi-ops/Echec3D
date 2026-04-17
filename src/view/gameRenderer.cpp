#include "gameRenderer.hpp"
#include "./../ai/SimpleAI.hpp"
#include "./../customs/customsPiece.hpp"
#include "./../customs/paladin.hpp"
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

  LightningManager& lm = game.getLightningManager();
  auto poppedAnims = lm.popAnimations();
  if (scene3D) {
      for (const auto& anim : poppedAnims) {
          scene3D->triggerPathAnimation(anim.path, anim.type, anim.color);
      }
  }

  if (lm.hasStruckRecently()) {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
    if (ImGui::Begin("!!! WEATHER ALERT !!!", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); 
        Coords loc = lm.getLastEpicenter();
        std::string sq = coordToString(loc);
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("⚡ LIGHTNING STRUCK AT [%c%c]!", toupper(sq[0]), sq[1]);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
        
        ImGui::Spacing(); ImGui::Spacing();
        if (ImGui::Button("Dismiss", ImVec2(-1, 30))) {
            lm.dismissStrike();
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
  }

  float flash = lm.getFlashAlpha();
  if (flash > 0.0f) {
      ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
      ImVec2 p_min = ImVec2(0.0f, 0.0f);
      ImVec2 p_max = ImGui::GetIO().DisplaySize;
      ImU32 color = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, flash));
      draw_list->AddRectFilled(p_min, p_max, color);
  }
}

void GameRenderer::drawBoard(Game &game, Scene3D *scene3D) {
  const Board &board = game.getBoard();
  GameState state = game.getGameState();
  PieceColor currentTurn = game.getCurrentTurn();

  float availX = ImGui::GetContentRegionAvail().x;
  float cellSize = availX / 8.0f;

  float fontBaseSize = 40.0f;
  float fontScale = (cellSize * 1.2f) / fontBaseSize;

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

  bool triggerPromotionPopup = shouldOpenPromotion;

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

        LightningManager& lm = game.getLightningManager();
        if (game.isCursedSquareEnabled() && game.getCurseDuration() > 0 && game.getCursedSquare() == pos) {
          bgCol = ImColor(150, 0, 200, 255);
        } else if (lm.hasStruckRecently() && lm.getLastEpicenter() == pos) {
          bgCol = ImVec4(0.0f, 0.0f, 0.8f, 0.9f); 
        } else if (lm.hasStruckRecently() && std::find(lm.getTargetHighlights().begin(), lm.getTargetHighlights().end(), pos) != lm.getTargetHighlights().end()) {
          bgCol = ImVec4(1.0f, 1.0f, 0.0f, 0.9f); 
        } else if (p && p->getType() == PieceType::King &&
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
          if (handleSquareClick(pos, game, scene3D)) {
            triggerPromotionPopup = true;
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
    const auto *customP = dynamic_cast<const CustomPiece *>(p);

    if (customP) {
      return customP->getIcon(); 
    }
    return "?"; 
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
  case PieceType::Custom:
    return "C";
  }
  return "?";
}

void GameRenderer::drawPromotionPopup(Game &game) {

  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

  if (ImGui::BeginPopupModal("Choix Promotion", nullptr,
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

    ImGui::Separator();

    if (ImGui::Button("Annuler", ImVec2(120, 0))) {
      pendingPromotionMove = std::nullopt;
      shouldOpenPromotion = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
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
      } else {
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
  if (game.m_isFirstLaunch) {
      ImGui::OpenPopup("Game Configuration");
      game.m_isFirstLaunch = false;
  }

  ImGui::Text("Actions :");
  ImGui::Separator();
  
  bool disableUndo = game.getLightningManager().hasStruckRecently() || game.getHistory().empty();
  if (disableUndo) ImGui::BeginDisabled();
  
  if (ImGui::Button("Annuler le coup", ImVec2(-1, 30))) {
    if (!game.getLightningManager().hasStruckRecently()) {
        game.undoLastMove();
        selectedCase = std::nullopt;
        possibleMoves.clear();
        pendingPromotionMove = std::nullopt;
    }
  }
  
  if (disableUndo) ImGui::EndDisabled();

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
        if (game.getHistory().back().capturedPiece.has_value()) {
            PieceColor capCol = game.getCurrentTurn(); 
            glm::vec4 expColor = (capCol == PieceColor::White) ? glm::vec4(1.0f, 0.9f, 0.8f, 1.0f) : glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
            scene3D->spawnExplosion(glm::vec3(bestMove.to.x, 0.5f, bestMove.to.y), expColor);
        }

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

  ImGui::Separator();
  ImGui::Spacing();

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.85f, 0.3f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.6f, 0.15f, 1.0f));

  if (ImGui::Button("NEW GAME", ImVec2(-1, 35))) {
    ImGui::OpenPopup("Game Configuration");
  }
  ImGui::PopStyleColor(3);

  if (ImGui::BeginPopupModal("Game Configuration", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "GAME SETTINGS");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("CHOOSE GAME MODE:");
    {
      const auto& scenarios = ScenarioRegistry::instance().getAll();
      
      if (ImGui::RadioButton("Classic", selectedGameMode == 0)) {
        selectedGameMode = 0;
      }
      
      for (int i = 0; i < static_cast<int>(scenarios.size()); ++i) {
        bool isSelected = (selectedGameMode == i + 1);
        if (ImGui::RadioButton(scenarios[i].name.c_str(), isSelected)) {
          selectedGameMode = i + 1;
        }
        if (ImGui::IsItemHovered()) {
          ImGui::SetTooltip("%s", scenarios[i].desc.c_str());
        }
      }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("RANDOM EVENTS:");
    ImGui::Checkbox("Lightning Strikes", &game.m_lightningEnabled);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Unpredictable strikes that wipe move history.");
    }
    ImGui::Checkbox("Cursed Squares", &game.m_cursedSquareEnabled);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Squares that root pieces for a duration.");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float windowWidth = ImGui::GetWindowSize().x;
    float btnWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

    if (ImGui::Button("CANCEL", ImVec2(btnWidth, 35))) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.85f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.6f, 0.15f, 1.0f));

    if (ImGui::Button("START GAME", ImVec2(btnWidth, 35))) {
      game.reset();

      const auto& scenarios = ScenarioRegistry::instance().getAll();
      if (selectedGameMode == 0) {
        game.newGame("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      } else {
        int idx = selectedGameMode - 1;
        if (idx >= 0 && idx < static_cast<int>(scenarios.size())) {
          game.newGame(scenarios[idx].fen);
        }
      }
      
      game.setAppState(AppState::PLAYING);
      game.saveConfig();

      selectedCase = std::nullopt;
      possibleMoves.clear();
      pendingPromotionMove = std::nullopt;

      ImGui::CloseCurrentPopup();
    }
    ImGui::PopStyleColor(3);

    ImGui::EndPopup();
  }

  ImGui::Spacing();
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
  GameState state = game.getGameState();
  PieceColor turn = game.getCurrentTurn();

  float windowWidth = ImGui::GetContentRegionAvail().x;

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

  ImGui::Separator();

  int paladinCount = 0;
  int readyPaladins = 0;
  int maxBonus = 0;

  for (int y = 0; y < 8; ++y) {
      for (int x = 0; x < 8; ++x) {
          const Piece* p = game.getBoard().getPiece(x, y);
          if (p && p->getColor() == turn) {
              if (const auto* paladin = dynamic_cast<const Paladin*>(p)) {
                  paladinCount++;
                  int bonus = paladin->getCurrentBonus();
                  if (bonus > 0) {
                      readyPaladins++;
                      if (bonus > maxBonus) maxBonus = bonus;
                  }
              }
          }
      }
  }

  if (paladinCount > 0) {
      if (readyPaladins == 0) {
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
          ImGui::TextWrapped("WARNING: Paladin is paralyzed this turn!");
          ImGui::PopStyleColor();
      } else {
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
          ImGui::TextWrapped("WARNING: Paladin is ready (Bonus +%d)!", maxBonus);
          ImGui::PopStyleColor();
      }
  }

  ImGui::Separator();

  int duration = game.getCurseDuration();
  int cooldown = game.getCurseCooldown();
  if (duration > 0) {
      Coords cursed = game.getCursedSquare();
      ImGui::TextColored(ImVec4(0.6f, 0.0f, 0.8f, 1.0f), "⚠️ Case Maudite en [%d,%d] (Durée: %d tours)", cursed.x, cursed.y, duration);
  } else if (cooldown > 0) {
      ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Calme avant la tempête (Prochaine malédiction dans %d tours)", cooldown);
  }
}

bool GameRenderer::handleSquareClick(Coords pos, Game &game, Scene3D *scene3D) {
  if (game.getAppState() != AppState::PLAYING) {
      return false; 
  }

  if (scene3D && scene3D->isAnimationPlaying()) {
      return false; 
  }

  const Board &board = game.getBoard();
  PieceColor currentTurn = game.getCurrentTurn();

  if (!selectedCase.has_value()) {
    const Piece *p = board.getPiece(pos);
    if (p && p->getColor() == currentTurn) {
      selectedCase = pos;
      possibleMoves.clear();
      auto moves = game.getValidMoves(pos);
      for (const auto &m : moves)
        possibleMoves.push_back(m.to);
    }
    return false; 
  }

  else {
    Coords start = selectedCase.value();
    if (start == pos) {
      selectedCase = std::nullopt;
      possibleMoves.clear();
      return false;
    }

    const Piece *pStart = board.getPiece(start);
    if (!pStart)
      return false;

    PieceType type = pStart->getType();
    Move moveAttempt(start, pos, type);

    bool isPawn = (type == PieceType::Pawn);
    bool isLastRow = (pos.y == 0 || pos.y == 7);

    if (isPawn && isLastRow) {
      pendingPromotionMove = moveAttempt;
      pendingPromotionMove->isPromotion = true;
      shouldOpenPromotion = true;
      return true;
    }

    else {
      if (game.playMove(moveAttempt)) {
        if (scene3D) {
          if (game.getHistory().back().capturedPiece.has_value()) {
              PieceColor capCol = game.getCurrentTurn();
              glm::vec4 expColor = (capCol == PieceColor::White) ? glm::vec4(1.0f, 0.9f, 0.8f, 1.0f) : glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
              scene3D->spawnExplosion(glm::vec3(pos.x, 0.5f, pos.y), expColor);
          }

          PieceColor movedColor = (game.getCurrentTurn() == PieceColor::White)
                                      ? PieceColor::Black
                                      : PieceColor::White;
          scene3D->triggerMoveAnimation(moveAttempt.from, moveAttempt.to,
                                        moveAttempt.movingPiece, movedColor);
        }
        selectedCase = std::nullopt;
        possibleMoves.clear();
      } else {
        const Piece *pNew = board.getPiece(pos);
        if (pNew && pNew->getColor() == currentTurn) {
          selectedCase = pos;
          possibleMoves.clear();
          auto moves = game.getValidMoves(pos);
          for (const auto &m : moves)
            possibleMoves.push_back(m.to);
        } else {
          selectedCase = std::nullopt;
          possibleMoves.clear();
        }
      }
      return false; 
    }
  }
}

std::string GameRenderer::coordToString(Coords c) {
  char col = 'a' + c.x;
  int row = 8 - c.y;

  std::stringstream ss;
  ss << col << row;
  return ss.str();
}
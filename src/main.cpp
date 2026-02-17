#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>

#include <glad/glad.h>

#include "imgui.h"
#include "quick_imgui/quick_imgui.hpp"

#include "./customs/paladin.hpp"
#include "./customs/siegeTower.hpp"

#include "./model/Factory/factory.hpp"

#include "model/game.hpp"
#include "view/Scene3D.hpp"
#include "view/gameRenderer.hpp"

void initGame(Game *game);

int main() {
  std::srand(std::time(nullptr));

  std::unique_ptr<Game> game;
  std::unique_ptr<GameRenderer> renderer;
  std::unique_ptr<Scene3D> scene3D;

  initGame(game.get());

  quick_imgui::loop(
      "Jeu d'Echecs",
      {.init =
           [&]() {
             ImGuiIO &io = ImGui::GetIO();
             (void)io;
             io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

             io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
             io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
             io.IniFilename = nullptr;

             static const ImWchar ranges[] = {
                 0x0020, 0x00FF, 0x2000, 0x27FF, 0,
             };

             const char *fontPath = "./ressources/fonts/DejaVuSans.ttf";
             std::ifstream fileCheck(fontPath);

             if (fileCheck.good()) {

               io.Fonts->AddFontFromFileTTF(fontPath, 24.0f, nullptr, ranges);
               std::cout << "SUCCES : Police chargée depuis " << fontPath
                         << std::endl;
             } else {
               std::cout << "ERREUR CRITIQUE : Le fichier " << fontPath
                         << " n'existe pas !" << std::endl;
               io.Fonts->AddFontDefault();
             }
             io.Fonts->Build();

             game = std::make_unique<Game>();
             renderer = std::make_unique<GameRenderer>();

             scene3D = std::make_unique<Scene3D>();
             scene3D->init();

             glEnable(GL_DEPTH_TEST);

             std::cout << "Moteur, Affichage 2D et Scène 3D initialisés."
                       << std::endl;
           },

       .loop =
           [&]() {
             float dt = ImGui::GetIO().DeltaTime;

             glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

             if (scene3D && game) {
               scene3D->draw(*game, dt);
             }

             if (game && renderer) {
               renderer->render(*game, scene3D.get());
             }
           }});

  return 0;
}

void initGame(Game *game) {
  // GameRegistry.cpp
  PieceFactory::registerPiece(
      "Paladin", [](PieceColor c) { return std::make_unique<Paladin>(c); });

  for (const auto &scen : Paladin::getAddonScenarios()) {
    ScenarioRegistry::instance().add(scen.name, scen.desc, scen.fen);
  }

  PieceFactory::registerPiece("SiegeTower", [](PieceColor c) {
    return std::make_unique<SiegeTower>(c);
  });

  // Enregistrement des scénarios (Optionnel)
  for (const auto &s : SiegeTower::getAddonScenarios()) {
    ScenarioRegistry::instance().add(s.name, s.desc, s.fen);
  }
}
#pragma once

#include <iostream>
#include <vector>

struct GameScenario {
  std::string name;
  std::string desc;
  std::string fen;
};

class ScenarioRegistry {
public:
  // --- ACCÈS SINGLETON ---
  // Cette méthode statique garantit qu'il n'y a qu'une seule instance du
  // registre
  static ScenarioRegistry &instance();

  // Suppression des constructeurs de copie pour garantir l'unicité
  ScenarioRegistry(const ScenarioRegistry &) = delete;
  void operator=(const ScenarioRegistry &) = delete;

  // --- MÉTHODES PUBLIQUES ---

  /**
   * Enregistre un nouveau scénario dans la liste.
   * @param name : Le titre du bouton
   * @param description : Le texte d'aide
   * @param fen : Le setup du plateau
   */
  void add(const std::string &name, const std::string &description,
           const std::string &fen);

  /**
   * Récupère la liste de tous les scénarios enregistrés.
   * Utilisé par l'interface graphique (ImGui) pour générer le menu.
   */
  const std::vector<GameScenario> &getAll() const;

private:
  // Constructeur privé (personne ne peut créer l'instance à la main)
  ScenarioRegistry() = default;

  // La liste de stockage
  std::vector<GameScenario> m_scenarios;
};
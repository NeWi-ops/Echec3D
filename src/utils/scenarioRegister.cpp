#include "./scenarioRegister.hpp"

// =========================================================
// IMPLEMENTATION DU SINGLETON
// =========================================================

ScenarioRegistry &ScenarioRegistry::instance() {
  // Variable statique locale : elle est initialisée une seule fois
  // lors du premier appel à cette fonction. C'est thread-safe en C++11.
  static ScenarioRegistry instance;
  return instance;
}

// =========================================================
// METHODES PUBLIQUES
// =========================================================

void ScenarioRegistry::add(const std::string &name,
                           const std::string &description,
                           const std::string &fen) {

  // On crée une instance de la structure GameScenario
  // Note : Dans ton header, le champ s'appelle 'desc', ici on lui passe
  // 'description'
  GameScenario newScenario = {name, description, fen};

  // On l'ajoute à la liste
  m_scenarios.push_back(newScenario);
}

const std::vector<GameScenario> &ScenarioRegistry::getAll() const {
  // On retourne simplement la référence vers le vecteur privé
  return m_scenarios;
}
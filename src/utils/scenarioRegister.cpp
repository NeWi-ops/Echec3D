#include "./scenarioRegister.hpp"

// C'EST UN SINGLETON

ScenarioRegistry &ScenarioRegistry::instance() {
  // Variable statique locale : elle est initialisée une seule fois
  // lors du premier appel à cette fonction. C'est thread-safe en C++11.
  static ScenarioRegistry instance;
  return instance;
}

void ScenarioRegistry::add(const std::string &name,
                           const std::string &description,
                           const std::string &fen) {

  GameScenario newScenario = {name, description, fen};

  m_scenarios.push_back(newScenario);
}

const std::vector<GameScenario> &ScenarioRegistry::getAll() const {
  return m_scenarios;
}
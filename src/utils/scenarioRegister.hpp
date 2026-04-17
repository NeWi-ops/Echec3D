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
  static ScenarioRegistry &instance();

  ScenarioRegistry(const ScenarioRegistry &) = delete;
  void operator=(const ScenarioRegistry &) = delete;

  // Enregistre un nouveau scénario dans la liste.

  void add(const std::string &name, const std::string &description,
           const std::string &fen);

  const std::vector<GameScenario> &getAll() const;

private:
  ScenarioRegistry() = default;

  std::vector<GameScenario> m_scenarios;
};
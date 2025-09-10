#pragma once
#include "input/Directions.h"
#include <iostream>
#include <string>
#include <string_view>

class GameSession;
class Creature;

class Action {
protected:
  std::string m_name{};
  bool m_needsInput{};

public:
  Action(std::string_view name, bool needsInput)
      : m_name{name}, m_needsInput{needsInput} {}
  std::string_view getName() const { return m_name; }
  virtual void execute(GameSession &gameSession, Creature &actor,
                       Creature &target) = 0;
  virtual void playerExecute(GameSession &gameSession,
                             Directions::Direction direction) = 0;
  bool needsInput() const { return m_needsInput; }

  virtual ~Action() = default;
  friend std::ostream &operator<<(std::ostream &out, const Action &action) {
    out << action.getName();
    return out;
  }
};
#pragma once
#include "input/Directions.h"
#include <sstream>
#include <string>
#include <string_view>

class GameSession;
class Creature;

class Action {
protected:
  std::string m_name{};
  bool m_needsHotkeyInput{};
  bool m_needsDirectionalInput{};
  // if the action has both of these at true, executing it
  // as a player in the action menu will use the hotkey one,
  // but the directional one might be mapped to another
  // command
  int m_cost{1}; // in some systems (pf2e) variable action cost
  // for 1 spell but not gonna bother here

public:
  Action(std::string_view name, bool needsDirectionalInput,
         bool needsHotkeyInput)
      : m_name{name}, m_needsHotkeyInput{needsHotkeyInput},
        m_needsDirectionalInput{needsDirectionalInput} {}
  std::string_view getName() const { return m_name; }
  virtual std::string execute(GameSession &gameSession, Creature &actor,
                              Creature &target) const = 0;
  virtual std::string
  playerExecute([[maybe_unused]] GameSession &gameSession,
                [[maybe_unused]] Directions::Direction direction) const {
    return {};
  };
  virtual std::string playerExecute([[maybe_unused]] GameSession &gameSession,
                                    [[maybe_unused]] Creature &target) const {
    return {};
  };
  virtual std::string
  playerExecute([[maybe_unused]] GameSession &gameSession) const {
    return {};
  };
  // these actions arent pure virtual because most actions need
  // one of the two, not both
  bool needsDirectionalInput() const { return m_needsDirectionalInput; }
  bool needsHotkeyInput() const { return m_needsHotkeyInput; }

  virtual ~Action() = default;
  friend std::ostream &operator<<(std::ostream &out, const Action &action) {
    out << action.getName();
    return out;
  }
};
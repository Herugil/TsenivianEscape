#pragma once
#include "gameObjects/creatures/Stats.h"
#include "input/Directions.h"
#include "nlohmann/json.hpp"
#include <sstream>
#include <string>
#include <string_view>

using json = nlohmann::json;

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
  Stat m_usedStat{};
  int m_cost{1}; // in some systems (pf2e) variable action cost
  // for 1 spell but not gonna bother here
  int m_maxCharges{-1};     // -1 means unlimited
  int m_currentCharges{-1}; // -1 means unlimited
  int m_cooldown{0};
  int m_currentCooldown{0};

public:
  Action(std::string_view name, bool needsDirectionalInput,
         bool needsHotkeyInput, Stat usedStat = Stat::nbStats, int cost = 1,
         int maxCharges = -1, int cooldown = 0)
      : m_name{name}, m_needsHotkeyInput{needsHotkeyInput},
        m_needsDirectionalInput{needsDirectionalInput}, m_usedStat{usedStat},
        m_cost{cost}, m_maxCharges{maxCharges}, m_currentCharges{maxCharges},
        m_cooldown{cooldown} {}
  std::string_view getName() const { return m_name; }
  virtual std::string execute(GameSession &gameSession, Creature &actor,
                              Creature &target) = 0;
  virtual std::string
  playerExecute([[maybe_unused]] GameSession &gameSession,
                [[maybe_unused]] Directions::Direction direction) {
    return {};
  };
  virtual std::string playerExecute([[maybe_unused]] GameSession &gameSession,
                                    [[maybe_unused]] Creature &target) {
    return {};
  };
  virtual std::string playerExecute([[maybe_unused]] GameSession &gameSession) {
    return {};
  };
  // these actions arent pure virtual because most actions need
  // one of the two, not both
  bool needsDirectionalInput() const { return m_needsDirectionalInput; }
  bool needsHotkeyInput() const { return m_needsHotkeyInput; }
  Stat getUsedStat() const { return m_usedStat; }
  virtual std::unique_ptr<Action> clone() const = 0;
  friend std::ostream &operator<<(std::ostream &out, const Action &action) {
    out << action.getName();
    return out;
  }
  int getCooldown() const { return m_cooldown; }
  int getCurrentCooldown() const { return m_currentCooldown; }
  void reduceCooldown() {
    if (m_currentCooldown > 0)
      --m_currentCooldown;
  }
  void resetCooldown() { m_currentCooldown = 0; }
  void refillCharges() {
    if (m_maxCharges != -1)
      m_currentCharges = m_maxCharges;
  }
  void setCurrentCharges(int charges) { m_currentCharges = charges; }
  virtual int getRange([[maybe_unused]] Creature &actor) const { return 0; }
  virtual int getDamage([[maybe_unused]] Creature &actor) const { return 0; }
  virtual int getHitChance([[maybe_unused]] Creature &actor,
                           [[maybe_unused]] Creature &target) const {
    return 0;
  }
  int getCurrentCharges() const { return m_currentCharges; }
  int getMaxCharges() const { return m_maxCharges; }
  bool useActionResources(Creature &creature);
  bool canBeUsed(Creature &creature);

  virtual json toJson() const = 0;

  virtual ~Action() = default;
};
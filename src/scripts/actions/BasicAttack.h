#pragma once
#include "gameObjects/creatures/Creature.h"
#include "scripts/actions/Action.h"
#include "scripts/passives/PassiveEffect.h"

class GameSession;

class BasicAttack : public Action {
protected:
  std::vector<std::unique_ptr<PassiveEffect>> m_applyOnHit{};

public:
  BasicAttack(std::string_view name, Stat usedStat,
              std::vector<std::unique_ptr<PassiveEffect>> applyOnHit = {});
  std::string execute(GameSession &gameSession, Creature &actor,
                      Creature &target) const override;
  std::string playerExecute(GameSession &gameSession,
                            Directions::Direction direction) const override;
  std::string playerExecute(GameSession &gameSession,
                            Creature &target) const override;
  std::unique_ptr<Action> clone() const override {
    std::vector<std::unique_ptr<PassiveEffect>> effectsCloned;
    for (const auto &effect : m_applyOnHit) {
      effectsCloned.emplace_back(effect->clone());
    }
    return std::make_unique<BasicAttack>(m_name, m_usedStat,
                                         std::move(effectsCloned));
  };
  ~BasicAttack() = default;
};
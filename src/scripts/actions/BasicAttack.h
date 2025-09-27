#pragma once
#include "gameObjects/creatures/Creature.h"
#include "scripts/actions/Action.h"
#include "scripts/passives/PassiveEffect.h"

using json = nlohmann::json;

class GameSession;

class BasicAttack : public Action {
protected:
  std::vector<std::unique_ptr<PassiveEffect>> m_applyOnHit{};

public:
  BasicAttack(std::string_view name, Stat usedStat,
              std::vector<std::unique_ptr<PassiveEffect>> applyOnHit = {},
              int cost = 1, int maxCharges = -1, int cooldown = 0);
  std::string execute(GameSession &gameSession, Creature &actor,
                      Creature &target) override;
  std::string playerExecute(GameSession &gameSession,
                            Directions::Direction direction) override;
  std::string playerExecute(GameSession &gameSession,
                            Creature &target) override;
  std::unique_ptr<Action> clone() const override {
    std::vector<std::unique_ptr<PassiveEffect>> effectsCloned;
    for (const auto &effect : m_applyOnHit) {
      effectsCloned.emplace_back(effect->clone());
    }
    return std::make_unique<BasicAttack>(m_name, m_usedStat,
                                         std::move(effectsCloned), m_cost,
                                         m_maxCharges, m_cooldown);
  };
  int getRange(Creature &actor) const override;
  int getDamage(Creature &actor) const override;
  int getHitChance(Creature &actor, Creature &target) const override;

  json toJson() const override;

  ~BasicAttack() = default;
};
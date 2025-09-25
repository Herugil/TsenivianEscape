#pragma once
#include "scripts/actions/BasicAttack.h"
#include <functional>

class HealingAttack : public BasicAttack {
protected:
  std::function<int(const Creature &actor, const Creature &target)>
      m_healAmountFunc;

public:
  HealingAttack(
      const std::string &name, int cost, int charges, int cooldown,
      Stat usedStat,
      std::function<int(const Creature &actor, const Creature &target)>
          healAmountFunc,
      std::vector<std::unique_ptr<PassiveEffect>> applyOnHit = {});
  std::string execute(GameSession &gameSession, Creature &actor,
                      Creature &target) override;
  std::string playerExecute(GameSession &gameSession,
                            Creature &target) override;
  std::unique_ptr<Action> clone() const override {
    std::vector<std::unique_ptr<PassiveEffect>> effects;
    for (const auto &effect : m_applyOnHit) {
      effects.emplace_back(effect->clone());
    }
    return std::make_unique<HealingAttack>(
        m_name, m_cost, m_maxCharges, m_cooldown, m_usedStat, m_healAmountFunc,
        std::move(effects));
  }
  virtual ~HealingAttack() = default;
};

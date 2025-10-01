#pragma once
#include "scripts/actions/Action.h"
#include "scripts/passives/PassiveEffect.h"

class StatModifier : public Action {
protected:
  PassiveEffect m_effect;
  int m_range{};

public:
  StatModifier(std::string_view name, PassiveEffect::Type type, int value,
               int rounds, int cooldown, int cost, int maxCharges, int range);
  std::string execute(GameSession &gameSession, Creature &actor,
                      Creature &target) override;
  std::string playerExecute(GameSession &gameSession,
                            Creature &target) override;
  std::unique_ptr<Action> clone() const override {
    return std::make_unique<StatModifier>(*this);
  };
  int getRange(const Creature &actor) const override;
  json toJson() const override;
  ~StatModifier() = default;
};
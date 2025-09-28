#pragma once

#include "scripts/actions/Action.h"
#include <functional>

using json = nlohmann::json;

class CubeAoe : public Action {
protected:
  int m_radius;
  std::function<int(const Creature &actor)> m_damageFormula;

public:
  CubeAoe(std::string_view name, int radius,
          std::function<int(const Creature &actor)> damageFormula, int cost,
          int maxCharges, int cooldown);
  std::string playerExecute(GameSession &gameSession) override;
  std::string execute(GameSession &gameSession, Creature &actor,
                      [[maybe_unused]] Creature &target) override;
  std::unique_ptr<Action> clone() const override {
    return std::make_unique<CubeAoe>(*this);
  }
  int getDamage(const Creature &actor) const override;

  json toJson() const override;
  virtual ~CubeAoe() = default;
};
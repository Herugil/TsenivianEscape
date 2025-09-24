#pragma once

#include "scripts/actions/Action.h"
#include <functional>

class CubeAoe : public Action {
protected:
  int m_radius;
  std::function<int(const Creature &actor)> m_damageFormula;

public:
  CubeAoe(std::string_view name, int radius,
          std::function<int(const Creature &actor)> damageFormula, int cost,
          int maxCharges, int cooldown);
  std::string playerExecute(GameSession &gameSession) override;
  std::string execute(GameSession &gameSession, Creature &actor);
  std::unique_ptr<Action> clone() const override {
    return std::make_unique<CubeAoe>(*this);
  }
};
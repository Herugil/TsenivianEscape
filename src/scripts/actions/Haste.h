#pragma once

#include "scripts/actions/Action.h"

class Haste : public Action {
  int m_value{};

public:
  Haste(int value, int cost = 0, int maxCharges = 2, int cooldown = 1,
        std::string_view name = "Haste");
  std::string execute(GameSession &gameSession, Creature &actor,
                      Creature &target) override;
  std::string playerExecute(GameSession &gameSession) override;
  std::unique_ptr<Action> clone() const override;
  virtual ~Haste() = default;
};

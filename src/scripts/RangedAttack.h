#pragma once
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "scripts/Action.h"

class RangedAttack : public Action {

public:
  RangedAttack(std::string_view name);
  std::ostringstream execute(GameSession &gameSession, Creature &actor,
                             Creature &target) const override;
  std::ostringstream playerExecute(GameSession &gameSession,
                                   Creature &target) const override;
  ~RangedAttack() = default;
};
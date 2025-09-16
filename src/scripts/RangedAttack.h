#pragma once
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "scripts/Action.h"

class RangedAttack : public Action {

public:
  RangedAttack(std::string_view name);
  void execute(GameSession &gameSession, Creature &actor,
               Creature &target) const override;
  void playerExecute(GameSession &gameSession, Creature &target) const override;
  ~RangedAttack() = default;
};
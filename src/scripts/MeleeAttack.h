#pragma once
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "scripts/Action.h"

class MeleeAttack : public Action {

public:
  MeleeAttack(std::string_view name);
  std::ostringstream execute(GameSession &gameSession, Creature &actor,
                             Creature &target) const override;
  std::ostringstream
  playerExecute(GameSession &gameSession,
                Directions::Direction direction) const override;
  ~MeleeAttack() = default;
};
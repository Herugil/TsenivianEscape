#pragma once
#include "gameObjects/creatures/Creature.h"
#include "scripts/Action.h"

class GameSession;

class MeleeAttack : public Action {

public:
  MeleeAttack(std::string_view name);
  std::string execute(GameSession &gameSession, Creature &actor,
                      Creature &target) const override;
  std::string playerExecute(GameSession &gameSession,
                            Directions::Direction direction) const override;
  std::string playerExecute(GameSession &gameSession,
                            Creature &target) const override;
  ~MeleeAttack() = default;
};
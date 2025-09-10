#pragma once
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "scripts/Action.h"

class GameSession;
class Creature;

class MeleeAttack : public Action {

public:
  MeleeAttack(std::string_view name);
  void execute(GameSession &gameSession, Creature &actor,
               Creature &target) override;
  void playerExecute(GameSession &gameSession,
                     Directions::Direction direction) override;
  ~MeleeAttack() = default;
};
#pragma once
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "scripts/actions/Action.h"

class RangedAttack : public Action {

public:
  RangedAttack(
      std::string_view name); // TODO: this should take range and
                              // damage as parameters, so that when displaying
                              // the details of the action, they can be
                              // displayed without calling weapon functions
  std::string execute(GameSession &gameSession, Creature &actor,
                      Creature &target) const override;
  std::string playerExecute(GameSession &gameSession,
                            Creature &target) const override;
  ~RangedAttack() = default;
};
#pragma once
#include "gameObjects/creatures/Creature.h"
#include "scripts/actions/Action.h"

class GameSession;

class Dodge : public Action {

public:
  using Action::playerExecute;
  Dodge(std::string_view name);
  std::string execute(GameSession &gameSession, Creature &actor,
                      Creature &target) const override;
  std::string execute(Creature &actor) const;
  std::string playerExecute(GameSession &gameSession) const override;
  ~Dodge() = default;
};
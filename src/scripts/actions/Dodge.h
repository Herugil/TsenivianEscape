#pragma once
#include "gameObjects/creatures/Creature.h"
#include "scripts/actions/Action.h"

class GameSession;

class Dodge : public Action {

public:
  using Action::playerExecute;
  Dodge(std::string_view name);
  std::string execute(GameSession &gameSession, Creature &actor,
                      Creature &target) override;
  std::string execute(Creature &actor);
  std::string playerExecute(GameSession &gameSession) override;
  std::unique_ptr<Action> clone() const override {
    return std::make_unique<Dodge>(*this);
  };
  ~Dodge() = default;
};
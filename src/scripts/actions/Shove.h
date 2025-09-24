#pragma once
#include "gameObjects/GameObject.h"
#include "gameObjects/creatures/Creature.h"
#include "scripts/actions/Action.h"
#include <memory>

class GameSession;

class Shove : public Action {

public:
  explicit Shove(std::string_view name = "Shove");
  std::string execute(GameSession &gameSession, Creature &actor,
                      Creature &target) override;
  std::string execute(GameSession &gameSession, Creature &actor,
                      std::shared_ptr<GameObject> target);
  std::string playerExecute(GameSession &gameSession,
                            Directions::Direction direction) override;
  std::unique_ptr<Action> clone() const override {
    return std::make_unique<Shove>(*this);
  };
  ~Shove() = default;
};

#pragma once
#include "gameObjects/GameObject.h"
#include "gameObjects/creatures/Creature.h"
#include "scripts/Action.h"
#include <memory>

class GameSession;

class Shove : public Action {

public:
  explicit Shove(std::string_view name = "Shove");
  void execute(GameSession &gameSession, Creature &actor,
               Creature &target) override;
  void execute(GameSession &gameSession, Creature &actor,
               std::shared_ptr<GameObject> target);
  void playerExecute(GameSession &gameSession,
                     Directions::Direction direction) override;
  ~Shove() = default;
};

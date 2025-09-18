#pragma once
#include "gameObjects/GameObject.h"
#include "gameObjects/creatures/Creature.h"
#include "scripts/Action.h"
#include <memory>

class GameSession;

class Shove : public Action {

public:
  explicit Shove(std::string_view name = "Shove");
  std::ostringstream execute(GameSession &gameSession, Creature &actor,
                             Creature &target) const override;
  std::ostringstream execute(GameSession &gameSession, Creature &actor,
                             std::shared_ptr<GameObject> target) const;
  std::ostringstream
  playerExecute(GameSession &gameSession,
                Directions::Direction direction) const override;
  ~Shove() = default;
};

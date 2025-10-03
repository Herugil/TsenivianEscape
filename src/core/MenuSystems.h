#pragma once
#include "core/GameState.h"
#include "gameObjects/items/Item.h"
#include <memory>

class GameSession;

namespace MenuSystems {
struct ItemInspectResult {
  std::string logs;
  GameState nextState;
};
ItemInspectResult inspectItem(GameSession &gameSession,
                              std::weak_ptr<Item> item);
} // namespace MenuSystems
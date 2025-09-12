#pragma once
#include "core/GameSession.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "map/Point.h"
#include <memory>

namespace NpcCombatAI {
enum Behaviors {
  basicAttack,
  nbBehaviors,
};
void npcActCombat(GameSession &gameSession,
                  std::shared_ptr<NonPlayableCharacter> actor);
std::deque<Point> getDestination(Behaviors currentGoal,
                                 const GameSession &gameSession,
                                 const NonPlayableCharacter &actor);
} // namespace NpcCombatAI
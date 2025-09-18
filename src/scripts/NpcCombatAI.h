#pragma once
#include "core/GameSession.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "map/Point.h"
#include <memory>

namespace NpcCombatAI {
std::ostringstream npcActCombat(GameSession &gameSession,
                                std::shared_ptr<NonPlayableCharacter> actor);
} // namespace NpcCombatAI
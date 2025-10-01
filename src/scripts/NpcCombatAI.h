#pragma once
#include "core/GameSession.h"
#include "map/Point.h"
#include <memory>

class NonPlayableCharacter;

namespace NpcCombatAI {
std::string npcActCombat(GameSession &gameSession,
                         std::shared_ptr<NonPlayableCharacter> actor);
std::string tryCreatureMove(GameSession &gameSession,
                            std::shared_ptr<NonPlayableCharacter> actor);
std::string tryCreatureAction(GameSession &gameSession,
                              std::shared_ptr<NonPlayableCharacter> actor,
                              Creature &target, Action *action);
bool checkTargetAvailable(GameSession &gameSession,
                          std::shared_ptr<NonPlayableCharacter> actor,
                          Creature &target, Action *action);
std::string useAction(GameSession &gameSession,
                      std::shared_ptr<NonPlayableCharacter> actor);
} // namespace NpcCombatAI
#pragma once
#include "Settings.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/creatures/Player.h"
#include <iostream>

namespace Interface {
void displayCombatInterface(const Player &player,
                            std::string_view creatureName = "Your");
void timeAndDisplayInterface(GameSession &gameSession, Creature &actor,
                             int timeToWait = Settings::g_timeEnemyTurnMS);
} // namespace Interface
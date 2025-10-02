#pragma once

#include "core/GameState.h"
#include <iostream>

class GameSession;

namespace Combat {
GameState getCombatState(GameSession &gameSession);
GameState playerTurn(GameSession &gameSession);
GameState enemyTurn(GameSession &gameSession,
                    std::ostringstream &m_logsToDisplay);
} // namespace Combat

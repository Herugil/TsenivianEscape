#include "core/Combat.h"
#include "core/GameSession.h"
#include "scripts/NpcCombatAI.h"
#include "utils/Interface.h"
#include "utils/ScreenUtils.h"
#include <chrono>
#include <thread>

GameState Combat::getCombatState(GameSession &gameSession) {
  gameSession.initializeTurnOrder();
  auto activeCreature{gameSession.getActiveCreature().lock()};
  if (activeCreature && activeCreature == gameSession.getPlayerPtr())
    return GameState::CombatPlayerTurn;
  else
    return GameState::CombatEnemyTurn;
}

GameState Combat::playerTurn(GameSession &gameSession) {

  if (!gameSession.enemiesInMap()) {
    gameSession.getPlayer().unsetCombat();
    return GameState::Exploration;
  }
  auto &player{gameSession.getPlayer()};
  ScreenUtils::clearScreen();
  gameSession.displayMap();
  std::cout << "Round " << gameSession.getCurrentTurn() << "\n";
  std::cout << "Your turn: \n";
  Interface::displayCombatInterface(player);
  return GameState::CombatPlayerTurn;
}

GameState Combat::enemyTurn(GameSession &gameSession,
                            std::ostringstream &logsToDisplay) {
  if (!gameSession.enemiesInMap()) {
    gameSession.getPlayer().unsetCombat();
    gameSession.resetInitiative();
    return GameState::Exploration;
  }
  auto activeCreature{gameSession.getActiveCreature().lock()};
  if (activeCreature) {
    if (auto enemy{
            std::dynamic_pointer_cast<NonPlayableCharacter>(activeCreature)}) {
      ScreenUtils::clearScreen();
      gameSession.displayMap();
      std::cout << "Round " << gameSession.getCurrentTurn() << "\n";
      std::cout << enemy->getName() << " turn: \n";
      Interface::displayCombatInterface(gameSession.getPlayer());
      std::this_thread::sleep_for(
          std::chrono::milliseconds(Settings::g_timeEnemyActionMS));
      if (enemy->getCurrentBehavior() == NonPlayableCharacter::skipTurn) {
        enemy->resetTurn();
        enemy->reduceCooldowns();
        enemy->setDefaultBehavior();
        gameSession.incrementTurnIndex();
      } else {
        logsToDisplay << NpcCombatAI::npcActCombat(gameSession, enemy)
                      << gameSession.cleanDeadNpcs();
        if (!logsToDisplay.str().empty())
          return GameState::Display;
      }
    }
  }
  return GameState::Exploration;
}
#include "Settings.h"
#include "core/GameSession.h"
#include "core/GameStateManager.h"
#include "dataLoading/parseJson.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/creatures/Player.h"
#include "gameObjects/items/Weapon.h"
#include "gameObjects/terrain/Container.h"
#include "input/Directions.h"
#include "input/Input.h"
#include "map/Layer.h"
#include "map/Map.h"
#include "map/Point.h"
#include "scripts/NpcCombatAI.h"
#include "utils/Interface.h"
#include "utils/ScreenUtils.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>

int main() {
  GameSession gameSession{
      std::make_shared<Player>(Point(2, 1), "placeHolder", 10)};
  std::unordered_map<std::string, std::shared_ptr<Item>> items{
      DataLoader::getAllItems()};
  DataLoader::populateGameSession(items, gameSession);
  GameStateManager gameStateManager{std::move(gameSession)};
  gameStateManager.mainLoop();
}

//   gameSession.getPlayer().takeItem(items["itemSword"]->clone());
//   DataLoader::populateGameSession(items, gameSession);
//   gameSession.setCurrentMap("level1");
//   gameSession.respawnPlayer();
//   gameSession.displayMap();

//   while (true) {
//     if (gameSession.enemiesInMap()) {
//       gameSession.initializeTurnOrder();
//       for (auto character : gameSession.getTurnOrder()) {
//         auto ptr{character.lock()};
//         if (ptr) {
//           ptr->setCombat();
//         }
//       }
//       std::size_t initiativeIndex{0};
//       while (gameSession.enemiesInMap()) {
//         auto
//         activeChar{gameSession.getTurnOrder()[initiativeIndex].lock()}; if
//         (!activeChar)
//           break;

//         if (auto player{std::dynamic_pointer_cast<Player>(activeChar)}) {
//           Interface::displayCombatInterface(*player);
//           while ((player->getActionPoints() > 0) ||
//                  (player->getMovementPoints() > 0)) {
//             Command::command command{
//                 CommandHandler::getCommand(Input::getKeyBlocking())};
//             if (command == Command::skipTurn)
//               break;
//             CommandHandler::executeWorldCommand(gameSession, command);
//             Interface::displayCombatInterface(gameSession.getPlayer());
//             gameSession.initializeTurnOrder(); // if someone dies, no turn
//             if (!gameSession.enemiesInMap())
//               break;
//           }
//           gameSession.getPlayer().resetTurn();
//           Interface::timeAndDisplayInterface(gameSession, *player,
//                                              Settings::g_timeEnemyActionMS);

//         } else if (auto
//         enemy{std::dynamic_pointer_cast<NonPlayableCharacter>(
//                        activeChar)}) {
//           NpcCombatAI::npcActCombat(gameSession, enemy);
//         }
//         if (initiativeIndex >= gameSession.getTurnOrder().size() - 1) {
//           initiativeIndex = 0; // loop back to first player
//         } else {
//           initiativeIndex++;
//         }
//       }
//       gameSession.getPlayer().unsetCombat();
//       gameSession.resetInitiative();
//     }

//     if (Input::hasKeyPressed()) {
//       Command::command command{
//           CommandHandler::getCommand(Input::getKeyBlocking())};
//       CommandHandler::executeWorldCommand(gameSession, command);
//     }
//     std::this_thread::sleep_for(
//         std::chrono::milliseconds(Settings::g_timeSleepMS));
//   }
// }
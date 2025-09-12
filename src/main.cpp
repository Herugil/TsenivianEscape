#include "Settings.h"
#include "core/GameSession.h"
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

GameSession &prepGame(GameSession &gameSession) {
  gameSession.getMap().placeWalls(Point(0, 0), Point(0, 19));
  gameSession.getMap().placeWalls(Point(0, 0), Point(19, 0));
  gameSession.getMap().placeWalls(Point(19, 0), Point(19, 19));
  gameSession.getMap().placeWalls(Point(0, 9), Point(19, 9));
  gameSession.getMap().placeWalls(Point(5, 0), Point(5, 5));
  gameSession.getMap().placeWalls(Point(12, 19), Point(12, 5));
  std::vector<std::shared_ptr<Item>> containerItems;

  containerItems.push_back(
      std::make_shared<Weapon>("sword", Item::ItemType::oneHanded, 6, 1));
  containerItems.push_back(
      std::make_shared<Weapon>("spear", Item::ItemType::oneHanded, 3, 2));
  containerItems.push_back(
      std::make_shared<Weapon>("bow", Item::ItemType::twoHanded, 2, 10));
  Container cont(std::move(containerItems), Point(0, 0),
                 "A battered and old looking wooden chest.");
  gameSession.getMap().placeFloor(std::make_unique<Container>(std::move(cont)),
                                  Point(2, 2));
  auto enemyLoot{
      std::make_shared<Weapon>("Banana", Item::ItemType::oneHanded, 1, 1)};
  auto enemyLoot2{
      std::make_shared<Weapon>("Apple", Item::ItemType::oneHanded, 1, 1)};

  auto npc{std::make_shared<NonPlayableCharacter>(
      'e', Point(4, 7), 12, std::vector<std::shared_ptr<Item>>{enemyLoot},
      "Gruff-looking man",
      "A somber looking man, threatening you with a banana.", "A dead body.")};
  gameSession.addNpc(npc);

  auto npc2{std::make_shared<NonPlayableCharacter>(
      'E', Point(8, 2), 8, std::vector<std::shared_ptr<Item>>{enemyLoot2},
      "Strange-looking man",
      "A strange looking man, threatening you with an apple.", "A dead body.")};
  gameSession.addNpc(npc2);

  gameSession.getPlayer().takeItem(
      std::make_shared<Weapon>("Dagger", Item::ItemType::oneHanded, 2, 1));
  gameSession.displayMap();
  return gameSession;
}

int main() {
  GameSession gameSession{20, 10, std::make_shared<Player>(Point(2, 1), 10)};
  prepGame(gameSession);

  if (gameSession.enemiesInMap()) {
    gameSession.getPlayer().setCombat();
    gameSession.initializeTurnOrder();
    for (auto character : gameSession.getTurnOrder()) {
      auto ptr{character.lock()};
      if (ptr)
        ptr->setCombat();
    }
    std::size_t initiativeIndex{0};

    while (gameSession.enemiesInMap()) {
      auto activeChar{gameSession.getTurnOrder()[initiativeIndex].lock()};
      if (!activeChar)
        break;

      if (auto player{std::dynamic_pointer_cast<Player>(activeChar)}) {
        Interface::displayCombatInterface(*player);
        while ((player->getActionPoints() > 0) |
               (player->getMovementPoints() > 0)) {
          Command::command command{
              CommandHandler::getCommand(Input::getKeyBlocking())};
          CommandHandler::executeWorldCommand(gameSession, command);
          Interface::displayCombatInterface(gameSession.getPlayer());
          gameSession.initializeTurnOrder(); // if someone dies, no turn
          if (!gameSession.enemiesInMap())
            break;
        }
        gameSession.getPlayer().refillActionPoints();
        Interface::timeAndDisplayInterface(gameSession, *player,
                                           Settings::g_timeEnemyActionMS);

      } else if (auto enemy{std::dynamic_pointer_cast<NonPlayableCharacter>(
                     activeChar)}) {
        NpcCombatAI::npcActCombat(gameSession, enemy);
      }
      if (initiativeIndex >= gameSession.getTurnOrder().size() - 1) {
        initiativeIndex = 0; // loop back to first player
      } else {
        initiativeIndex++;
      }
    }

    while (true) {
      gameSession.getPlayer().unsetCombat();
      if (Input::hasKeyPressed()) {
        Command::command command{CommandHandler::getCommand(Input::getKey())};
        CommandHandler::executeWorldCommand(gameSession, command);
      }
      std::this_thread::sleep_for(
          std::chrono::milliseconds(Settings::g_timeSleepMS));
    }
  }
}
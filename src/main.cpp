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
#include "utils/ScreenUtils.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

void displayCombatInterface(const Player &player) {
  std::cout << "Movement:";
  for (int i{0}; i < player.getMovementPoints(); ++i) {
    std::cout << 'X';
  }
  for (int i{0}; i < player.getMaxMovementPoints() - player.getMovementPoints();
       ++i)
    std::cout << ' ';
  std::cout << "   ";
  std::cout << "Actions:";
  for (int i{0}; i < player.getActionPoints(); ++i) {
    std::cout << 'X';
  }
  for (int i{0}; i < player.getMaxActionPoints() - player.getActionPoints();
       ++i)
    std::cout << ' ';
  std::cout << '\n'
            << "HP: " << player.getHealthPoints() << '/'
            << player.getMaxHealthPoints() << '\n';
}

GameSession &prepGame(GameSession &gameSession) {
  gameSession.getMap().placeWalls(Point(0, 0), Point(0, 9));
  gameSession.getMap().placeWalls(Point(0, 0), Point(9, 0));
  gameSession.getMap().placeWalls(Point(9, 0), Point(9, 9));
  gameSession.getMap().placeWalls(Point(0, 9), Point(9, 9));
  gameSession.getMap().placeWalls(Point(5, 0), Point(5, 5));
  gameSession.getMap().placeWalls(Point(3, 9), Point(3, 5));
  std::vector<std::shared_ptr<Item>> containerItems;

  containerItems.push_back(
      std::make_shared<Weapon>("sword", Item::ItemType::oneHanded, 13, 1));
  containerItems.push_back(
      std::make_shared<Weapon>("spear", Item::ItemType::oneHanded, 2, 2));
  containerItems.push_back(
      std::make_shared<Weapon>("bow", Item::ItemType::twoHanded, 2, 10));
  Container cont(std::move(containerItems), Point(0, 0),
                 "A battered and old looking wooden chest.");
  gameSession.getMap().placeFloor(std::make_unique<Container>(std::move(cont)),
                                  Point(2, 2));
  auto enemyLoot{
      std::make_shared<Weapon>("Banana", Item::ItemType::oneHanded, 1, 1)};

  auto npc{std::make_shared<NonPlayableCharacter>(
      'e', Point(4, 7), 12, std::vector<std::shared_ptr<Item>>{enemyLoot},
      "Gruffy-looking man",
      "A somber looking man, threatening you with a banana.", "A dead body.")};
  gameSession.addNpc(npc);

  gameSession.getPlayer().takeItem(
      std::make_shared<Weapon>("Dagger", Item::ItemType::oneHanded, 2, 1));
  gameSession.displayMap();
  return gameSession;
}

int main() {
  GameSession gameSession{10, 10, std::make_shared<Player>(Point(2, 1), 10)};
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
        while ((player->getActionPoints() > 0) |
               (player->getMovementPoints() > 0)) {
          Command::command command{
              CommandHandler::getCommand(Input::getKeyBlocking())};
          CommandHandler::executeWorldCommand(gameSession, command);
          displayCombatInterface(gameSession.getPlayer());
          gameSession.initializeTurnOrder(); // if someone dies, no turn
        }
        gameSession.getPlayer().refillActionPoints();

      } else if (auto enemy{std::dynamic_pointer_cast<NonPlayableCharacter>(
                     activeChar)}) {
        while ((enemy->getActionPoints() > 0)) {
          // AI script!!!
          enemy->executeBasicAttack(gameSession.getPlayer(), gameSession);
          gameSession.initializeTurnOrder(); // if someone dies, no turn
          std::this_thread::sleep_for(
              std::chrono::milliseconds(Settings::g_timeEnemyTurnMS));
          ScreenUtils::clearScreen();
          gameSession.displayMap();
          displayCombatInterface(gameSession.getPlayer());
        }
      }
      if (initiativeIndex >= gameSession.getTurnOrder().size() - 1) {
        initiativeIndex = 0; // loop back to first player
      } else
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
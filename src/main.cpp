#include "Settings.h"
#include "core/GameSession.h"
#include "gameObjects/Item.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/creatures/Player.h"
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

int main() {
  GameSession gameSession{10, 10, std::make_shared<Player>(Point(2, 1), 10)};
  gameSession.getMap().placeWalls(Point(0, 0), Point(0, 9));
  gameSession.getMap().placeWalls(Point(0, 0), Point(9, 0));
  gameSession.getMap().placeWalls(Point(9, 0), Point(9, 9));
  gameSession.getMap().placeWalls(Point(0, 9), Point(9, 9));
  gameSession.getMap().placeWalls(Point(5, 0), Point(5, 5));
  gameSession.getMap().placeWalls(Point(3, 9), Point(3, 5));
  std::vector<std::shared_ptr<Item>> containerItems;

  containerItems.push_back(
      std::make_shared<Item>("sword", Item::ItemType::oneHanded));
  containerItems.push_back(std::make_shared<Item>("bow"));
  Container cont(std::move(containerItems), Point(0, 0),
                 "A battered and old looking wooden chest.");
  gameSession.getMap().placeFloor(std::make_unique<Container>(std::move(cont)),
                                  Point(2, 2));
  auto enemyLoot{std::make_shared<Item>("Banana", Item::ItemType::oneHanded)};
  auto npc{std::make_shared<NonPlayableCharacter>(
      'e', Point(4, 7), 12, std::vector<std::shared_ptr<Item>>{enemyLoot},
      "A dead body.")};
  gameSession.addNpc(npc);

  gameSession.getPlayer().takeItem(
      std::make_shared<Item>("Dagger", Item::ItemType::oneHanded));
  gameSession.displayMap();
  ScreenUtils::clearScreen();
  npc->takeDamage(12);
  gameSession.cleanDeadNpcs();

  while (true) {
    if (Input::hasKeyPressed()) {
      Command::command command{CommandHandler::getCommand(Input::getKey())};
      CommandHandler::executeWorldCommand(gameSession, command);
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(Settings::g_timeSleepMS));
  }
}

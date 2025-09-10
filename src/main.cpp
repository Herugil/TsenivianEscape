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
      std::make_shared<Weapon>("sword", Item::ItemType::oneHanded, 3, 1));
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
      "Gruffy-looking man", "A dead body.")};
  gameSession.addNpc(npc);

  gameSession.getPlayer().takeItem(
      std::make_shared<Weapon>("Dagger", Item::ItemType::oneHanded, 2, 1));
  gameSession.displayMap();

  while (true) {
    if (Input::hasKeyPressed()) {
      Command::command command{CommandHandler::getCommand(Input::getKey())};
      CommandHandler::executeWorldCommand(gameSession, command);
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(Settings::g_timeSleepMS));
  }
}

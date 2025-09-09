#ifdef _WIN32
#include <windows.h>
#endif

#include "Container.h"
#include "Directions.h"
#include "GameSession.h"
#include "Input.h"
#include "Item.h"
#include "Layer.h"
#include "Map.h"
#include "Player.h"
#include "Point.h"
#include "Settings.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

int main() {
  GameSession gameSession{10, 10, std::make_shared<Player>(Point(1, 1))};
  gameSession.getMap().placeWalls(Point(0, 0), Point(0, 9));
  gameSession.getMap().placeWalls(Point(0, 0), Point(9, 0));
  gameSession.getMap().placeWalls(Point(9, 0), Point(9, 9));
  gameSession.getMap().placeWalls(Point(0, 9), Point(9, 9));
  gameSession.getMap().placeWalls(Point(5, 0), Point(5, 5));
  gameSession.getMap().placeWalls(Point(3, 9), Point(3, 5));
  std::vector<std::shared_ptr<Item>> containerItems;

  containerItems.push_back(std::make_shared<Item>("sword"));
  containerItems.push_back(std::make_shared<Item>("bow"));
  Container cont(std::move(containerItems), Point(0, 0),
                 "A battered and old looking wooden chest.");
  gameSession.getMap().placeFloor(std::make_unique<Container>(std::move(cont)),
                                  Point(2, 2));

  gameSession.getPlayer().takeItem(std::make_shared<Item>("Dagger"));
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

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

void clearScreen() {
#ifdef _WIN32

  // Windows-specific clear screen
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hConsole != INVALID_HANDLE_VALUE) {
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
      FillConsoleOutputCharacter(hConsole, ' ', csbi.dwSize.X * csbi.dwSize.Y,
                                 coordScreen, &cCharsWritten);
      SetConsoleCursorPosition(hConsole, coordScreen);
    }
  }
#else
  // macOS/Linux - ANSI escape codes work fine
  std::cout << "\033[2J\033[H";
  std::cout.flush(); // Force immediate output
#endif
}

int main() {
  GameSession gameSession{10, 10, std::make_shared<Player>(Point(1, 1))};
  gameSession.getMap().placeWalls(Point(0, 0), Point(0, 9));
  gameSession.getMap().placeWalls(Point(0, 0), Point(9, 0));
  gameSession.getMap().placeWalls(Point(9, 0), Point(9, 9));
  gameSession.getMap().placeWalls(Point(0, 9), Point(9, 9));
  gameSession.getMap().placeWalls(Point(5, 0), Point(5, 5));
  gameSession.getMap().placeWalls(Point(3, 9), Point(3, 5));
  std::vector<std::unique_ptr<Item>> containerItems;

  containerItems.push_back(std::make_unique<Item>("sword"));
  containerItems.push_back(std::make_unique<Item>("bow"));
  Container cont(std::move(containerItems), Point(0, 0));
  cont.displayContents();
  gameSession.getMap().placeTop(std::make_shared<Container>(std::move(cont)),
                                Point(0, 0));

  gameSession.displayMap();
  while (true) {
    if (Input::hasKeyPressed()) {
      Directions::Direction dir = Input::getDirection();
      if (dir != Directions::nbDirections) {
        gameSession.movePlayer(dir);
        clearScreen();
        gameSession.displayMap();
      }
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(Settings::g_timeSleepMS));
  }
}

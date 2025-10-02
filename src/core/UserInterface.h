#pragma once
#include "Settings.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/creatures/Player.h"
#include <iostream>

namespace UserInterface {
struct NewGameResult {
  bool confirmed;
  std::string playerName;
};
struct LoadGameResult {
  bool confirmed;
  std::string saveSlot;
};
void displayCombatInterface(const Player &player);
NewGameResult newGame(bool inSession);
bool saveGameMenu(bool inSession, bool inCombat);
LoadGameResult loadGameMenu(const std::vector<std::string> &saveFiles);
int mainMenu(bool inSession);
} // namespace UserInterface
#include "core/UserInterface.h"
#include "utils/ScreenUtils.h"
#include <chrono>
#include <thread>

void UserInterface::displayCombatInterface(const Player &player) {
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

UserInterface::NewGameResult UserInterface::newGame(bool inSession) {
  ScreenUtils::clearScreen();
  std::string name;
  if (inSession) {
    std::cout << "Are you sure you want to start a new game?\n This will erase "
                 "all current progress.\n";
    std::cout << "Press 'e' to confirm or any other key to cancel: ";
    auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (!CommandHandler::isInteractionCommand(command)) {
      return NewGameResult{false, ""};
    }
  }
  while (true) {
    ScreenUtils::clearScreen();
    std::cout << "Enter your character's name: ";
    std::getline(std::cin, name);
    if (!name.empty()) {
      return NewGameResult{true, name};
    }
  }
}

bool UserInterface::saveGameMenu(bool inSession, bool inCombat) {
  ScreenUtils::clearScreen();
  if (inCombat) {
    std::cout << "You cannot save the game while in combat.\n Press any key to "
                 "continue.\n";
    Input::getKeyBlocking();
    return false;
  }
  if (!inSession) {
    std::cout << "You are not currently in a game session.\n Press any key to "
                 "continue.\n";
    Input::getKeyBlocking();
    return false;
  }
  return true;
}

UserInterface::LoadGameResult
UserInterface::loadGameMenu(const std::vector<std::string> &saveFiles) {
  ScreenUtils::clearScreen();
  if (saveFiles.empty()) {
    std::cout << "No saves available.\n";
    std::cout << "Press any key to return to main menu.\n";
    Input::getKeyBlocking();
    return LoadGameResult{false, ""};
  }
  std::cout << "Available saves:\n";
  for (std::size_t i = 0; i < saveFiles.size(); ++i) {
    std::cout << i + 1 << ": " << saveFiles[i] << '\n';
  }
  std::cout << "Select a save to load: ";
  auto loadCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
  auto loadPressedKey{
      static_cast<std::size_t>(CommandHandler::getPressedKey(loadCommand))};
  if (loadPressedKey > 0 && loadPressedKey <= saveFiles.size()) {
    return LoadGameResult{true, saveFiles[loadPressedKey]};
  } else {
    return LoadGameResult{false, ""};
  }
}

int UserInterface::mainMenu(bool inSession) {
  ScreenUtils::clearScreen();
  if (inSession) {
    std::cout << "Main Menu\n";
    std::cout << "1: New Game\n";
    std::cout << "2: Continue Game\n";
    std::cout << "3: Save Game\n";
    std::cout << "4: Load Game\n";
    std::cout << "5: Exit to Desktop\n";
  } else {
    std::cout << "Main Menu\n";
    std::cout << "1: New Game\n";
    std::cout << "2: Load Game\n";
    std::cout << "3: Exit to Desktop\n";
  }
  std::cout << "Press the corresponding number key to choose an option.\n";
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  return CommandHandler::getPressedKey(command);
}
#include "Input.h"
#include "core/GameSession.h"
#include "gameObjects/items/Item.h"
#include "input/Directions.h"
#include "utils/ScreenUtils.h"

#ifndef _WIN32
struct termios Input::originalTermios;
bool Input::termiosInitialized = false;
#endif

bool Input::hasKeyPressed() {
#ifdef _WIN32
  return _kbhit() != 0;
#else
  setNonBlockingMode(true);
  int ch = getchar();
  setNonBlockingMode(false);

  if (ch != EOF) {
    ungetc(ch, stdin);
    return true;
  }
  return false;
#endif
}

char Input::getKey() {
#ifdef _WIN32
  if (hasKeyPressed()) {
    return _getch();
  }
  return 0;
#else
  setNonBlockingMode(true);
  int ch = getchar();
  setNonBlockingMode(false);

  return (ch == EOF) ? 0 : static_cast<char>(ch);
#endif
}

char Input::getKeyBlocking() {
#ifdef _WIN32
  return _getch();
#else
  // Use the same terminal setup as getKey() but without non-blocking
  if (!termiosInitialized) {
    tcgetattr(STDIN_FILENO, &originalTermios);
    termiosInitialized = true;
  }

  struct termios newTermios = originalTermios;
  newTermios.c_lflag &= ~static_cast<unsigned long>((ICANON | ECHO));
  tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

  int ch = getchar();

  // Restore original settings
  tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);

  return static_cast<char>(ch);
#endif
}

#ifndef _WIN32
void Input::setNonBlockingMode(bool enable) {
  if (!termiosInitialized) {
    tcgetattr(STDIN_FILENO, &originalTermios);
    termiosInitialized = true;
  }

  if (enable) {
    struct termios newTermios = originalTermios;
    newTermios.c_lflag &= ~static_cast<unsigned long>((ICANON | ECHO));
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
  } else {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
  }
}
#endif

bool CommandHandler::isMovementCommand(Command::command cmd) {
  return cmd >= Command::top && cmd <= Command::right;
}
bool CommandHandler::isInteractionCommand(Command::command cmd) {
  return cmd == Command::interact;
}
bool CommandHandler::isTakeAllCommand(Command::command cmd) {
  return cmd == Command::takeAll;
}
bool CommandHandler::isHotkeyCommand(Command::command cmd) {
  return cmd >= Command::hotkey1 && cmd <= Command::hotkey9;
}
bool CommandHandler::isInventoryCommand(Command::command cmd) {
  return cmd == Command::inventory;
}
bool CommandHandler::isActionMenuCommand(Command::command cmd) {
  return cmd == Command::actionMenu;
}
bool CommandHandler::isShoveCommand(Command::command cmd) {
  return cmd == Command::shove;
}
bool CommandHandler::isShowEnemiesCommand(Command::command cmd) {
  return cmd == Command::showEnemies;
}
bool CommandHandler::isSkipTurnCommand(Command::command cmd) {
  return cmd == Command::skipTurn;
}
bool CommandHandler::isAttackCommand(Command::command cmd) {
  return cmd == Command::attack;
}
bool CommandHandler::isCharacterSheetCommand(Command::command cmd) {
  return cmd == Command::characterSheet;
}
bool CommandHandler::isMainMenuCommand(Command::command cmd) {
  return cmd == Command::mainMenu;
}

Command::command CommandHandler::getCommand(char pressedKey) {
  switch (pressedKey) {
  case CommandChar::top:
    return Command::top;
  case CommandChar::bottom:
    return Command::bottom;
  case CommandChar::left:
    return Command::left;
  case CommandChar::right:
    return Command::right;
  case CommandChar::interact:
    return Command::interact;
  case CommandChar::takeAll:
    return Command::takeAll;
  case CommandChar::hotkey1:
    return Command::hotkey1;
  case CommandChar::hotkey2:
    return Command::hotkey2;
  case CommandChar::hotkey3:
    return Command::hotkey3;
  case CommandChar::hotkey4:
    return Command::hotkey4;
  case CommandChar::hotkey5:
    return Command::hotkey5;
  case CommandChar::hotkey6:
    return Command::hotkey6;
  case CommandChar::hotkey7:
    return Command::hotkey7;
  case CommandChar::hotkey8:
    return Command::hotkey8;
  case CommandChar::hotkey9:
    return Command::hotkey9;
  case CommandChar::inventory:
    return Command::inventory;
  case CommandChar::actionMenu:
    return Command::actionMenu;
  case CommandChar::skipTurn:
    return Command::skipTurn;
  case CommandChar::showEnemies:
    return Command::showEnemies;
  case CommandChar::attack:
    return Command::attack;
  case CommandChar::shove:
    return Command::shove;
  case CommandChar::characterSheet:
    return Command::characterSheet;
  case CommandChar::mainMenu:
    return Command::mainMenu;
  default:
    return Command::nbCommands;
  }
}

int CommandHandler::getPressedKey(Command::command command) {
  // goes from 0 to 9
  return static_cast<int>(command - Command::hotkey1);
}

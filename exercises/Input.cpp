#include "Input.h"
#include "Directions.h"
#include "GameSession.h"
#include "Item.h"
#include "ScreenUtils.h"

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

bool isMovementCommand(Command::command cmd) {
  return cmd >= Command::top && cmd <= Command::right;
}
bool isInteractionCommand(Command::command cmd) {
  return cmd == Command::interact;
}
bool isTakeAllCommand(Command::command cmd) { return cmd == Command::takeAll; }
bool isHotkeyCommand(Command::command cmd) {
  return cmd >= Command::hotkey1 && cmd <= Command::hotkey9;
}
bool isInventoryCommand(Command::command cmd) {
  return cmd == Command::inventory;
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
  default:
    return Command::nbCommands;
  }
}

int getPressedKey(Command::command command) {
  // goes from 0 to 9
  return static_cast<int>(command - Command::hotkey1);
}

void CommandHandler::executeWorldCommand(GameSession &gameSession,
                                         Command::command command) {
  if (isMovementCommand(command)) {
    gameSession.movePlayer(static_cast<Directions::Direction>(command));
    ScreenUtils::clearScreen();
    gameSession.displayMap();
  } else if (isInteractionCommand(command)) {
    // TODO: the block below could be a nice function to get
    // directional input after a first input (eg attack, aim, etc)
    Command::command directionCommand{
        CommandHandler::getCommand(Input::getKeyBlocking())};
    auto directionInteract{
        static_cast<Directions::Direction>(directionCommand)};
    const Point adjPoint{
        gameSession.getPlayerPos().getAdjacentPoint(directionInteract)};
    ScreenUtils::clearScreen();
    gameSession.displayMap();
    gameSession.getMap().interactPoint(adjPoint, gameSession.getPlayer());
  } else if (isInventoryCommand(command)) {
    ScreenUtils::clearScreen();
    gameSession.getPlayer().inventoryMenu();
    ScreenUtils::clearScreen();
    gameSession.displayMap();
  } else {
    ScreenUtils::clearScreen();
    gameSession.displayMap();
  }
}

void CommandHandler::handleContainerCommands(Container &container,
                                             Player &player) {
  while (!container.getContents().empty()) {
    auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (isTakeAllCommand(command)) {
      player.takeAllItems(container);
      container.displayContents();
      break;
    } else if (isHotkeyCommand(command)) {
      auto pressedKey{static_cast<std::size_t>(getPressedKey(command))};
      auto item{container.popItem(pressedKey)};
      if (item) {
        player.takeItem(item);
        container.displayContents();
      }
    } else
      break;
  }
}

void CommandHandler::handleInventoryCommands(Player &player) {
  while (true) {
    ScreenUtils::clearScreen();
    player.displayInventory();
    auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (isHotkeyCommand(command)) {
      auto pressedKey{static_cast<std::size_t>(getPressedKey(command))};
      auto item{player.getItem(pressedKey)};
      if (item) {
        player.equipItem(item);
      }
    } else
      return;
  }
}
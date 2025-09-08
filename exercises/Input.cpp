#include "Input.h"
#include "Directions.h"
#include "GameSession.h"
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

// struct Command:
// one char member, one "command" enum listing all commands??

// getCommand() -> returns Command::command

// executeWorldCommand() -> for normal context(no combat, no menu etc) -> move
// if w a s d. If E, executes getCommand(), if w pressed, interact with top
// object, if s pressed bottom object, etc

bool isMovementCommand(Command::command cmd) {
  return cmd >= Command::top && cmd <= Command::right;
}
bool isInteractionCommand(Command::command cmd) {
  return cmd == Command::interact;
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
  default:
    return Command::nbCommands;
  }
}

void CommandHandler::executeWorldCommand(GameSession &gameSession,
                                         Command::command command) {
  if (isMovementCommand(command))
    gameSession.movePlayer(static_cast<Directions::Direction>(command));
  if (isInteractionCommand(command)) {
    Command::command directionCommand{
        CommandHandler::getCommand(Input::getKeyBlocking())};
    auto directionInteract{
        static_cast<Directions::Direction>(directionCommand)};
    const Point adjPoint{
        gameSession.getPlayerPos().getAdjacentPoint(directionInteract)};
    ScreenUtils::clearScreen();
    gameSession.displayMap();
    gameSession.getMap().interactPoint(adjPoint);
  }
}

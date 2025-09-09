#pragma once
#include "Directions.h"

// Platform-specific includes
#ifdef _WIN32
#include <conio.h>
#else
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#endif

class GameSession;
class Container;
class Player;

namespace CommandChar {
constexpr char top{'z'};
constexpr char bottom{'s'};
constexpr char right{'d'};
constexpr char left{'q'};
constexpr char interact{'e'};
constexpr char takeAll('t');
constexpr char hotkey1('1');
constexpr char hotkey2('2');
constexpr char hotkey3('3');
constexpr char hotkey4('4');
constexpr char hotkey5('5');
constexpr char hotkey6('6');
constexpr char hotkey7('7');
constexpr char hotkey8('8');
constexpr char hotkey9('9');
constexpr char inventory('i');
} // namespace CommandChar

struct Command {
  enum command {
    top,
    bottom,
    left,
    right,
    interact,
    takeAll,
    hotkey1,
    hotkey2,
    hotkey3,
    hotkey4,
    hotkey5,
    hotkey6,
    hotkey7,
    hotkey8,
    hotkey9,
    inventory,
    nbCommands,
  };
};

class Input {
public:
  static bool hasKeyPressed();
  static char getKey();
  static char getKeyBlocking();

#ifndef _WIN32
private:
  static void setNonBlockingMode(bool enable);
  static struct termios originalTermios;
  static bool termiosInitialized;
#endif
};

bool isMovementCommand(Command::command cmd);
bool isInteractionCommand(Command::command cmd);

namespace CommandHandler {
Command::command getCommand(char pressedKey);
void executeWorldCommand(GameSession &gameSession, Command::command command);
void handleContainerCommands(Container &container, Player &player);
void handleInventoryCommands(Player &player);
} // namespace CommandHandler

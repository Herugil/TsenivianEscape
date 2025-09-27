#pragma once
#include "input/Directions.h"

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
constexpr char takeAll{'t'};
constexpr char hotkey1{'1'};
constexpr char hotkey2{'2'};
constexpr char hotkey3{'3'};
constexpr char hotkey4{'4'};
constexpr char hotkey5{'5'};
constexpr char hotkey6{'6'};
constexpr char hotkey7{'7'};
constexpr char hotkey8{'8'};
constexpr char hotkey9{'9'};
constexpr char inventory{'i'};
constexpr char actionMenu{'a'};
constexpr char shove{'r'};
constexpr char attack{'f'};
constexpr char characterSheet{'c'};
constexpr char showEnemies{'\t'};
constexpr char skipTurn{' '};
constexpr char saveGame{'m'};
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
    actionMenu,
    attack,
    characterSheet,
    showEnemies,
    skipTurn,
    shove,
    saveGame,
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

namespace CommandHandler {
Command::command getCommand(char pressedKey);
int getPressedKey(Command::command command);
bool isMovementCommand(Command::command cmd);
bool isInteractionCommand(Command::command cmd);
bool isTakeAllCommand(Command::command cmd);
bool isHotkeyCommand(Command::command cmd);
bool isInventoryCommand(Command::command cmd);
bool isActionMenuCommand(Command::command cmd);
bool isShoveCommand(Command::command cmd);
bool isAttackCommand(Command::command cmd);
bool isShowEnemiesCommand(Command::command cmd);
bool isSkipTurnCommand(Command::command cmd);
bool isCharacterSheetCommand(Command::command cmd);
bool isSaveGameCommand(Command::command cmd);
} // namespace CommandHandler

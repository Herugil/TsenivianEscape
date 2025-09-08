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

namespace CommandChar {
constexpr char top{'z'};
constexpr char bottom{'s'};
constexpr char right{'d'};
constexpr char left{'q'};
constexpr char interact{'e'};
} // namespace CommandChar

struct Command {
  enum command {
    top,
    bottom,
    left,
    right,
    interact,
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
} // namespace CommandHandler
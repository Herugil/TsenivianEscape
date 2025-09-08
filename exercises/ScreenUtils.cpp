#include "ScreenUtils.h"
#include <iostream>

void ScreenUtils::clearScreen() {
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
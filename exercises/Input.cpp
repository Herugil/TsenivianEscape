#include "Input.h"

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

Directions::Direction Input::getDirection() {
    char key = getKey();
    switch (key) {
    case 'z': case 'Z':
        return Directions::top;
    case 's': case 'S':
        return Directions::bottom;
    case 'q': case 'Q':
        return Directions::left;
    case 'd': case 'D':
        return Directions::right;
    default:
        return Directions::nbDirections;
    }
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
    }
    else {
        tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
    }
}
#endif
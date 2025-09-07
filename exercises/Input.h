#pragma once
#include "Directions.h"

// Platform-specific includes
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#endif

class Input {
public:
    static bool hasKeyPressed();
    static char getKey();
    static Directions::Direction getDirection();

#ifndef _WIN32
private:
    static void setNonBlockingMode(bool enable);
    static struct termios originalTermios;
    static bool termiosInitialized;
#endif
};
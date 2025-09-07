#pragma once
#include "GameObject.h"
#include "Point.h"

class Wall :
    public GameObject
{
public:
    Wall(const Point& position): GameObject{false, false, 'X', position}{}
};


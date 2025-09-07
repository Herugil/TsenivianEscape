#pragma once
#include "Creature.h"
#include "Point.h"
#include "Directions.h"
#include "Map.h"

class Player :
    public Creature
{
public:
    Player(const Point& position);
};


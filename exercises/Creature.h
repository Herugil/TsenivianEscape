#pragma once
#include "GameObject.h"


// will have a lot of stats down the road
class Creature: public GameObject
{
public:
	Creature(char symbol, const Point& position);
};


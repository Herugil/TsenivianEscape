#pragma once
#include "gameObjects/GameObject.h"
#include "map/Point.h"

class Wall : public GameObject {
public:
  Wall(const Point &position)
      : GameObject{true,     false,  'X',
                   position, "Wall", "A simple wall of roughly hewn stone."} {}
};

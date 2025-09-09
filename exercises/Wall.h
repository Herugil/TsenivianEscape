#pragma once
#include "GameObject.h"
#include "Point.h"

class Wall : public GameObject {
public:
  Wall(const Point &position)
      : GameObject{true, false, 'X', position,
                   "A simple wall of roughly hewn stone."} {}
};

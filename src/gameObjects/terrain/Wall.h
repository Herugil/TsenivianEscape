#pragma once
#include "gameObjects/GameObject.h"
#include "map/Point.h"

class Wall : public GameObject {
public:
  Wall(const Point &position, std::string_view currentMap,
       std::string_view description)
      : GameObject{true,     false,  'X',        currentMap,
                   position, "Wall", description} {}
};

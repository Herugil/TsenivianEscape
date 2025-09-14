#pragma once
#include "gameObjects/GameObject.h"
#include "map/Point.h"
#include <string>

class GameSession;
class Creature;

class WalkOnObject : public GameObject {
protected:
public:
  WalkOnObject(std::string_view currentMap, Point position, char symbol = ' ',
               std::string_view name = "", std::string_view description = "")
      : GameObject(false, true, symbol, currentMap, position, name,
                   description) {}
  virtual void activateWalkOn(std::shared_ptr<GameObject> gameObject,
                              GameSession &gameSession) = 0;
};
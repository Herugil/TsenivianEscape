#pragma once
#include "gameObjects/GameObject.h"
#include "gameObjects/creatures/Player.h"
#include "map/Layer.h"
#include "map/Point.h"
#include <iostream>
#include <memory>
#include <vector>

class Map {
private:
  int m_width{};
  int m_height{};
  Layer<std::unique_ptr<GameObject>> m_floorLayer;
  Layer<std::weak_ptr<GameObject>> m_topLayer;

public:
  explicit Map(int width = 10, int height = 10);
  void placeFloor(std::unique_ptr<GameObject> gameObject, const Point &point);
  void placeTop(std::shared_ptr<GameObject> gameObject, const Point &point);
  void removeTop(const Point &point);
  void placeWalls(const Point &bottomLeft, const Point &topRight);
  void moveFloor(const Point &position, Directions::Direction direction);
  bool isAvailable(const Point &point) const;
  bool checkBounds(const Point &point) const;
  friend std::ostream &operator<<(std::ostream &out, const Map &map);
  void interactPoint(const Point &point, Player &player);
  std::shared_ptr<GameObject> getTopObject(const Point &point) const;
};

std::ostream &operator<<(std::ostream &out, const GameObject &gameObject);
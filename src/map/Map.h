#pragma once
#include "gameObjects/GameObject.h"
#include "gameObjects/creatures/Player.h"
#include "map/Layer.h"
#include "map/Point.h"
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

struct InteractionResult;

class Map {
private:
  std::string m_name{};
  int m_width{};
  int m_height{};
  std::string m_introText{};
  bool m_readIntroText{true};
  Layer<std::unique_ptr<GameObject>> m_floorLayer;
  Layer<std::weak_ptr<GameObject>> m_topLayer;

public:
  Map(std::string_view mapName, int width = 10, int height = 10,
      std::string_view introText = "");
  int getWidth() const;
  int getHeight() const;
  const std::string &getName() const;
  void placeFloor(std::unique_ptr<GameObject> gameObject, const Point &point);
  void placeTop(std::shared_ptr<GameObject> gameObject, const Point &point);
  void removeTop(const Point &point);
  void placeWalls(const Point &bottomLeft, const Point &topRight,
                  std::string_view description);
  void moveFloor(const Point &position, Directions::Direction direction);
  bool isAvailable(const Point &point) const;
  bool checkBounds(const Point &point) const;
  friend std::ostream &operator<<(std::ostream &out, Map &map);
  InteractionResult interactPoint(const Point &point);
  void setIntroTextRead(); // careful to always call this when entering a level
  std::shared_ptr<GameObject> getTopObject(const Point &point) const;
  GameObject *getFloorObject(const Point &point) const;
  std::deque<Point> reconstructPath(std::unordered_map<Point, Point> cameFrom,
                                    Point current) const;
  std::deque<Point> findPath(const Point &startPoint,
                             const Point &endPoint) const;
  bool isPointVisible(const Point &from, const Point &to) const;
  void printIntroText();
};

std::ostream &operator<<(std::ostream &out, const GameObject &gameObject);
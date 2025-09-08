#include "Map.h"
#include "Wall.h"
#include <iostream>
#include <memory>

Map::Map(int width, int height)
    : m_width{width}, m_height{height}, m_floorLayer(width, height),
      m_topLayer(width, height) {}

void Map::placeFloor(std::unique_ptr<GameObject> gameObject,
                     const Point &point) {
  if (checkBounds(point))
    m_floorLayer[point.getX(), point.getY()] = std::move(gameObject);
}

void Map::placeTop(std::shared_ptr<GameObject> gameObject, const Point &point) {
  if (checkBounds(point))
    m_topLayer[point.getX(), point.getY()] =
        gameObject; // implicit conversion to weak ptr
}

void Map::removeTop(const Point &point) {
  m_topLayer[point.getX(), point.getY()].reset();
}

bool Map::isAvailable(const Point &point) const {
  int x{point.getX()};
  int y{point.getY()};
  if (!checkBounds(point))
    return false;
  if (m_floorLayer[x, y] && !m_floorLayer[x, y]->isTraversable())
    return false;

  auto topObject = m_topLayer[x, y].lock();
  if (topObject && !topObject->isTraversable())
    return false;
  return true;
}

bool Map::checkBounds(const Point &point) const {
  return point.getX() >= 0 && point.getX() < m_width && point.getY() >= 0 &&
         point.getY() < m_height;
}

void Map::moveFloor(const Point &position, Directions::Direction direction)
// this is probably very rarely used, only if scripted events happen
// this is why the nature of the object being moved isnt a parameter
{
  int x{position.getX()};
  int y{position.getY()};
  Point destination{position.getAdjacentPoint(direction)};
  int destX{destination.getX()};
  int destY{destination.getY()};
  if (!m_floorLayer[x, y])
    return;
  if (m_floorLayer[x, y]->isMoveable() && isAvailable(destination)) {
    m_floorLayer[x, y]->setPosition(destination);
    m_floorLayer[destX, destY] = std::move(m_floorLayer[x, y]);
  }
  // this should call move semantics, because these are unique pointers?
}

void Map::placeWalls(const Point &bottomLeft, const Point &topRight) {
  int leftMin{std::min(bottomLeft.getX(), topRight.getX())};
  int bottomMin{std::min(bottomLeft.getY(), topRight.getY())};
  int rightMax{std::max(bottomLeft.getX(), topRight.getX())};
  int topMax{std::max(bottomLeft.getY(), topRight.getY())};
  for (int i{leftMin}; i <= rightMax; ++i) {
    for (int j{bottomMin}; j <= topMax; ++j) {
      placeFloor(std::make_unique<Wall>(Point(i, j)), Point(i, j));
    }
  }
}

std::ostream &operator<<(std::ostream &out, const Map &map) {
  for (int row{0}; row < map.m_height; ++row) {
    for (int col{0}; col < map.m_width; ++col) {
      auto topObject{map.m_topLayer[col, row].lock()};
      if (topObject)
        out << *topObject;
      else if (map.m_floorLayer[col, row])
        out << *(map.m_floorLayer[col, row]);
      else
        out << ' ';
      out << ' ';
    }
    out << '\n';
  }
  return out;
}

void Map::interactPoint(const Point &point) {
  int x{point.getX()};
  int y{point.getY()};
  if (checkBounds(point)) {
    auto topObject = m_topLayer[x, y].lock();
    if (topObject)
      topObject->playerInteraction();
    else if (m_floorLayer[x, y])
      m_floorLayer[x, y]->playerInteraction();
  }
}

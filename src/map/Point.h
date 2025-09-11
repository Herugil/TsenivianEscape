#pragma once
#include "input/Directions.h"
#include <functional>
class Point {
private:
  int m_x{};
  int m_y{};

public:
  explicit Point(int x = 0, int y = 0) : m_x{x}, m_y{y} {}
  int getX() const { return m_x; }
  int getY() const { return m_y; }
  Point getAdjacentPoint(Directions::Direction direction) const {
    switch (direction) {
    case Directions::top:
      return Point{m_x, m_y - 1};
    case Directions::bottom:
      return Point{m_x, m_y + 1};
    case Directions::right:
      return Point{m_x + 1, m_y};
    case Directions::left:
      return Point{m_x - 1, m_y};
    default:
      return *this;
    }
  };

  friend bool operator==(const Point &p1, const Point &p2) {
    return p1.m_x == p2.m_x && p1.m_y == p2.m_y;
  }
};

namespace std {
template <> struct hash<Point> {
  size_t operator()(const Point &p) const noexcept {
    return hash<int>()(p.getX()) ^ (hash<int>()(p.getY()) << 1);
  }
};
} // namespace std
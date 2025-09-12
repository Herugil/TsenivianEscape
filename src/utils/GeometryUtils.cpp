#include "GeometryUtils.h"
#include <cmath>

int GeometryUtils::distanceL1(const Point &p1, const Point &p2) {
  return std::abs(p1.getX() - p2.getX()) + std::abs(p1.getY() - p2.getY());
}

double GeometryUtils::distanceL2(const Point &p1, const Point &p2) {
  return std::sqrt(std::pow(p1.getX() - p2.getX(), 2) +
                   std::pow(p1.getY() - p2.getY(), 2));
}

Directions::Direction GeometryUtils::getRequiredDirection(const Point &p1,
                                                          const Point &p2) {
  // because movement should use the move function which requires a starting
  // point and a direction
  int xDelta{p2.getX() - p1.getX()};
  int yDelta{p2.getY() - p1.getY()};
  if (xDelta != 0 && yDelta != 0)
    // invalid move
    return Directions::nbDirections;
  if (xDelta > 0)
    return Directions::right;
  if (xDelta < 0)
    return Directions::left;
  if (yDelta > 0)
    return Directions::bottom;
  if (yDelta < 0)
    return Directions::top;
  return Directions::nbDirections;
}
#include "GeometryUtils.h"
#include <cmath>

int GeometryUtils::distanceL1(const Point &p1, const Point &p2) {
  return std::abs(p1.getX() - p2.getX()) + std::abs(p1.getY() - p2.getY());
}

double GeometryUtils::distanceL2(const Point &p1, const Point &p2) {
  return std::sqrt(std::pow(p1.getX() - p2.getX(), 2) +
                   std::pow(p1.getY() - p2.getY(), 2));
}
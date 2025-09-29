#include "GeometryUtils.h"
#include "core/GameSession.h"
#include <algorithm>
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
  if (std::abs(xDelta) >= std::abs(yDelta)) {
    // horizontal move
    if (xDelta > 0)
      return Directions::right;
    if (xDelta < 0)
      return Directions::left;
  } else if (std::abs(yDelta) > std::abs(xDelta)) {
    // vertical move
    if (yDelta > 0)
      return Directions::bottom;
    if (yDelta < 0)
      return Directions::top;
  }
  return Directions::nbDirections;
}

std::vector<Point> drawStraightLineLow(const Point &p1, const Point &p2) {
  std::vector<Point> points;
  int x1{p1.getX()};
  int x2{p2.getX()};
  int y{p1.getY()};
  int dx{x2 - x1};
  int dy{p2.getY() - y};
  int yi{1};
  if (dy < 0) {
    yi = -1;
    dy = -dy;
  }
  int difference{2 * dy - dx};
  for (int x{x1}; x <= x2; ++x) {
    points.emplace_back(Point(x, y));
    if (difference > 0) {
      y += yi;
      difference += 2 * (dy - dx);
    } else
      difference += 2 * dy;
  }
  return points;
}

std::vector<Point> drawStraightLineHigh(const Point &p1, const Point &p2) {
  std::vector<Point> points;
  int dx{p2.getX() - p1.getX()};
  int dy{p2.getY() - p1.getY()};
  int xi{1};
  if (dx < 0) {
    xi = -1;
    dx = -dx;
  }
  int difference{2 * dx - dy};
  int x{p1.getX()};

  for (int y{p1.getY()}; y <= p2.getY(); ++y) {
    points.emplace_back(Point(x, y));
    if (difference > 0) {
      x += xi;
      difference += 2 * (dx - dy);
    } else
      difference += 2 * dx;
  }
  return points;
}

std::vector<Point> GeometryUtils::drawStraightLine(const Point &p1,
                                                   const Point &p2) {
  // see https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
  int x1{p1.getX()};
  int x2{p2.getX()};
  int y1{p1.getY()};
  int y2{p2.getY()};
  if (std::abs(y2 - y1) < std::abs(x2 - x1)) {
    if (x1 > x2) {
      auto res{drawStraightLineLow(p2, p1)};
      std::reverse(res.begin(), res.end());
      return res;
    } else
      return drawStraightLineLow(p1, p2);
  } else {
    if (y1 > y2) {
      auto res{drawStraightLineHigh(p2, p1)};
      std::reverse(res.begin(), res.end());
      return res;
    } else {
      return drawStraightLineHigh(p1, p2);
    }
  }
}

std::deque<Point> GeometryUtils::sortPointsAndFindPath(
    std::vector<Point> points, const Point &startPoint,
    const GameSession &gameSession, const Point &comparisonPoint,
    bool closest) {
  // This function sorts the points by distance to startPoint and
  // returns the path to the closest (or farthest if closest is false)
  // from comparisonPoint
  std::sort(
      points.begin(), points.end(),
      [startPoint, closest, comparisonPoint](const Point &p1, const Point &p2) {
        if (distanceL2(p1, comparisonPoint) == distanceL2(p2, comparisonPoint))
          return (GeometryUtils::distanceL2(startPoint, p1) <=
                  GeometryUtils::distanceL2(startPoint, p2)

          );
        return (closest ? distanceL2(p1, comparisonPoint) <
                              distanceL2(p2, comparisonPoint)
                        : distanceL2(p1, comparisonPoint) >
                              distanceL2(p2, comparisonPoint));
      });
  for (auto safePoint : points) {
    std::deque<Point> res{gameSession.getMap().findPath(startPoint, safePoint)};
    if (!res.empty()) {
      res.pop_front(); // this is the current point! unneeded
      return res;
    }
  }
  return {};
}
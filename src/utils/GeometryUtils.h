#pragma once
#include "input/Directions.h"
#include "map/Point.h"
#include <queue>
#include <vector>

class GameSession;

namespace GeometryUtils {
int distanceL1(const Point &p1, const Point &p2);
double distanceL2(const Point &p1, const Point &p2);
Directions::Direction getRequiredDirection(const Point &p1, const Point &p2);
std::vector<Point> drawStraightLine(const Point &p1, const Point &p2);
std::deque<Point> sortPointsAndFindPath(std::vector<Point> points,
                                        const Point &startPoint,
                                        const GameSession &gameSession);
} // namespace GeometryUtils
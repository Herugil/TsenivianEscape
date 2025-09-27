#include "map/Map.h"
#include "core/GameStateManager.h"
#include "gameObjects/terrain/Wall.h"
#include "input/Input.h"
#include "utils/GeometryUtils.h"
#include "utils/QueueClass.h"
#include "utils/ScreenUtils.h"
#include <iostream>
#include <memory>
#include <unordered_map>

Map::Map(std::string_view mapName, int width, int height,
         std::string_view introText)
    : m_name{mapName}, m_width{width}, m_height{height}, m_introText{introText},
      m_floorLayer(width, height), m_topLayer(width, height) {}

int Map::getWidth() const { return m_width; }
int Map::getHeight() const { return m_height; }
const std::string &Map::getName() const { return m_name; }

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

void Map::placeWalls(const Point &bottomLeft, const Point &topRight,
                     std::string_view description) {
  int leftMin{std::min(bottomLeft.getX(), topRight.getX())};
  int bottomMin{std::min(bottomLeft.getY(), topRight.getY())};
  int rightMax{std::max(bottomLeft.getX(), topRight.getX())};
  int topMax{std::max(bottomLeft.getY(), topRight.getY())};
  for (int i{leftMin}; i <= rightMax; ++i) {
    for (int j{bottomMin}; j <= topMax; ++j) {
      placeFloor(std::make_unique<Wall>(Point(i, j), m_name, description),
                 Point(i, j));
    }
  }
}

std::ostream &operator<<(std::ostream &out, Map &map) {
  if (map.m_readIntroText) {
    map.printIntroText();
    map.setVisited();
  }
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

void Map::setVisited() {
  m_readIntroText = false;
  m_visited = true;
}

bool Map::hasBeenVisited() const { return m_visited; }

InteractionResult Map::interactPoint(const Point &point) {
  int x{point.getX()};
  int y{point.getY()};
  if (checkBounds(point)) {
    auto topObject = m_topLayer[x, y].lock();
    if (topObject)
      return topObject->playerInteraction();
    else if (m_floorLayer[x, y])
      return m_floorLayer[x, y]->playerInteraction();
  }
  return InteractionResult{GameState::Exploration, nullptr};
  // this is a failsafe. state manager will change
  // to combat state if needed after this function call
}

std::shared_ptr<GameObject> Map::getTopObject(const Point &point) const {
  if (!checkBounds(point))
    return nullptr;
  return m_topLayer[point.getX(), point.getY()].lock();
}

GameObject *Map::getFloorObject(const Point &point) const {
  // returns raw pointer to floor object
  // ownership isnt transferred
  if (!checkBounds(point))
    return nullptr;
  return m_floorLayer[point.getX(), point.getY()].get();
}

struct ComparatorFunc {
  std::unordered_map<Point, int> &fScore;
  bool operator()(const Point &p1, const Point &p2) const {
    return fScore[p1] >= fScore[p2];
  }
};

std::deque<Point>
Map::reconstructPath(std::unordered_map<Point, Point> cameFrom,
                     Point current) const {
  std::deque<Point> path{current};
  while (cameFrom.contains(current)) {
    Point next{cameFrom[current]};
    path.push_front(next);
    current = next;
  }
  return path;
}

std::deque<Point> Map::findPath(const Point &startPoint,
                                const Point &endPoint) const {
  // implementation of A* (see
  // https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode)
  // with map specific in minds (accessible tiles in the grid)
  if (!isAvailable(endPoint) || (startPoint == endPoint))
    return {};
  // this probably wouldnt be called, but who knows
  std::unordered_map<Point, Point> cameFrom{};
  std::unordered_map<Point, int> gScore{};
  // gScore stores score to get from start to key
  std::unordered_map<Point, int> fScore{};
  ComparatorFunc comp{fScore};
  MyQueue<Point, std::vector<Point>, ComparatorFunc> openSet(comp);
  // fScore is the guessed cost to get to the objective starting from key
  gScore[startPoint] = 0;
  fScore[startPoint] = GeometryUtils::distanceL1(startPoint, endPoint);
  openSet.emplace(startPoint);

  while (!openSet.empty()) {
    Point currentPoint{openSet.top()};
    if (currentPoint == endPoint)
      return reconstructPath(cameFrom, currentPoint);
    openSet.pop();
    for (int i{0}; i < Directions::nbDirections; ++i) {
      auto direction{static_cast<Directions::Direction>(i)};
      Point neighbour{currentPoint.getAdjacentPoint(direction)};
      if (!isAvailable(neighbour)) {
        continue;
      }
      int tentativeGScore{gScore[currentPoint] + 1}; // 1 is a magic number
      // and is the cost of moving from a point to its neighbour.
      // should be replaced with neighbour.getMoveCost() once
      // difficult terrain is implemented.
      if ((gScore.contains(neighbour) && tentativeGScore < gScore[neighbour]) ||
          (!gScore.contains(neighbour))) {
        cameFrom[neighbour] = currentPoint;
        gScore[neighbour] = tentativeGScore;
        fScore[neighbour] =
            tentativeGScore + GeometryUtils::distanceL1(neighbour, endPoint);
        if (!openSet.contains(neighbour)) {
          openSet.emplace(neighbour);
        }
      }
    }
  }
  return {};
}

bool Map::isPointVisible(const Point &from, const Point &to) const {
  if (!checkBounds(from) || !checkBounds(to))
    return false;
  if (from == to)
    return true;
  auto linePoints{GeometryUtils::drawStraightLine(from, to)};
  linePoints.erase(linePoints.begin()); // remove starting point
  linePoints.pop_back();                // remove end point
  for (const auto &point : linePoints) {
    if (!isAvailable(point))
      // this is only a first approximation, ideally should check for
      // visibility blocking property (can shoot across creatures or difficult
      // terrain, but not walls for example)
      return false;
  }
  return true;
}

void Map::printIntroText() {
  ScreenUtils::clearScreen();
  std::cout << m_introText << '\n';
  std::cout << "Press any key to continue...\n";
  Input::getKeyBlocking();
  ScreenUtils::clearScreen();
}

json Map::toJson() const {
  json j;
  j["introTextRead"] = m_readIntroText;
  // every object in the top layer is owned by GameSession
  // therefore only floor layer needs to be saved
  j["floorLayer"] = json::array();
  for (int y{0}; y < m_height; ++y) {
    for (int x{0}; x < m_width; ++x) {
      if (m_floorLayer[x, y]) {
        json floorTileJson = m_floorLayer[x, y]->toJson();
        if (!floorTileJson.is_null())
          j["floorLayer"].push_back(floorTileJson);
      }
    }
  }
  return j;
}

void Map::updateFromJson(
    const json &j,
    const std::unordered_map<std::string, std::shared_ptr<Item>> &allItems) {
  for (const auto &item : j.at("floorLayer")) {
    Point pos{item["position"][0], item["position"][1]};
    auto currentFloorObject{getFloorObject(pos)};
    if (currentFloorObject && currentFloorObject->getName() == item["name"]) {
      if (item.contains("locked") && !item["locked"])
        // for now player cant lock objects (seems useless)
        currentFloorObject->unlock();
      if (item.contains("contents")) {
        if (auto container{dynamic_cast<Container *>(currentFloorObject)}) {
          // above cast should never fail
          container->clearContents();
          for (const auto &itemId : item["contents"]) {
            if (allItems.contains(itemId))
              container->addItem(allItems.at(itemId)->clone());
            else
              std::cout << itemId
                        << " not added to container, cant find it in items.\n";
            // this should never happen
          }
        } else
          throw std::runtime_error(
              "Error updating map from json: object with contents is not a "
              "container.");
      }
      if (item.contains("hasBeenUsed"))
        if (item["hasBeenUsed"])
          currentFloorObject->setUsed();
    }
  }
}

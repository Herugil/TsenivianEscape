#include "gameObjects/terrain/MapChanger.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/creatures/Player.h"
#include "map/Map.h"
#include "utils/GeometryUtils.h"
#include <unordered_map>

MapChanger::MapChanger(std::string_view currentMap, Point position,
                       std::string_view targetMap, Point spawningPoint,
                       char symbol, std::string_view name,
                       std::string_view description, bool locked,
                       std::string_view keyId)
    : WalkOnObject(currentMap, position, symbol, name, description),
      m_targetMap{targetMap}, m_spawningPoint{spawningPoint} {
  m_locked = locked;
  m_keyId = keyId;
  if (locked)
    m_traversable = false;
}

void MapChanger::activateWalkOn(std::shared_ptr<GameObject> gameObject,
                                GameSession &gameSession) {
  if (m_locked)
    return;
  if (gameSession.getMap().getName() == m_targetMap) {
    return;
  }
  if (auto player{std::dynamic_pointer_cast<Player>(gameObject)}) {
    if (player->inCombat()) {
      return;
    }
    gameSession.getMap().removeTop(player->getPosition());
    gameSession.setCurrentMap(m_targetMap);
    player->setPosition(m_spawningPoint);
    gameSession.getMap().placeTop(player, m_spawningPoint);
    gameSession.getMap().setVisited();
    gameSession.displayMap();
    return;
  } else if (auto creature{
                 std::dynamic_pointer_cast<NonPlayableCharacter>(gameObject)}) {
    auto &targetMap = gameSession.getMap(m_targetMap);
    Point destPoint{m_spawningPoint};
    if (targetMap.isAvailable(destPoint)) {
      gameSession.getMap().removeTop(creature->getPosition());
      creature->unsetCombat();
      for (int i = 0; i < targetMap.getHeight(); ++i) {
        for (int j = 0; j < targetMap.getWidth(); ++j) {
          if (!targetMap.isAvailable(Point{i, j}) ||
              GeometryUtils::distanceL1(Point{i, j}, m_spawningPoint) > 5)
            continue;
          auto path{targetMap.findPath(m_spawningPoint, Point{i, j})};
          if (path.size() >= 2)
            destPoint = Point{i, j};
        }
      }
      creature->setCurrentMap(m_targetMap);
      targetMap.placeTop(creature, destPoint);
      // if a creature changed maps, map state should be saved
      targetMap.setVisited();
      creature->setPosition(destPoint);
      return;
    }
  }
}

void MapChanger::unlock() {
  m_locked = false;
  m_traversable = true;
}
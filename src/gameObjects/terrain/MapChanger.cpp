#include "gameObjects/terrain/MapChanger.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/creatures/Player.h"
#include "map/Map.h"
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
    for (int i{0}; i < Directions::nbDirections; ++i) {
      auto adjPoint{m_spawningPoint.getAdjacentPoint(
          static_cast<Directions::Direction>(i))};
      if (gameSession.getMap(m_targetMap).isAvailable(adjPoint)) {
        gameSession.getMap().removeTop(creature->getPosition());
        creature->unsetCombat();
        creature->setCurrentMap(m_targetMap);
        gameSession.getMap(m_targetMap).placeTop(creature, adjPoint);
        // if a creature changed maps, map state should be saved
        gameSession.getMap(m_targetMap).setVisited();
        creature->setPosition(adjPoint);
        creature->resetTurn();
        creature->setDefaultBehavior();
        // if creature was fleeing, it shouldnt be anymore
        return;
      }
    }
  }
}
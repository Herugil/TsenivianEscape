#include "gameObjects/terrain/MapChanger.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/creatures/Player.h"
#include "map/Map.h"
#include <unordered_map>

MapChanger::MapChanger(std::string_view currentMap, Point position,
                       std::string_view targetMap, Point spawningPoint,
                       char symbol, std::string_view name,
                       std::string_view description)
    : WalkOnObject(currentMap, position, symbol, name, description),
      m_targetMap{targetMap}, m_spawningPoint{spawningPoint} {}

void MapChanger::activateWalkOn(std::shared_ptr<GameObject> gameObject,
                                GameSession &gameSession) {
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
    gameSession.displayMap();
    return;
  } else if (auto creature{std::dynamic_pointer_cast<Creature>(gameObject)}) {
    gameSession.getMap().removeTop(creature->getPosition());
    gameSession.getMap(m_targetMap).placeTop(creature, m_spawningPoint);
    creature->setPosition(m_spawningPoint);
  }
}
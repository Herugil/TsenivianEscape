#include "core/GameSession.h"
#include "gameObjects/terrain/MapChanger.h"
#include "gameObjects/terrain/WalkOnObject.h"
#include "map/Map.h"
#include "map/Point.h"
#include "utils/ScreenUtils.h"

GameSession::GameSession(std::shared_ptr<Player> player)
    : m_player{std::move(player)} {
  m_allMaps.emplace("placeholder", Map{"placeholder"});
  m_currentMap = &m_allMaps.at("placeholder");
}

void GameSession::respawnPlayer() {
  m_currentMap->placeTop(m_player, m_player->getPosition());
}

void GameSession::moveCreature(std::shared_ptr<GameObject> gameObject,
                               Directions::Direction direction, bool forced) {
  // TODO: add actor parameter, rename function
  if (direction == Directions::nbDirections)
    // invalid direction
    return;
  Point currentPos{gameObject->getPosition()};
  Point adjPoint(currentPos.getAdjacentPoint(direction));
  if (m_currentMap->isAvailable(adjPoint)) {
    bool canMove{false};
    if (forced)
      canMove = gameObject->isMoveable();
    else if (auto creature{std::dynamic_pointer_cast<Creature>(gameObject)})
      canMove = creature->useMovementPoints();
    if (canMove) {
      m_currentMap->placeTop(gameObject, adjPoint);
      m_currentMap->removeTop(currentPos);
      gameObject->setPosition(adjPoint);
      if (auto walkOn{dynamic_cast<WalkOnObject *>(
              getMap().getFloorObject(adjPoint))}) {
        walkOn->activateWalkOn(gameObject, *this);
      }
    }
  }
}

void GameSession::displayMap() const { std::cout << *m_currentMap; }

const Point &GameSession::getPlayerPos() const {
  return m_player->getPosition();
}

Player &GameSession::getPlayer() { return *m_player; }
Map &GameSession::getMap() { return *m_currentMap; }
const Map &GameSession::getMap() const { return *m_currentMap; }
std::shared_ptr<Player> GameSession::getPlayerPtr() const { return m_player; }

void GameSession::addNpc(std::shared_ptr<NonPlayableCharacter> npc) {
  m_npcs.push_back(npc);
  m_currentMap->placeTop(npc, npc->getPosition());
}

void GameSession::addContainer(std::shared_ptr<Container> container) {
  m_sessionOwnedContainers.push_back(container);
  m_currentMap->placeTop(container, container->getPosition());
}

void GameSession::removeContainer(std::shared_ptr<Container> container) {
  for (auto it{m_sessionOwnedContainers.begin()};
       it != m_sessionOwnedContainers.end();) {
    if (*it == container) {
      // this is the container to remove.
      // first remove the weak ptr of the top layer of map
      // then remove shared ptr from the vector
      // will go out of scope after the scope of the call of this function
      // because shared ptr has been passed as an arg
      // so after the return the container probably lives somewhere!
      getMap().removeTop((*it)->getPosition());
      it = m_sessionOwnedContainers.erase(it);
      return;
    } else
      ++it;
  }
}

void GameSession::cleanDeadNpcs() {
  for (auto it{m_npcs.begin()}; it != m_npcs.end();) {
    if ((*it)->isDead()) {
      std::cout << (*it)->getName() << " is dead.\n";
      auto lootableBody{std::make_shared<Container>(
          (*it)->getInventory(), (*it)->getPosition(), m_currentMap->getName(),
          (*it)->getName() + "'s body", (*it)->getDeadDescription())};
      addContainer(lootableBody);
      m_currentMap->removeTop((*it)->getPosition());
      m_currentMap->placeTop(lootableBody, lootableBody->getPosition());
      it = m_npcs.erase(it);
    } else
      ++it;
  }
}

const std::vector<std::shared_ptr<NonPlayableCharacter>> &
GameSession::getNpcs() const {
  return m_npcs;
}

bool GameSession::enemiesInMap() const {
  for (auto npc : m_npcs) {
    if (npc->getCurrentMap() == m_currentMap->getName())
      return true;
  }
  return false;
}

void GameSession::initializeTurnOrder() {
  if (m_turnOrder.size() <= 1) { // player is alone in initiative
    // at the start of the game
    m_turnOrder.clear();
    m_turnOrder.push_back(m_player);
    for (std::weak_ptr<Creature> npc : m_npcs) {
      if (!npc.expired()) {
        if (npc.lock()->getCurrentMap() == m_currentMap->getName())
          m_turnOrder.push_back(npc);
      }
    }
  } else {
    for (auto it{m_turnOrder.begin()}; it != m_turnOrder.end();) {
      auto lockedCharacter{(*it).lock()};
      if (!lockedCharacter)
        it = m_turnOrder.erase(it);
      else
        ++it;
    }
  }
}

std::vector<std::weak_ptr<Creature>> GameSession::getTurnOrder() const {
  return m_turnOrder;
}

void GameSession::addMap(Map &&map) {
  m_allMaps.emplace(map.getName(), std::move(map));
}

void GameSession::setCurrentMap(std::string_view mapName) {
  std::string strMapName{mapName};
  if (!m_allMaps.contains(strMapName)) {
    std::cout << "Can't find " << mapName << ", not changing maps.\n";
    return;
  }
  m_currentMap->removeTop(m_player->getPosition());
  m_currentMap = &m_allMaps.at(strMapName);
  m_player->setCurrentMap(strMapName);
  initializeTurnOrder();
  m_player->resetTurn();
}

Map &GameSession::getMap(std::string_view mapName) {
  std::string strMapName{mapName};
  // not very dry
  if (!m_allMaps.contains(strMapName)) {
    std::cout << "Can't find " << mapName << ", not changing maps.\n";
    return *m_currentMap;
  }
  return m_allMaps.at(strMapName);
}

void GameSession::resetInitiative() { m_turnOrder.clear(); }
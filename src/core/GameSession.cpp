#include "core/GameSession.h"
#include "gameObjects/creatures/Player.h"
#include "map/Map.h"
#include "map/Point.h"
#include "utils/ScreenUtils.h"

GameSession::GameSession(int width, int height, std::shared_ptr<Player> player)
    : m_player{std::move(player)}, m_currentMap{width, height} {
  Point point{m_player->getPosition()};
  m_currentMap.placeTop(m_player, point);
}

void GameSession::movePlayer(Directions::Direction direction) {
  // TODO: add actor parameter, rename function
  bool canMove{m_player->canMove(1)};
  if (!canMove) {
    if (m_player->canAct(1)) {
      m_player->refillMovementPoints();
      canMove = true;
    }
  }
  if (canMove) {
    Point currentPos{m_player->getPosition()};
    Point adjPoint(currentPos.getAdjacentPoint(direction));
    if (m_currentMap.isAvailable(adjPoint)) {
      m_currentMap.placeTop(m_player, adjPoint);
      m_currentMap.removeTop(currentPos);
      m_player->setPosition(adjPoint);
    }
  }
}

void GameSession::displayMap() const { std::cout << m_currentMap; }

const Point &GameSession::getPlayerPos() const {
  return m_player->getPosition();
}

Player &GameSession::getPlayer() { return *m_player; }
Map &GameSession::getMap() { return m_currentMap; }

void GameSession::addNpc(std::shared_ptr<NonPlayableCharacter> npc) {
  m_npcs.push_back(npc);
  m_currentMap.placeTop(npc, npc->getPosition());
}

void GameSession::addContainer(std::shared_ptr<Container> container) {
  m_sessionOwnedContainers.push_back(container);
  m_currentMap.placeTop(container, container->getPosition());
}

void GameSession::cleanDeadNpcs() {
  for (auto it{m_npcs.begin()}; it != m_npcs.end();) {
    if ((*it)->isDead()) {
      auto lootableBody{std::make_shared<Container>(
          (*it)->getInventory(), (*it)->getPosition(),
          (*it)->getDeadDescription())};
      addContainer(lootableBody);
      m_currentMap.removeTop((*it)->getPosition());
      m_currentMap.placeTop(lootableBody, lootableBody->getPosition());
      it = m_npcs.erase(it);
    } else
      ++it;
  }
}

const std::vector<std::shared_ptr<NonPlayableCharacter>> &
GameSession::getNpcs() const {
  return m_npcs;
}

bool GameSession::enemiesInMap() const { return !m_npcs.empty(); }
#include "GameSession.h"
#include "Map.h"
#include "Player.h"
#include "Point.h"
#include "ScreenUtils.h"

GameSession::GameSession(int width, int height, std::shared_ptr<Player> player)
    : m_player{std::move(player)}, m_currentMap{width, height} {
  Point point{m_player->getPosition()};
  m_currentMap.placeTop(m_player, point);
}

void GameSession::movePlayer(Directions::Direction direction) {
  Point currentPos{m_player->getPosition()};
  Point adjPoint(currentPos.getAdjacentPoint(direction));
  if (m_currentMap.isAvailable(adjPoint)) {
    m_currentMap.placeTop(m_player, adjPoint);
    m_currentMap.removeTop(currentPos);
    m_player->setPosition(adjPoint);
    ScreenUtils::clearScreen();
    displayMap();
  }
}

void GameSession::displayMap() const { std::cout << m_currentMap; }

const Point &GameSession::getPlayerPos() const {
  return m_player->getPosition();
}

Map &GameSession::getMap() { return m_currentMap; }

#include "GameSession.h"
#include "Player.h"
#include "Point.h"
#include "Map.h"


GameSession::GameSession(int width, int height, std::shared_ptr<Player> player) :
	m_currentMap{ width, height }, m_player{std::move(player)}{
	Point point{ m_player->getPosition() };
	m_currentMap.placeTop(m_player, point);
}

void GameSession::movePlayer(Directions::Direction direction) {
	Point currentPos{ m_player->getPosition() };
	Point adjPoint(currentPos.getAdjacentPoint(direction));
	if (m_currentMap.isAvailable(adjPoint)) {
		m_currentMap.placeTop(m_player, adjPoint);
		m_currentMap.removeTop(currentPos);
		m_player->setPosition(adjPoint);
	}
}

void GameSession::displayMap() const {
	std::cout << m_currentMap;
}

Map& GameSession::getMap() {
	return m_currentMap;
}
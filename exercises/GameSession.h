#pragma once
#include "Directions.h"
#include "Player.h"
#include "Map.h"
#include <memory>
class GameSession
{
	std::shared_ptr<Player> m_player;
	Map m_currentMap{};
public:
	GameSession(int width, int height, std::shared_ptr<Player> player);
	void movePlayer(Directions::Direction direction);
	void displayMap() const;
	Map& getMap();
};


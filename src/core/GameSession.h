#pragma once
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/creatures/Player.h"
#include "gameObjects/terrain/Container.h"
#include "input/Directions.h"
#include "map/Map.h"
#include "map/Point.h"
#include <memory>
#include <vector>

class GameSession {
  std::shared_ptr<Player> m_player{};
  std::vector<std::shared_ptr<NonPlayableCharacter>> m_npcs{};
  std::vector<std::shared_ptr<Container>> m_sessionOwnedContainers{};
  std::vector<std::weak_ptr<Creature>> m_turnOrder{};
  // when NPCs die, containers pop on the top layer, which
  // is made of weak ptrs. to ensure they're not left dangling, gameSession
  // handles their existence.
  Map m_currentMap{};

public:
  GameSession(int width, int height, std::shared_ptr<Player> player);
  void movePlayer(Directions::Direction direction);
  void displayMap() const;
  const Point &getPlayerPos() const;
  void addNpc(std::shared_ptr<NonPlayableCharacter> npc);
  void addContainer(std::shared_ptr<Container> container);
  void cleanDeadNpcs();
  const std::vector<std::shared_ptr<NonPlayableCharacter>> &getNpcs() const;
  std::vector<std::weak_ptr<Creature>> getTurnOrder() const;
  bool enemiesInMap() const;
  Player &getPlayer();
  Map &getMap();

  void initializeTurnOrder();
};

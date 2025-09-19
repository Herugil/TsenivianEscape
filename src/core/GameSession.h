#pragma once
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/creatures/Player.h"
#include "gameObjects/terrain/Container.h"
#include "input/Directions.h"
#include "map/Map.h"
#include "map/Point.h"
#include <memory>
#include <sstream>
#include <vector>

class GameSession {
  std::shared_ptr<Player> m_player{};
  std::vector<std::shared_ptr<NonPlayableCharacter>> m_npcs{};
  std::vector<std::shared_ptr<Container>> m_sessionOwnedContainers{};
  std::vector<std::weak_ptr<Creature>> m_turnOrder{};
  // when NPCs die, containers pop on the top layer, which
  // is made of weak ptrs. to ensure they're not left dangling, gameSession
  // handles their existence.
  std::unordered_map<std::string, Map> m_allMaps;
  std::size_t m_currentTurnIndex{0};
  Map *m_currentMap{nullptr};

public:
  GameSession(std::shared_ptr<Player> player);
  void moveCreature(std::shared_ptr<GameObject> gameObject,
                    Directions::Direction direction, bool forced = false);
  void respawnPlayer();
  void displayMap() const;
  const Point &getPlayerPos() const;
  void addNpc(std::shared_ptr<NonPlayableCharacter> npc);
  void addContainer(std::shared_ptr<Container> container);
  void dropItem(std::shared_ptr<Item> item, const Point &point);
  void removeContainer(std::shared_ptr<Container> container);
  std::string cleanDeadNpcs();
  const std::vector<std::shared_ptr<NonPlayableCharacter>> &getNpcs() const;
  std::vector<std::weak_ptr<Creature>> getTurnOrder() const;
  bool enemiesInMap() const;
  std::shared_ptr<Player> getPlayerPtr() const; // useful in some cases where
  // player ptr is required as a parameter (eg move)
  Player &getPlayer();
  Map &getMap();
  const Map &getMap() const;
  void addMap(Map &&map);
  void setCurrentMap(std::string_view mapName);
  Map &getMap(std::string_view mapName);
  void incrementTurnIndex();
  std::weak_ptr<Creature> getActiveCreature() const;
  void initializeTurnOrder();
  void resetInitiative();

  std::vector<std::weak_ptr<NonPlayableCharacter>> getEnemiesInMap() const;
  void displayEnemiesInMap() const;
};

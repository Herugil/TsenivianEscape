#include "core/GameSession.h"
#include "gameObjects/terrain/MapChanger.h"
#include "gameObjects/terrain/WalkOnObject.h"
#include "map/Map.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include "utils/ScreenUtils.h"
#include <sstream>

using json = nlohmann::json;

GameSession::GameSession(std::shared_ptr<Player> player)
    : m_player{std::move(player)} {
  m_allMaps.emplace("placeholder", Map{"placeholder"});
  m_currentMap = &m_allMaps.at("placeholder");
}

void GameSession::respawnPlayer() {
  m_currentMap->placeTop(m_player, m_player->getPosition());
  m_currentMap->setVisited();
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
      canMove = !creature->inCombat() || creature->useMovementPoints();
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
const Player &GameSession::getPlayer() const { return *m_player; }
Map &GameSession::getMap() { return *m_currentMap; }
const Map &GameSession::getMap() const { return *m_currentMap; }
std::shared_ptr<Player> GameSession::getPlayerPtr() const { return m_player; }

void GameSession::addNpc(std::shared_ptr<NonPlayableCharacter> npc) {
  m_npcs.push_back(npc);
  getMap(npc->getCurrentMap()).placeTop(npc, npc->getPosition());
}

void GameSession::addContainer(std::shared_ptr<Container> container) {
  m_sessionOwnedContainers.push_back(container);
  getMap(container->getCurrentMap())
      .placeTop(container, container->getPosition());
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

std::string GameSession::cleanDeadNpcs() {
  // this is the correct version that iterates on turnOrder only
  // to remove dead/fled npcs from there without iterating on all npcs
  std::ostringstream result;
  for (auto it{m_turnOrder.begin()}; it != m_turnOrder.end();) {
    auto lockedCharacter{(*it).lock()};
    if (!lockedCharacter) {
      it = m_turnOrder.erase(it);
    } else if (auto player =
                   std::dynamic_pointer_cast<Player>(lockedCharacter)) {
      ++it;
    } else if (auto npc = std::dynamic_pointer_cast<NonPlayableCharacter>(
                   lockedCharacter)) {
      if (npc->isDead()) {
        m_player->addXP(npc->getXpValue());
        result << npc->getName() << " is dead.\n";
        m_currentMap->removeTop(npc->getPosition());
        if (!(npc->getInventory().empty())) {
          auto lootableBody{std::make_shared<Container>(
              std::move(npc->getInventory()), npc->getPosition(),
              m_currentMap->getName(), npc->getName() + "'s body",
              npc->getDeadDescription())};
          addContainer(lootableBody);
          m_currentMap->placeTop(lootableBody, lootableBody->getPosition());
        }
        m_npcs.erase(
            std::remove_if(
                m_npcs.begin(), m_npcs.end(),
                [npc](const std::shared_ptr<NonPlayableCharacter> &ptr) {
                  return ptr == npc;
                }),
            m_npcs.end());
        it = m_turnOrder.erase(it);
      } else if (npc->getCurrentMap() != m_currentMap->getName()) {
        result << npc->getName() << " fled the area!\n";
        it = m_turnOrder.erase(it);
      } else
        ++it;
    }
  }
  return result.str();
}

bool GameSession::dropItem(std::shared_ptr<Item> item, const Point &point) {
  if (!m_currentMap->checkBounds(point)) {
    return false;
  }
  for (int i{0}; i < Directions::nbDirections; ++i) {
    Point adjacentPoint{
        point.getAdjacentPoint(static_cast<Directions::Direction>(i))};
    if (auto cont{std::dynamic_pointer_cast<Container>(
            m_currentMap->getTopObject(adjacentPoint))})
      if (cont->getName() == "Left items bag") {
        cont->addItem(item);
        return true;
      }
  }
  for (int i{0}; i < Directions::nbDirections; ++i) {
    Point adjacentPoint{
        point.getAdjacentPoint(static_cast<Directions::Direction>(i))};
    if (m_currentMap->isAvailable(adjacentPoint)) {
      std::vector<std::shared_ptr<Item>> vec{item};
      auto cont{std::make_shared<Container>(
          std::move(vec), adjacentPoint, m_currentMap->getName(),
          "Left items bag", "A bag of items left by someone")};
      addContainer(cont);
      m_currentMap->placeTop(cont, cont->getPosition());
      return true;
    }
  }
  return false;
}

std::vector<std::shared_ptr<Container>> &
GameSession::getSessionOwnedContainers() {
  return m_sessionOwnedContainers;
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
  m_player->setCombat();
  if (m_turnOrder.size() <= 1) { // player is alone in initiative
    // at the start of the game
    m_currentTurn = 1;
    m_turnOrder.clear();
    m_turnOrder.push_back(m_player);
    for (std::weak_ptr<Creature> npc : m_npcs) {
      if (!npc.expired()) {
        {
          if (npc.lock()->getCurrentMap() == m_currentMap->getName()) {
            npc.lock()->setCombat();
            m_turnOrder.push_back(npc);
          }
        }
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
  if (enemiesInMap())
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

std::vector<std::weak_ptr<NonPlayableCharacter>>
GameSession::getEnemiesInMap() const {
  std::vector<std::weak_ptr<NonPlayableCharacter>> enemies{};
  for (auto npc : m_npcs) {
    if (npc->getCurrentMap() == m_currentMap->getName())
      enemies.push_back(npc);
  }
  return enemies;
}

void GameSession::incrementTurnIndex() {
  if (m_turnOrder.empty()) {
    return;
  }
  if (m_currentTurnIndex >= m_turnOrder.size() - 1) {
    m_currentTurnIndex = 0; // loop back to first player
    incrementCurrentTurn();
  } else {
    m_currentTurnIndex++;
  }
}

std::weak_ptr<Creature> GameSession::getActiveCreature() const {
  return m_turnOrder[m_currentTurnIndex];
}

void GameSession::displayEnemiesInMap(
    std::function<int(const Creature &)> hitChanceFunc) const {
  auto enemies{getEnemiesInMap()};
  std::ostringstream res{};
  if (enemies.empty()) {
    std::cout << "No enemies in this map.\n";
    return;
  }
  res << "Enemies in this map:\n";
  for (std::size_t i{0}; i < enemies.size(); ++i) {
    if (auto lockedEnemy{enemies[i].lock()}) {
      res << i + 1 << ": " << lockedEnemy->getName() << "  "
          << lockedEnemy->getSymbol() << " at distance "
          << GeometryUtils::distanceL2(m_player->getPosition(),
                                       lockedEnemy->getPosition())
          << "  HP: " << lockedEnemy->getHealthPoints() << '/'
          << lockedEnemy->getMaxHealthPoints();
      if (!m_currentMap->isPointVisible(m_player->getPosition(),
                                        lockedEnemy->getPosition()))
        res << " (not visible)";
      if (hitChanceFunc) {
        int chanceToHit = hitChanceFunc(*lockedEnemy);
        res << "  " << chanceToHit << '%';
      }
      res << '\n';
    }
  }
  std::cout << res.str();
}

void GameSession::resetInitiative() {
  m_player->unsetCombat();
  m_turnOrder.clear();
  m_currentTurnIndex = 0;
  m_currentTurn = 1;
}

int GameSession::getCurrentTurn() const { return m_currentTurn; }
void GameSession::incrementCurrentTurn() { ++m_currentTurn; }

json GameSession::toJson() const {
  // can't save in combat, so turn order doesn't have to be saved
  // only player and world info need to be saved

  json j;
  j["player"] = m_player->toJson();
  j["currentMap"] = m_currentMap->getName();
  json allMaps;
  for (const auto &[name, map] : m_allMaps) {
    if (map.hasBeenVisited())
      allMaps[name] = map.toJson();
  }
  j["allMaps"] = allMaps;
  j["npcs"] = json::array();
  for (const auto &npc : m_npcs) {
    std::string npcCurrentMap{npc->getCurrentMap()};
    if (m_allMaps.at(npcCurrentMap).hasBeenVisited())
      j["npcs"].push_back(npc->toJson());
  }
  j["containers"] = json::array();
  for (const auto &container : m_sessionOwnedContainers) {
    j["containers"].push_back(container->toJson());
  }
  j["player"] = m_player->toJson();
  return j;
}

GameSession GameSession::loadFromJson(const json &jsonFile) {

  // First, initialize a new game
  auto player{std::make_shared<Player>(Point{2, 1}, "placeHolder", 10, "")};
  auto allItems{DataLoader::getAllItems()};
  auto allNpcs{DataLoader::getAllNpcs()};
  GameSession gameSession{player};
  DataLoader::populateGameSession(allItems, allNpcs, gameSession);

  // Load the saved data

  for (const auto &[mapName, mapJson] : jsonFile.at("allMaps").items()) {
    gameSession.getMap(mapName).updateFromJson(mapJson, allItems);
  }
  for (auto it{gameSession.m_npcs.begin()}; it != gameSession.m_npcs.end();) {
    std::string npcCurrentMap{(*it)->getCurrentMap()};
    if (gameSession.getMap(npcCurrentMap).hasBeenVisited()) {
      gameSession.getMap(npcCurrentMap).removeTop((*it)->getPosition());
      // This just removes a dereferenced weak ptr from the top layer of the
      // map
      it = gameSession.m_npcs.erase(it);
      // this deletes the npc from memory
    } else
      ++it;
  }
  for (auto it{gameSession.m_sessionOwnedContainers.begin()};
       it != gameSession.m_sessionOwnedContainers.end();) {
    std::string contCurrentMap{(*it)->getCurrentMap()};
    if (gameSession.getMap(contCurrentMap).hasBeenVisited()) {
      gameSession.getMap(contCurrentMap).removeTop((*it)->getPosition());
      // This just removes a dereferenced weak ptr from the top layer of the
      // map
      it = gameSession.m_sessionOwnedContainers.erase(it);
      // this deletes the container from memory
    } else
      ++it;
  }
  for (const auto &npcJson : jsonFile.at("npcs")) {
    std::string id{npcJson["id"]};
    std::string currentMap{npcJson["currentMap"]};
    if (!allNpcs.contains(id)) {
      std::cout << "Npc with id " << id << " not found in data, skipping.\n";
      continue;
    }
    auto npc{allNpcs.at(id)->clone()};
    npc->updateFromJson(npcJson, allItems, currentMap);
    gameSession.addNpc(npc);
  }
  for (const auto &containerJson : jsonFile.at("containers")) {
    std::string currentMap{containerJson["currentMap"]};
    gameSession.addContainer(std::make_shared<Container>(
        Container::loadFromJson(containerJson, allItems, currentMap)));
  }
  gameSession.getPlayer().updateFromJson(jsonFile.at("player"), allItems);
  gameSession.setCurrentMap(gameSession.getPlayer().getCurrentMap());
  gameSession.respawnPlayer();
  return gameSession;
}

bool GameSession::isGameActive() const {
  return !m_player->getName().empty() && !m_player->isDead();
}

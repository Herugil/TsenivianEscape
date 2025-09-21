#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "AISettings.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/items/Item.h"
#include "gameObjects/terrain/Container.h"
#include "gameObjects/terrain/MapChanger.h"
#include "input/Directions.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include "utils/Random.h"
#include <algorithm>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

NonPlayableCharacter::NonPlayableCharacter(
    char symbol, const Point &point, std::string_view currentMap,
    int maxHealthPoints, std::string_view name, int evasion, int meleeHitChance,
    int distanceHitChance, std::vector<std::shared_ptr<Item>> items,
    std::string_view description, std::string_view deadDescription,
    std::string_view aiType, int xpValue)
    : Creature{symbol,  point, currentMap, maxHealthPoints,
               evasion, name,  description},
      m_deadDescription{deadDescription}, m_meleeHitChance{meleeHitChance},
      m_distanceHitChance{distanceHitChance}, m_AIType{stringToAIType(aiType)},
      m_xpValue{xpValue} {
  if (!items.empty())
    m_inventory = std::move(items);
}

NonPlayableCharacter::NonPlayableCharacter(const NonPlayableCharacter &other)
    : Creature(other), m_deadDescription(other.m_deadDescription),
      m_meleeHitChance(other.m_meleeHitChance),
      m_distanceHitChance(other.m_distanceHitChance),
      m_meleeRange(other.m_meleeRange), m_meleeDamage(other.m_meleeDamage),
      m_distanceRange(other.m_distanceRange),
      m_distanceDamage(other.m_distanceDamage),
      m_currentPath(other.m_currentPath),
      m_currentBehavior(other.m_currentBehavior), m_AIType(other.m_AIType),
      m_xpValue{other.m_xpValue} {}

std::shared_ptr<NonPlayableCharacter> NonPlayableCharacter::clone() const {
  return std::make_shared<NonPlayableCharacter>(*this);
}

std::string_view NonPlayableCharacter::getDeadDescription() const {
  return m_deadDescription;
}

std::vector<std::shared_ptr<Item>> NonPlayableCharacter::getInventory() const {
  return m_inventory;
}

void NonPlayableCharacter::addItemToInventory(std::shared_ptr<Item> item) {
  m_inventory.push_back(std::move(item));
}

int NonPlayableCharacter::getMeleeHitChance() const { return m_meleeHitChance; }
int NonPlayableCharacter::getDistanceHitChance() const {
  return m_distanceHitChance;
}
int NonPlayableCharacter::getMeleeDamage() const { return m_meleeDamage; }
int NonPlayableCharacter::getMeleeRange() const { return m_meleeRange; }
int NonPlayableCharacter::getDistanceDamage() const { return m_distanceDamage; }
int NonPlayableCharacter::getDistanceRange() const { return m_distanceRange; }

std::string NonPlayableCharacter::executeBasicAttack(Creature &target,
                                                     GameSession &gameSession) {
  auto basicAction{m_actions[0]};
  return basicAction->execute(gameSession, *this, target);
}

std::string NonPlayableCharacter::setCurrentBehavior(
    [[maybe_unused]] GameSession &gameSession) {
  std::ostringstream res{};
  if (m_actionPoints == 0) {
    m_currentBehavior = skipTurn;
  } else {
    switch (m_AIType) {
    case aggressiveMelee:
      m_currentBehavior = basicAttack;
      break;
    case waryMelee:
      if (m_healthPoints <=
          m_maxHealthPoints * AISettings::g_waryMeleeFleeHealthPercent / 100) {
        if (Random::rollD100() < AISettings::g_waryMeleeFleeChance) {
          m_currentBehavior = flee;
          res << getName() << " starts fleeing!\n";
        } else
          m_currentBehavior = basicAttack;
      } else
        m_currentBehavior = basicAttack;
      break;
    default:
      m_currentBehavior = defaultBehavior;
    }
  }
  return res.str();
}

NonPlayableCharacter::Behaviors
NonPlayableCharacter::getCurrentBehavior() const {
  return m_currentBehavior;
}
void NonPlayableCharacter::setSkipTurn() { m_currentBehavior = skipTurn; }
void NonPlayableCharacter::setDefaultBehavior() {
  m_currentBehavior = defaultBehavior;
}
std::deque<Point> &NonPlayableCharacter::getCurrentPath() {
  return m_currentPath;
}

void NonPlayableCharacter::setCurrentPath(GameSession &gameSession) {
  std::vector<Point> possiblePoints{};
  switch (m_currentBehavior) {
  case basicAttack:
    m_currentPath = getPathAttack(gameSession);
    return;
  case flee:
    m_currentPath = getPathFlee(gameSession);
    return;
  default: // actor has no goal, so he doesnt really need to move..
    m_currentPath.clear(); // will remain in place
  }
}

std::deque<Point> NonPlayableCharacter::getPathFlee(GameSession &gameSession) {
  std::vector<Point> possiblePoints{};
  std::vector<Point> safePoints{};
  std::vector<Point> mapChangers{};
  std::deque<Point> path{};
  for (int i{0}; i < gameSession.getMap().getWidth(); ++i) {
    for (int j{0}; j < gameSession.getMap().getHeight(); ++j) {
      Point potentialDestination{i, j};
      if (!gameSession.getMap().isAvailable(potentialDestination))
        continue; // cant go there
      if (GeometryUtils::distanceL2(potentialDestination,
                                    gameSession.getPlayerPos()) <=
          GeometryUtils::distanceL2(getPosition(), gameSession.getPlayerPos()))
        continue; // would get closer to player
      if (gameSession.getMap().getFloorObject(potentialDestination)) {
        if (auto{dynamic_cast<MapChanger *>(
                gameSession.getMap().getFloorObject(potentialDestination))})
          mapChangers.push_back(potentialDestination);
        else
          continue;
      }
      safePoints.push_back(potentialDestination);
    }
  }
  std::sort(mapChangers.begin(), mapChangers.end(),
            [this](const Point &p1, const Point &p2) {
              return (GeometryUtils::distanceL2(getPosition(), p1) >=
                      GeometryUtils::distanceL2(getPosition(), p2));
            });
  path = GeometryUtils::sortPointsAndFindPath(mapChangers, getPosition(),
                                              gameSession);
  if (!path.empty())
    return path;
  else
    return GeometryUtils::sortPointsAndFindPath(safePoints, getPosition(),
                                                gameSession);
}

std::deque<Point>
NonPlayableCharacter::getPathAttack(GameSession &gameSession) {
  std::vector<Point> possiblePoints{};
  for (int i{0};
       static_cast<Directions::Direction>(i) < Directions::nbDirections; ++i) {
    Point potentialDestination{gameSession.getPlayerPos().getAdjacentPoint(
        static_cast<Directions::Direction>(i))};
    if (!gameSession.getMap().isAvailable(potentialDestination))
      continue; // cant go there
    possiblePoints.push_back(potentialDestination);
  }
  std::sort(possiblePoints.begin(), possiblePoints.end(),
            [this](const Point &p1, const Point &p2) {
              return GeometryUtils::distanceL1(getPosition(), p1) <=
                     GeometryUtils::distanceL1(getPosition(), p2);
            });
  return GeometryUtils::sortPointsAndFindPath(possiblePoints, getPosition(),
                                              gameSession);
  return {};
}

NonPlayableCharacter::AITypes
NonPlayableCharacter::stringToAIType(std::string_view str) {
  if (str == "aggressiveMelee")
    return aggressiveMelee;
  else if (str == "waryMelee")
    return waryMelee;
  else
    return defaultAI;
}

int NonPlayableCharacter::getXpValue() const { return m_xpValue; }
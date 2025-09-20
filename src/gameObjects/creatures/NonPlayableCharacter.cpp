#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/items/Item.h"
#include "gameObjects/terrain/Container.h"
#include "input/Directions.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include <algorithm>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

NonPlayableCharacter::NonPlayableCharacter(
    char symbol, const Point &point, std::string_view currentMap,
    int maxHealthPoints, std::string_view name, int m_meleeHitChance,
    int m_distanceHitChance, std::vector<std::shared_ptr<Item>> items,
    std::string_view description, std::string_view deadDescription)
    : Creature{symbol, point, currentMap, maxHealthPoints, name, description},
      m_deadDescription{deadDescription}, m_meleeHitChance{m_meleeHitChance},
      m_distanceHitChance{m_distanceHitChance} {
  if (!items.empty())
    m_inventory = std::move(items);
}

std::string_view NonPlayableCharacter::getDeadDescription() const {
  return m_deadDescription;
}

std::vector<std::shared_ptr<Item>> NonPlayableCharacter::getInventory() const {
  return m_inventory;
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
  // this should check for range, etc
  // for now will hit the player anywhere on the map
}

void NonPlayableCharacter::setCurrentBehavior(
    [[maybe_unused]] GameSession &gameSession) {
  if (m_actionPoints == 0) {
    m_currentBehavior = skipTurn;
    return;
  }
  m_currentBehavior = basicAttack;
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
    if (GeometryUtils::distanceL1(getPosition(), gameSession.getPlayerPos()) <=
        getMeleeRange())
      m_currentPath.clear();
    // enemy is already in range, no movement needed
    for (int i{0};
         static_cast<Directions::Direction>(i) < Directions::nbDirections;
         ++i) {
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
    for (auto possiblePoint : possiblePoints) {
      std::deque<Point> res{
          gameSession.getMap().findPath(getPosition(), possiblePoint)};
      if (!res.empty()) {
        res.pop_front(); // this is the current point! unneeded
        m_currentPath = res;
        return;
      }
    }

  default: // actor has no goal, so he doesnt really need to move..
    m_currentPath.clear(); // will remain in place
  }
}

#include "gameObjects/creatures/Creature.h"
#include "core/GameSession.h"
#include "gameObjects/GameObject.h"
#include "map/Point.h"
#include "scripts/MeleeAttack.h"

Creature::Creature(char symbol, const Point &position,
                   std::string_view currentMap, int maxHealthPoints,
                   std::string_view name, std::string_view description)
    : GameObject{true, false, symbol, currentMap, position, name, description},
      m_maxHealthPoints{maxHealthPoints} {
  m_actions.emplace_back(
      std::make_shared<MeleeAttack>("Attack with right hand weapon"));
  m_healthPoints = m_maxHealthPoints;
}

int Creature::getHealthPoints() const { return m_healthPoints; }
int Creature::getMaxHealthPoints() const { return m_maxHealthPoints; }
bool Creature::isDead() const { return m_healthPoints <= 0; }

void Creature::takeDamage(int damage) { m_healthPoints -= damage; }

const std::string &Creature::getName() const { return m_name; }
int Creature::getMovementPoints() const { return m_movementPoints; }
int Creature::getActionPoints() const { return m_actionPoints; }
int Creature::getMaxMovementPoints() const { return m_maxMovementPoints; }
int Creature::getMaxActionPoints() const { return m_maxActionPoints; }
bool Creature::useActionPoints(int cost) {
  bool returnVal{canAct(cost)};
  if (returnVal)
    m_actionPoints -= cost;
  return returnVal;
}

void Creature::resetTurn() {
  refillActionPoints();
  m_movementPoints = 0;
}

bool Creature::useMovementPoints(int cost) {
  bool returnVal{canMove(cost)};
  if (canMove(cost)) {
    if (m_movementPoints - cost >= 0)
      m_movementPoints -= cost;
    else {
      m_actionPoints -= 1;
      m_movementPoints = getMaxMovementPoints() - cost;
      returnVal = true;
    }
  }
  return returnVal;
}

bool Creature::canAct(int cost) const {
  return (!m_inCombat || m_actionPoints - cost >= 0);
}
bool Creature::canMove(int cost) const {
  return (!m_inCombat || m_movementPoints - cost >= 0 || m_actionPoints > 0);
}

void Creature::refillActionPoints() { m_actionPoints = m_maxActionPoints; }
void Creature::refillMovementPoints() {
  m_movementPoints = m_maxMovementPoints;
}

bool Creature::inCombat() const { return m_inCombat; }
void Creature::setCombat() { m_inCombat = true; }
void Creature::unsetCombat() { m_inCombat = false; }
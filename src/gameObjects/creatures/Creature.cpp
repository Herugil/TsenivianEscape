#include "gameObjects/creatures/Creature.h"
#include "core/GameSession.h"
#include "gameObjects/GameObject.h"
#include "map/Point.h"

Creature::Creature(char symbol, const Point &position, int healthPoints,
                   std::string_view name, std::string_view description)
    : GameObject{true, false, symbol, position, description},
      m_healthPoints{healthPoints}, m_name{name} {}

int Creature::getHealthPoints() const { return m_healthPoints; }
bool Creature::isDead() const { return m_healthPoints <= 0; }

void Creature::takeDamage(int damage) { m_healthPoints -= damage; }

std::string_view Creature::getName() const { return m_name; }
int Creature::getMovementPoints() const { return m_movementPoints; }
int Creature::getActionPoints() const { return m_actionPoints; }
int Creature::getMaxMovementPoints() const { return m_maxMovementPoints; }
int Creature::getMaxActionPoints() const { return m_maxActionPoints; }
bool Creature::useActionPoints(int cost) {
  bool canAct{m_actionPoints - cost >= 0};
  if (canAct)
    m_actionPoints -= cost;
  return canAct;
}
bool Creature::useMovementPoints(int cost) {
  bool canMove{m_movementPoints - cost >= 0};
  if (canMove)
    m_movementPoints -= cost;
  return canMove;
}
bool Creature::canAct(int cost) {
  if (!m_inCombat)
    return true;
  return useActionPoints(cost);
}
bool Creature::canMove(int cost) {
  if (!m_inCombat)
    return true;
  return useMovementPoints(cost);
}

void Creature::refillActionPoints() { m_actionPoints = m_maxActionPoints; }
void Creature::refillMovementPoints() {
  m_movementPoints = m_maxMovementPoints;
}

bool Creature::inCombat() const { return m_inCombat; }
void Creature::setCombat() { m_inCombat = true; }
void Creature::unsetCombat() { m_inCombat = false; }
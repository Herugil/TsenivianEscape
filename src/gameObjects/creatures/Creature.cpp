#include "gameObjects/creatures/Creature.h"
#include "core/GameSession.h"
#include "gameObjects/GameObject.h"
#include "map/Point.h"
#include "scripts/actions/BasicAttack.h"

Creature::Creature(char symbol, const Point &position,
                   std::string_view currentMap, int maxHealthPoints,
                   int evasion, std::string_view name,
                   std::string_view description)
    : GameObject{true, false, symbol, currentMap, position, name, description},
      m_maxHealthPoints{maxHealthPoints}, m_evasion{evasion} {
  m_actions.emplace_back(std::make_unique<BasicAttack>(
      "Attack with right hand weapon", Stat::Strength));
  m_healthPoints = m_maxHealthPoints;
}

Creature::Creature(const Creature &other)
    : GameObject(other), m_inventory{}, m_healthPoints{other.m_healthPoints},
      m_maxHealthPoints{other.m_maxHealthPoints}, m_evasion{other.m_evasion},
      m_maxMovementPoints{other.m_maxMovementPoints},
      m_maxActionPoints{other.m_maxActionPoints},
      m_movementPoints{other.m_movementPoints},
      m_actionPoints{other.m_actionPoints}, m_inCombat{other.m_inCombat} {
  for (const auto &effect : other.m_passiveEffects) {
    m_passiveEffects.emplace_back(std::make_unique<PassiveEffect>(*effect));
  } // hey this is slicing
  for (const auto &item : other.m_inventory) {
    m_inventory.emplace_back(item->clone());
  }
  for (const auto &action : other.m_actions) {
    m_actions.emplace_back(action->clone());
  }
}

int Creature::getHealthPoints() const { return m_healthPoints; }
int Creature::getMaxHealthPoints() const { return m_maxHealthPoints; }
int Creature::getStrength() const { return 0; }
int Creature::getDexterity() const { return 0; }
int Creature::getIntelligence() const { return 0; }
int Creature::getConstitution() const { return 0; }
int Creature::getArmor() const { return 0; }
bool Creature::isDead() const { return m_healthPoints <= 0; }
int Creature::getEvasion() const {
  return m_evasion + getStatModifier(Stat::Evasion);
}
int Creature::takeDamage(int damage, bool ignoreArmor) {
  if (!ignoreArmor)
    damage = std::max(0, damage - getArmor());
  m_healthPoints -= damage;
  return damage;
}
void Creature::addHealthPoints(int healthPoints) {
  m_healthPoints = std::min(m_healthPoints + healthPoints, m_maxHealthPoints);
}

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

void Creature::reduceCooldowns() {
  for (auto it{m_passiveEffects.begin()}; it < m_passiveEffects.end(); ++it) {
    (*it)->applyEffect(*this);
    (*it)->decrementRounds();
  }
  m_passiveEffects.erase(
      std::remove_if(m_passiveEffects.begin(), m_passiveEffects.end(),
                     [](const std::unique_ptr<PassiveEffect> &ptr) {
                       return ptr->isExpired();
                     }),
      m_passiveEffects.end());
  for (auto &action : m_actions) {
    action->reduceCooldown();
  }
}

void Creature::removePassives() {
  for (auto it{m_passiveEffects.begin()}; it < m_passiveEffects.end();) {
    if ((*it)->expiresOnRest())
      it = m_passiveEffects.erase(it);
    else
      ++it;
  }
}

void Creature::refillSkillCharges() {
  for (auto &action : m_actions) {
    action->refillCharges();
  }
}

void Creature::addPassiveEffect(const PassiveEffect &passive) {
  if (!passive.isStackable()) {
    for (auto &existing : m_passiveEffects) {
      if (existing->getType() == passive.getType() &&
          existing->getId() == passive.getId()) {
        // refresh duration
        existing->setRoundsLeft(passive.getRoundsLeft());
        return;
      }
    }
  }
  m_passiveEffects.emplace_back(passive.clone());
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
void Creature::addActionPoints(int points) {
  m_actionPoints += points; // can go over max, intentional
}
void Creature::addMovementPoints(int points) {
  m_movementPoints += points; // can go over max, intentional
}

bool Creature::inCombat() const { return m_inCombat; }
void Creature::setCombat() { m_inCombat = true; }
void Creature::unsetCombat() { m_inCombat = false; }
void Creature::resetOutOfCombat() {
  for (auto it{m_passiveEffects.begin()}; it < m_passiveEffects.end();) {
    if ((*it)->getRoundsLeft() != -1)
      it = m_passiveEffects.erase(it);
    else
      ++it;
  }
  for (auto it{m_actions.begin()}; it < m_actions.end(); ++it) {
    (*it)->resetCooldown();
  }
}
int Creature::getStatModifier(Stat stat) const {
  int totalModifier{0};
  for (const auto &effect : m_passiveEffects) {
    totalModifier += effect->getStatModifier(stat);
  }
  return totalModifier;
}
#include "gameObjects/creatures/Creature.h"
#include "core/GameSession.h"
#include "gameObjects/GameObject.h"
#include "map/Point.h"

Creature::Creature(char symbol, const Point &position, int healthPoints)
    : GameObject{true, false, symbol, position}, m_healthPoints{healthPoints} {}

int Creature::getHealthPoints() const { return m_healthPoints; }
bool Creature::isDead() const { return m_healthPoints <= 0; }

void Creature::takeDamage(int damage) { m_healthPoints -= damage; }

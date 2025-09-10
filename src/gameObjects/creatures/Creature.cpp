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
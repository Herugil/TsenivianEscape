#pragma once
#include "gameObjects/GameObject.h"
#include "gameObjects/Item.h"
#include <vector>

// will have a lot of stats down the road
class Creature : public GameObject {
protected:
  std::vector<std::shared_ptr<Item>> m_inventory{};
  // player needs an inventory, monsters should be lootable once they die
  // or player could pickpocket them
  int m_healthPoints{};
  bool m_isDead{};

public:
  Creature(char symbol, const Point &position, int healthPoints);
  int getHealthPoints() const;
  bool isDead() const;
  void takeDamage(int damage);
};

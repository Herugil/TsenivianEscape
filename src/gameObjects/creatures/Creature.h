#pragma once
#include "gameObjects/GameObject.h"
#include "gameObjects/items/Item.h"
#include <vector>

// will have a lot of stats down the road
class Creature : public GameObject {
protected:
  std::vector<std::shared_ptr<Item>> m_inventory{};
  // player needs an inventory, monsters should be lootable once they die
  // or player could pickpocket them
  std::string m_name{};
  int m_healthPoints{};
  bool m_isDead{};

public:
  Creature(char symbol, const Point &position, int healthPoints,
           std::string_view name = "", std::string_view description = "");
  int getHealthPoints() const;
  bool isDead() const;
  void takeDamage(int damage);
  std::string_view getName() const;
  virtual int getMeleeDamage() const = 0;
  virtual ~Creature() = default;
};

#pragma once
#include "Settings.h"
#include "gameObjects/GameObject.h"
#include "gameObjects/items/Item.h"
#include "scripts/Action.h"
#include <vector>

// will have a lot of stats down the road
class Creature : public GameObject {
protected:
  std::vector<std::shared_ptr<Item>> m_inventory{};
  // player needs an inventory, monsters should be lootable once they die
  // or player could pickpocket them
  std::vector<std::shared_ptr<Action>> m_actions{};
  std::string m_name{};
  int m_healthPoints{};
  int m_maxHealthPoints{};
  int m_maxMovementPoints{Settings::g_averageMoveSpeed};
  int m_maxActionPoints{Settings::g_numActions};
  int m_movementPoints{0};
  int m_actionPoints{Settings::g_numActions};
  bool m_inCombat{false};

public:
  Creature(char symbol, const Point &position, int maxHealthPoints,
           std::string_view name = "", std::string_view description = "");
  int getHealthPoints() const;
  bool isDead() const;
  void takeDamage(int damage);
  std::string_view getName() const;
  virtual int getMeleeDamage() const = 0;
  int getMovementPoints() const;
  int getActionPoints() const;
  int getMaxMovementPoints() const;
  int getMaxActionPoints() const;
  int getMaxHealthPoints() const;
  bool useActionPoints(int cost = 1);
  bool useMovementPoints(int cost = 1);
  bool canAct(int cost = 1);
  bool canMove(int cost = 1);
  void refillActionPoints();
  void refillMovementPoints();
  virtual ~Creature() = default;
  bool inCombat() const;
  void setCombat();
  void unsetCombat();
};

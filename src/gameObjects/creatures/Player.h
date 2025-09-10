#pragma once
#include "gameObjects/Item.h"
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/terrain/Container.h"
#include "input/Directions.h"
#include "input/Input.h"
#include "map/Point.h"
#include <memory>
#include <vector>

class GameSession;

struct Equipment {
  std::weak_ptr<Item> rightHand{};
  // will see the rest later..
};

class Player : public Creature {
private:
  Equipment m_equipment{};

public:
  Player(const Point &position, int healthPoints);
  void takeItem(std::shared_ptr<Item> item);
  void inventoryMenu();
  void displayInventory() const;
  void takeAllItems(Container &container);
  void getRightHandItem() const {};
  void equipItem(std::shared_ptr<Item> item);
  std::shared_ptr<Item> getItem(std::size_t index) const;
};

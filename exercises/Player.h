#pragma once
#include "Container.h"
#include "Creature.h"
#include "Directions.h"
#include "Item.h"
#include "Point.h"
#include <memory>
#include <vector>

struct Equipment {
  std::weak_ptr<Item> rightHand{};
  // will see the rest later..
};

class Player : public Creature {
private:
  Equipment m_equipment{};

public:
  Player(const Point &position);
  void takeItem(std::shared_ptr<Item> item);
  void inventoryMenu();
  void displayInventory() const;
  void takeAllItems(Container &container);
  void getRightHandItem() const {};
  void equipItem(std::shared_ptr<Item> item);
  std::shared_ptr<Item> getItem(std::size_t index) const;
};

#pragma once
#include "Container.h"
#include "Creature.h"
#include "Directions.h"
#include "Item.h"
#include "Point.h"
#include <memory>
#include <vector>

class Player : public Creature {
public:
  Player(const Point &position);
  void takeItem(std::shared_ptr<Item> item);
  void displayInventory() const;
  void takeAllItems(Container &container);
};

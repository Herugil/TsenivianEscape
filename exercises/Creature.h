#pragma once
#include "GameObject.h"
#include "Item.h"
#include <vector>

// will have a lot of stats down the road
class Creature : public GameObject {
protected:
  std::vector<std::shared_ptr<Item>> m_inventory{};
  // player needs an inventory, monsters should be lootable once they die
  // or player could pickpocket them

public:
  Creature(char symbol, const Point &position);
};

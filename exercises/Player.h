#pragma once
#include "Creature.h"
#include "Directions.h"
#include "Item.h"
#include "Map.h"
#include "Point.h"
#include <memory>
#include <vector>

class Player : public Creature {
private:
  std::vector<std::shared_ptr<Item>> m_inventory{};

public:
  Player(const Point &position);
};

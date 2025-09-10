#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/Item.h"
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/terrain/Container.h"
#include "map/Point.h"
#include <vector>

NonPlayableCharacter::NonPlayableCharacter(char symbol, const Point &point,
                                           int healthPoints,
                                           std::string_view deadDescription)
    : Creature{symbol, point, healthPoints},
      m_deadDescription{deadDescription} {}

NonPlayableCharacter::NonPlayableCharacter(
    char symbol, const Point &point, int healthPoints,
    std::vector<std::shared_ptr<Item>> items, std::string_view deadDescription)
    : NonPlayableCharacter{symbol, point, healthPoints, deadDescription} {
  m_inventory = std::move(items);
}

std::string_view NonPlayableCharacter::getDeadDescription() const {
  return m_deadDescription;
}

std::vector<std::shared_ptr<Item>> NonPlayableCharacter::getInventory() const {
  return m_inventory;
}
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/items/Item.h"
#include "gameObjects/terrain/Container.h"
#include "map/Point.h"
#include <vector>

NonPlayableCharacter::NonPlayableCharacter(char symbol, const Point &point,
                                           int healthPoints,
                                           std::string_view name,
                                           std::string_view description,
                                           std::string_view deadDescription)
    : Creature{symbol, point, healthPoints, name, description},
      m_deadDescription{deadDescription} {}

NonPlayableCharacter::NonPlayableCharacter(
    char symbol, const Point &point, int healthPoints,
    std::vector<std::shared_ptr<Item>> items, std::string_view name,
    std::string_view description, std::string_view deadDescription)
    : NonPlayableCharacter{symbol, point,       healthPoints,
                           name,   description, deadDescription} {
  m_inventory = std::move(items);
}

std::string_view NonPlayableCharacter::getDeadDescription() const {
  return m_deadDescription;
}

std::vector<std::shared_ptr<Item>> NonPlayableCharacter::getInventory() const {
  return m_inventory;
}

int NonPlayableCharacter::getMeleeDamage() const { return m_meleeDamage; }
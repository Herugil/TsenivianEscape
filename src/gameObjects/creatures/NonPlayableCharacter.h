#pragma once
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/items/Item.h"
#include "map/Point.h"
#include <string>
#include <string_view>
#include <vector>

class GameSession;

class NonPlayableCharacter : public Creature {
protected:
  std::string m_deadDescription{};
  int m_meleeDamage{};

public:
  NonPlayableCharacter(char symbol, const Point &point, int healthPoints,
                       std::string_view name = "",
                       std::string_view description = "enemy",
                       std::string_view deadDescription = "");
  NonPlayableCharacter(char symbol, const Point &point, int healthPoints,
                       std::vector<std::shared_ptr<Item>>,
                       std::string_view name = "",
                       std::string_view description = "enemy",
                       std::string_view deadDescription = "");
  std::string_view getDeadDescription() const;
  std::vector<std::shared_ptr<Item>> getInventory() const;
  int getMeleeDamage() const override;
  virtual ~NonPlayableCharacter() = default;
};
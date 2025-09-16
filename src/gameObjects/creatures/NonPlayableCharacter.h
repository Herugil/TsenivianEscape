#pragma once
#include "Settings.h"
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
  int m_meleeRange{Settings::g_baseEnemyRange};
  int m_meleeDamage{Settings::g_baseEnemyDamage};
  int m_distanceRange{Settings::g_baseEnemyDistanceRange};
  int m_distanceDamage{Settings::g_baseEnemyDistanceDamage};

public:
  NonPlayableCharacter(char symbol, const Point &point,
                       std::string_view currentMap, int maxHealthPoints,
                       std::string_view name,
                       std::string_view description = "enemy",
                       std::string_view deadDescription = "");
  NonPlayableCharacter(char symbol, const Point &point,
                       std::string_view currentMap, int maxHealthPoints,
                       std::vector<std::shared_ptr<Item>>,
                       std::string_view name,
                       std::string_view description = "enemy",
                       std::string_view deadDescription = "");
  std::string_view getDeadDescription() const;
  std::vector<std::shared_ptr<Item>> getInventory() const;
  void executeBasicAttack(Creature &target, GameSession &gameSession);
  int getMeleeDamage() const override;
  int getMeleeRange() const override;
  int getDistanceDamage() const override;
  int getDistanceRange() const override;
  virtual ~NonPlayableCharacter() = default;
};
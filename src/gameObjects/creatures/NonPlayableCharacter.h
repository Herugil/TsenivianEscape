#pragma once
#include "Settings.h"
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/items/Item.h"
#include "map/Point.h"
#include <queue>
#include <string>
#include <string_view>
#include <vector>

class GameSession;

class NonPlayableCharacter : public Creature {
public:
  enum Behaviors {
    basicAttack,
    skipTurn,
    defaultBehavior,
  };

protected:
  std::string m_deadDescription{};
  int m_meleeRange{Settings::g_baseEnemyRange};
  int m_meleeDamage{Settings::g_baseEnemyDamage};
  int m_distanceRange{Settings::g_baseEnemyDistanceRange};
  int m_distanceDamage{Settings::g_baseEnemyDistanceDamage};
  std::deque<Point> m_currentPath{};
  Behaviors m_currentBehavior{defaultBehavior};

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
  std::string executeBasicAttack(Creature &target, GameSession &gameSession);
  int getMeleeDamage() const override;
  int getMeleeRange() const override;
  int getDistanceDamage() const override;
  int getDistanceRange() const override;
  Behaviors getCurrentBehavior() const;
  void setCurrentBehavior(GameSession &gameSession);
  void setSkipTurn();
  void setDefaultBehavior();
  std::deque<Point> &getCurrentPath();
  void setCurrentPath(GameSession &gameSession);
  void clearCurrentPath() { m_currentPath.clear(); }

  virtual ~NonPlayableCharacter() = default;
};
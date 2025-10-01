#pragma once
#include "Settings.h"
#include "gameObjects/GameObject.h"
#include "gameObjects/creatures/Stats.h"
#include "gameObjects/items/Item.h"
#include "scripts/actions/Action.h"
#include "scripts/passives/PassiveEffect.h"
#include <vector>

// will have a lot of stats down the road
class Creature : public GameObject {
protected:
  std::vector<std::shared_ptr<Item>> m_inventory{};
  // player needs an inventory, monsters should be lootable once they die
  // or player could pickpocket them
  std::vector<std::unique_ptr<Action>> m_actions{};
  // this should probably be unique ptr, need to change, mb
  int m_healthPoints{};
  int m_maxHealthPoints{};
  int m_evasion{};
  Stats m_stats{};
  int m_maxMovementPoints{Settings::g_averageMoveSpeed};
  int m_maxActionPoints{Settings::g_numActions};
  std::vector<std::unique_ptr<PassiveEffect>> m_passiveEffects{};
  int m_movementPoints{0};
  int m_actionPoints{Settings::g_numActions};
  bool m_inCombat{false};
  bool m_buffedThisTurn{false};

public:
  Creature(char symbol, const Point &position, std::string_view currentMap,
           int maxHealthPoints, int evasion = 0, Stats stats = Stats{},
           std::string_view name = "", std::string_view description = "");
  Creature(const Creature &other);
  int getHealthPoints() const;
  bool isDead() const;
  int takeDamage(int damage, bool ignoreArmor = false);
  void addHealthPoints(int healthPoints);
  const std::string &getName() const;
  virtual int getEvasion() const;
  virtual int getMeleeHitChance() const = 0;
  virtual int getDistanceHitChance() const = 0;
  virtual int getMeleeDamage() const = 0;
  virtual int getMeleeRange() const = 0;
  virtual int getDistanceDamage() const = 0;
  virtual int getDistanceRange() const = 0;
  virtual int getStrength() const;
  virtual int getDexterity() const;
  virtual int getIntelligence() const;
  virtual int getConstitution() const;
  virtual int getArmor() const;
  int getMovementPoints() const;
  int getActionPoints() const;
  int getMaxMovementPoints() const;
  int getMaxActionPoints() const;
  void resetTurn();
  int getMaxHealthPoints() const;
  bool useActionPoints(int cost = 1);
  bool useMovementPoints(int cost = 1);
  bool canAct(int cost = 1) const;
  bool canMove(int cost = 1) const;
  void addPassiveEffect(const PassiveEffect &passive);
  void reduceCooldowns();
  void removePassives();
  void refillSkillCharges();
  void refillActionPoints();
  void refillMovementPoints();
  void addActionPoints(int points);
  void addMovementPoints(int points);
  virtual ~Creature() = default;
  bool inCombat() const;
  void setCombat();
  void resetOutOfCombat();
  void unsetCombat();
  const std::vector<std::shared_ptr<Item>> &getInventory() const;
  std::vector<std::shared_ptr<Item>> &getInventory();
  int getStatModifier(Stat stat) const;
  int countActiveBuffs() const;
  int countActiveDebuffs() const;
  bool hasBuffedThisTurn() const { return m_buffedThisTurn; }
  void setBuffedThisTurn() { m_buffedThisTurn = true; }
  void resetBuffedThisTurn() { m_buffedThisTurn = false; }
};

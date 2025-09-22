#pragma once
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/items/Item.h"
#include "gameObjects/items/UsableItem.h"
#include "gameObjects/terrain/Container.h"
#include "input/Directions.h"
#include "input/Input.h"
#include "map/Point.h"
#include "scripts/actions/Action.h"
#include "scripts/actions/MeleeAttack.h"
#include "scripts/actions/Shove.h"
#include <memory>
#include <sstream>
#include <vector>

class GameSession;

class Player : public Creature {
public:
  struct Equipment {
    std::weak_ptr<Item> rightHand{};
    std::weak_ptr<Item> leftHand{};
    // will see the rest later..
  };
  struct Stats {
    int strength{};
    int dexterity{};
    int intelligence{};
    int constitution{};
  };

private:
  Stats m_stats{};
  Equipment m_equipment{};
  Shove m_shoveAction{};
  std::shared_ptr<MeleeAttack> m_meleeAttack{
      std::make_shared<MeleeAttack>("Attack with melee weapon")};
  int m_currentXP{0};
  int m_level{1};
  int m_xpToNextLevel{100}; // const, linear, exp ?

public:
  Player(const Point &position, std::string_view currentMap,
         int maxHealthPoints, Stats stats = Stats{1, 1, 1, 1});
  void takeItem(std::shared_ptr<Item> item);
  void displayInventory(std::size_t page = 0) const;
  void displayActions() const;
  std::shared_ptr<Action> getAction(std::size_t index) const;
  void takeAllItems(Container &container);
  void equipItem(std::shared_ptr<Item> item);
  void removeItem(std::shared_ptr<Item> item);
  void updateActionsOnEquip();
  std::shared_ptr<Item> getItem(std::size_t index) const;
  std::string shove(GameSession &gameSession, Directions::Direction direction);
  std::string meleeAttack(GameSession &gameSession,
                          Directions::Direction direction);
  std::string useItem(std::shared_ptr<UsableItem> item);
  int getStrength() const;
  int getDexterity() const;
  int getIntelligence() const;
  int getConstitution() const;
  int getEvasion() const override;
  int getMeleeHitChance() const override;
  int getDistanceHitChance() const override;
  int getMeleeDamage() const override;
  int getMeleeRange() const override;
  int getDistanceDamage() const override;
  int getDistanceRange() const override;
  int numObjectsHeld() const;
  int getCurrentXP() const;
  int getLevel() const;
  int getXpToNextLevel() const;
  void addXP(int xp);
  void displayCharacterSheet() const;
  bool canLevelUp() const;
  void levelUp();
  virtual ~Player() = default;
};

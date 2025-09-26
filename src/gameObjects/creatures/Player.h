#pragma once
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/creatures/Stats.h"
#include "gameObjects/items/Equipment.h"
#include "gameObjects/items/Item.h"
#include "gameObjects/items/UsableItem.h"
#include "gameObjects/terrain/Container.h"
#include "input/Directions.h"
#include "input/Input.h"
#include "map/Point.h"
#include "scripts/actions/Action.h"
#include "scripts/actions/BasicAttack.h"
#include "scripts/actions/Shove.h"
#include <memory>
#include <sstream>
#include <vector>

class GameSession;

class Player : public Creature {
public:
  struct PlayerEquipment {
    std::weak_ptr<Equipment> rightHand{};
    std::weak_ptr<Equipment> leftHand{};
    std::weak_ptr<Equipment> chestArmor{};
    std::weak_ptr<Equipment> legArmor{};
    std::weak_ptr<Equipment> helmet{};
    std::weak_ptr<Equipment> boots{};
    std::weak_ptr<Equipment> gloves{};
    // will see the rest later..
  };

private:
  Stats m_stats{};
  PlayerEquipment m_equipment{};
  Shove m_shoveAction{};
  int m_currentXP{100};
  int m_level{1};
  int m_xpToNextLevel{100}; // const, linear exp ?

public:
  Player(const Point &position, std::string_view currentMap,
         int maxHealthPoints, Stats stats = Stats{1, 1, 1, 1});
  void takeItem(std::shared_ptr<Item> item);
  void displayInventory(std::size_t page = 0) const;
  void displayActions() const;
  Action *getAction(std::size_t index);
  const std::vector<std::unique_ptr<Action>> &getAllActions();
  void takeAllItems(Container &container);
  void equipItem(std::shared_ptr<Equipment> item);
  void handleEquipSlot(std::weak_ptr<Equipment> &currentItem,
                       std::shared_ptr<Equipment> newItem);
  bool checkRequirements(std::shared_ptr<Item> item) const;
  void removeItem(std::shared_ptr<Item> item);
  void updateActionsOnEquip();
  std::shared_ptr<Item> getItem(std::size_t index) const;
  std::string shove(GameSession &gameSession, Directions::Direction direction);
  std::string useItem(std::shared_ptr<UsableItem> item);
  int getStrength() const override;
  int getDexterity() const override;
  int getIntelligence() const override;
  int getConstitution() const override;
  int getArmor() const override;
  Stats getStats() const;
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
  void levelUp(Stat stat);
  void addAction(std::unique_ptr<Action> action);
  virtual ~Player() = default;
};

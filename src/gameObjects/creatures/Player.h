#pragma once
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/items/Item.h"
#include "gameObjects/terrain/Container.h"
#include "input/Directions.h"
#include "input/Input.h"
#include "map/Point.h"
#include "scripts/Action.h"
#include "scripts/Shove.h"
#include <memory>
#include <vector>

class GameSession;

struct Equipment {
  std::weak_ptr<Item> rightHand{};
  std::weak_ptr<Item> leftHand{};
  // will see the rest later..
};

class Player : public Creature {
private:
  Equipment m_equipment{};
  Shove m_shoveAction{};

public:
  Player(const Point &position, std::string_view currentMap,
         int maxHealthPoints);
  void takeItem(std::shared_ptr<Item> item);
  void inventoryMenu();
  void actionMenu(GameSession &gameSession);
  void displayInventory(std::size_t page = 0) const;
  void displayActions() const;
  std::shared_ptr<Action> getAction(std::size_t index) const;
  void takeAllItems(Container &container);
  void equipItem(std::shared_ptr<Item> item);
  void updateActionsOnEquip();
  std::shared_ptr<Item> getItem(std::size_t index) const;
  void shove(GameSession &gameSession, Directions::Direction direction);
  int getMeleeDamage() const override;
  int getMeleeRange() const override;
  int getDistanceDamage() const override;
  int getDistanceRange() const override;
  int numObjectsHeld() const;
  virtual ~Player() = default;
};

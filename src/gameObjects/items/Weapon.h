#pragma once
#include "gameObjects/items/Item.h"

class Weapon : public Item {
protected:
  int m_damage{};
  int m_range{};

public:
  Weapon(std::string_view name, ItemType itemType = ItemType::other,
         int damage = 1, int range = 1)
      : Item{name, itemType}, m_damage{damage}, m_range{range} {}
  int getDamage() const override { return m_damage; }
  int getRange() const override { return m_damage; }
  std::shared_ptr<Item> clone() const override {
    return std::make_shared<Weapon>(*this);
  };
  virtual ~Weapon() = default;
};
#pragma once
#include "gameObjects/items/Item.h"

class Weapon : public Item {
public:
  enum WeaponType {
    melee,
    ranged,
    nbTypes,
  };

protected:
  int m_damage{};
  int m_range{};
  WeaponType m_weaponType{};

public:
  Weapon(std::string_view name, std::string_view weaponType = "melee",
         ItemType itemType = ItemType::other, int damage = 1, int range = 1)
      : Item{name, itemType}, m_damage{damage}, m_range{range},
        m_weaponType{getTypeFromStr(weaponType)} {}
  int getDamage() const override { return m_damage; }
  int getRange() const override { return m_range; }
  std::shared_ptr<Item> clone() const override {
    return std::make_shared<Weapon>(*this);
  };
  WeaponType getWeaponType() const { return m_weaponType; }
  static WeaponType getTypeFromStr(std::string_view typeName) {
    if (typeName == "melee")
      return melee;
    if (typeName == "ranged")
      return ranged;
    return nbTypes;
  };
  virtual ~Weapon() = default;
};
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
  Weapon(std::string_view name, std::string_view id,
         std::string_view weaponType = "melee",
         std::string_view description = "", ItemType itemType = ItemType::other,
         int damage = 1, int range = 1);
  int getDamage() const;
  int getRange() const;
  std::shared_ptr<Item> clone() const override;
  WeaponType getWeaponType() const;
  std::string getDisplayItem() const override;
  static WeaponType getTypeFromStr(std::string_view typeName);
  virtual ~Weapon() = default;
};
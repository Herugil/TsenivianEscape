#include "gameObjects/items/Weapon.h"
#include <sstream>

Weapon::Weapon(std::string_view name, std::string_view id,
               std::string_view weaponType, std::string_view description,
               ItemType itemType, int damage, int range)
    : Item{name, id, itemType, description}, m_damage{damage}, m_range{range},
      m_weaponType{getTypeFromStr(weaponType)} {}

int Weapon::getDamage() const { return m_damage; }
int Weapon::getRange() const { return m_range; }
std::shared_ptr<Item> Weapon::clone() const {
  return std::make_shared<Weapon>(*this);
};

Weapon::WeaponType Weapon::getWeaponType() const { return m_weaponType; }
std::string Weapon::getDisplayItem() const {
  std::ostringstream res;
  res << m_name << '\n'
      << m_description << (isEquipped() ? "\nEquipped" : "")
      << "\nDamage: " << m_damage << "\nRange: " << m_range;
  return res.str();
}
static Weapon::WeaponType getTypeFromStr(std::string_view typeName) {
  if (typeName == "melee")
    return Weapon::melee;
  if (typeName == "ranged")
    return Weapon::ranged;
  return Weapon::nbTypes;
};

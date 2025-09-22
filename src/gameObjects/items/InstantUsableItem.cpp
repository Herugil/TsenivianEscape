#include "gameObjects/items/InstantUsableItem.h"
#include "gameObjects/creatures/Creature.h"

InstantUsableItem::InstantUsableItem(std::string_view name, std::string_view id,
                                     Type regenType, int effectValue,
                                     int costToUse, bool isUnlimitedUse,
                                     int usesLeft, std::string_view description)
    : UsableItem{name, id, costToUse, isUnlimitedUse, usesLeft, description},
      m_effectValue{effectValue}, m_regenType{regenType} {}

InstantUsableItem::Type InstantUsableItem::getRegenType() const {
  return m_regenType;
}

int InstantUsableItem::getEffectValue() const { return m_effectValue; }

std::shared_ptr<Item> InstantUsableItem::clone() const {
  return std::make_shared<InstantUsableItem>(*this);
}

std::string InstantUsableItem::use(Creature &user) {
  std::ostringstream res{};
  if (!user.useActionPoints(getCostToUse())) {
    return res.str();
  }
  if (!isUnlimitedUse()) {
    if (m_usesLeft > 0)
      --m_usesLeft;
  }
  switch (m_regenType) {
    res << user.getName() << " uses " << m_name << " and ";
  case Type::healthRegen:
    user.addHealthPoints(m_effectValue);
    res << " regenerates " << m_effectValue << " health points.\n";
    break;
  case Type::movementRegen:
    res << " gains " << m_effectValue << " movement points.\n";
    user.addMovementPoints(m_effectValue);
    break;
  case Type::actionPointRegen:
    res << " gains " << m_effectValue << " action points.\n";
    user.addActionPoints(m_effectValue);
    break;
  default:
    res << "nothing happens.\n";
  }
  return res.str();
}

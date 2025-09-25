#include "gameObjects/creatures/Player.h"
#include "Settings.h"
#include "gameObjects/items/Equipment.h"
#include "gameObjects/items/Weapon.h"
#include "map/Map.h"
#include "map/Point.h"
#include "scripts/actions/BasicAttack.h"
#include "scripts/actions/CubeAoe.h"
#include "scripts/actions/Dodge.h"
#include "skills/SkillTree.h"
#include <memory>

Player::Player(const Point &position, std::string_view currentMap,
               int maxHealthPoints, Stats stats)
    : Creature('@', position, currentMap, maxHealthPoints, 0, "you"),
      m_stats{stats}, m_shoveAction{} {
  m_actions.emplace_back(std::make_unique<Dodge>(Dodge("Dodge")));
}

void Player::takeItem(std::shared_ptr<Item> item) {
  if (item) {
    m_inventory.emplace_back(item);
  }
}

void Player::displayInventory(std::size_t page) const {
  std::cout << "Inventory (page): " << page << '\n';
  std::size_t lenItemsDisplay{Settings::g_itemListSize};
  if (page == m_inventory.size() / Settings::g_itemListSize)
    // ie if viewing the last page
    lenItemsDisplay = m_inventory.size() % Settings::g_itemListSize;
  // only display remaining items
  for (std::size_t i{0}; i < lenItemsDisplay; ++i) {
    std::cout
        << i + 1 << ": "
        << m_inventory[page * Settings::g_itemListSize + i]->getDisplayForMenu()
        << '\n';
  }
}

int Player::numObjectsHeld() const {
  return static_cast<int>(m_inventory.size());
}

void Player::displayActions() const {
  std::cout << "Available actions: \n";
  for (std::size_t i{0}; i < m_actions.size(); ++i) {
    std::cout << i + 1 << ": " << *(m_actions[i]);
    if (m_actions[i]->getCurrentCooldown() > 0)
      std::cout << " cooldown: " << m_actions[i]->getCurrentCooldown();
    if (m_actions[i]->getMaxCharges() != -1)
      std::cout << " charges: " << m_actions[i]->getCurrentCharges() << '/'
                << m_actions[i]->getMaxCharges();
    std::cout << '\n';
  }
}

void Player::takeAllItems(Container &container) {
  auto containerItems = container.getContents(); // Get copies of shared_ptrs

  m_inventory.insert(m_inventory.end(), containerItems.begin(),
                     containerItems.end());
  container.clearContents();
}

void Player::updateActionsOnEquip() {
  m_actions.erase(m_actions.begin());
  auto m_rightHand = m_equipment.rightHand.lock();
  if (m_rightHand) {
    if (auto weapon = std::dynamic_pointer_cast<Weapon>(m_rightHand)) {
      if (weapon->getWeaponType() == Weapon::ranged) {
        m_actions.insert(
            m_actions.begin(),
            std::make_unique<BasicAttack>(
                "Ranged attack with right hand weapon", Stat::Dexterity));
        return;
      }
    }
  }
  // cases where no weapon or melee weapon equipped
  m_actions.insert(m_actions.begin(),
                   std::make_unique<BasicAttack>("Attack with melee weapon",
                                                 Stat::Strength));
}

bool Player::checkRequirements(std::shared_ptr<Item> item) const {
  if (!item)
    return false;
  return getStrength() >= item->getStrReq() &&
         getDexterity() >= item->getDexReq() &&
         getIntelligence() >= item->getIntReq() &&
         getConstitution() >= item->getConReq();
} // TODO: use temporary stats or raw values?

void Player::equipItem(std::shared_ptr<Equipment> item) {
  if (!checkRequirements(item))
    return;
  if (useActionPoints(1)) {
    if (item->getEquipmentType() == Equipment::EquipmentType::oneHanded) {
      auto equippedRightHand{m_equipment.rightHand.lock()};
      if (equippedRightHand) {
        if (equippedRightHand->getEquipmentType() ==
            Equipment::EquipmentType::twoHanded) {
          m_equipment.leftHand.reset();
        }
        equippedRightHand->setUnequipped();
        m_equipment.rightHand.reset();
        if (equippedRightHand == item)
          return;
      }
      m_equipment.rightHand = item;
      item->setEquipped();
    } else if (item->getEquipmentType() ==
               Equipment::EquipmentType::twoHanded) {
      auto equippedRightHand{m_equipment.rightHand.lock()};
      if (equippedRightHand) {
        equippedRightHand->setUnequipped();
        m_equipment.rightHand.reset();
      }
      auto equippedLeftHand{m_equipment.leftHand.lock()};
      if (equippedLeftHand) {
        equippedLeftHand->setUnequipped();
        m_equipment.leftHand.reset();
      }
      if (equippedRightHand && equippedRightHand == item)
        return;
      m_equipment.rightHand = item;
      m_equipment.leftHand = item;
      item->setEquipped();
    } else if (item->getEquipmentType() == Equipment::EquipmentType::shield) {
      auto equippedLeftHand{m_equipment.leftHand.lock()};
      if (equippedLeftHand) {
        if (equippedLeftHand->getEquipmentType() ==
            Equipment::EquipmentType::twoHanded) {
          m_equipment.rightHand.reset();
        }
        equippedLeftHand->setUnequipped();
        m_equipment.leftHand.reset();
        if (equippedLeftHand == item)
          return;
      }
      m_equipment.leftHand = item;
      item->setEquipped();
    } else if (item->getEquipmentType() ==
               Equipment::EquipmentType::chestArmor) {
      handleEquipSlot(m_equipment.chestArmor, item);
    } else if (item->getEquipmentType() == Equipment::EquipmentType::legArmor) {
      handleEquipSlot(m_equipment.legArmor, item);
    } else if (item->getEquipmentType() == Equipment::EquipmentType::helmet) {
      handleEquipSlot(m_equipment.helmet, item);
    } else if (item->getEquipmentType() == Equipment::EquipmentType::boots) {
      handleEquipSlot(m_equipment.boots, item);
    } else if (item->getEquipmentType() == Equipment::EquipmentType::gloves) {
      handleEquipSlot(m_equipment.gloves, item);
    }
    updateActionsOnEquip();
  }
}

void Player::handleEquipSlot(std::weak_ptr<Equipment> &currentItem,
                             std::shared_ptr<Equipment> newItem) {
  auto currentEquipped{currentItem.lock()};
  if (currentEquipped) {
    currentEquipped->setUnequipped();
    currentItem.reset();
    if (currentEquipped == newItem)
      return;
  }
  currentItem = newItem;
  newItem->setEquipped();
}

void Player::removeItem(std::shared_ptr<Item> item) {
  if (auto equipment = std::dynamic_pointer_cast<Equipment>(item)) {
    if (equipment->isEquipped()) {
      if (auto weapon{std::dynamic_pointer_cast<Weapon>(item)}) {
        if (weapon->getEquipmentType() == Equipment::EquipmentType::twoHanded) {
          m_equipment.leftHand.reset();
        } else if (m_equipment.leftHand.lock() == item) {
          m_equipment.leftHand.reset();
        } else
          m_equipment.rightHand.reset();
      }
    }
    equipment->setUnequipped();
  }
  auto it{std::find(m_inventory.begin(), m_inventory.end(), item)};
  if (it != m_inventory.end())
    m_inventory.erase(it);
  updateActionsOnEquip();
}

std::shared_ptr<Item> Player::getItem(std::size_t index) const {
  auto i{static_cast<std::size_t>(index)};
  if (i >= m_inventory.size())
    return nullptr;
  return m_inventory[i];
}

std::string Player::shove(GameSession &gameSession,
                          Directions::Direction direction) {
  return m_shoveAction.playerExecute(gameSession, direction);
}

std::string Player::useItem(std::shared_ptr<UsableItem> item) {
  auto res{item->use(*this)};
  if (!item->isUnlimitedUse() && item->getUsesLeft() == 0) {
    removeItem(item);
  }
  return res;
}

int Player::getStrength() const {
  return m_stats.strength + getStatModifier(Stat::Strength);
}
int Player::getDexterity() const {
  return m_stats.dexterity + getStatModifier(Stat::Dexterity);
}
int Player::getIntelligence() const {
  return m_stats.intelligence + getStatModifier(Stat::Intelligence);
}
int Player::getConstitution() const {
  return m_stats.constitution + getStatModifier(Stat::Constitution);
}
int Player::getArmor() const {
  int armorValue{0};
  auto chestArmor{m_equipment.chestArmor.lock()};
  if (chestArmor)
    armorValue += chestArmor->getArmorValue();
  auto legArmor{m_equipment.legArmor.lock()};
  if (legArmor)
    armorValue += legArmor->getArmorValue();
  auto helmet{m_equipment.helmet.lock()};
  if (helmet)
    armorValue += helmet->getArmorValue();
  auto boots{m_equipment.boots.lock()};
  if (boots)
    armorValue += boots->getArmorValue();
  auto gloves{m_equipment.gloves.lock()};
  if (gloves)
    armorValue += gloves->getArmorValue();
  auto shield{m_equipment.leftHand.lock()};
  if (shield && shield->getEquipmentType() == Equipment::EquipmentType::shield)
    armorValue += shield->getArmorValue();
  return armorValue + getStatModifier(Stat::Armor);
}
Stats Player::getStats() const {
  return m_stats; // unmodified stats
}
int Player::getEvasion() const {
  return getDexterity() * 5 + getStatModifier(Stat::Evasion);
}
int Player::getMeleeHitChance() const {
  return Settings::g_baseHitChance + (getStrength() * 5);
}
int Player::getDistanceHitChance() const {
  return Settings::g_baseHitChance + (getDexterity() * 5);
}
int Player::getMeleeDamage() const {
  auto rightHandItem{m_equipment.rightHand.lock()};
  if (auto rightHandWeapon = std::dynamic_pointer_cast<Weapon>(rightHandItem))
    if (rightHandWeapon->getWeaponType() == Weapon::melee)
      return rightHandWeapon->getDamage() + getStrength();
  return 1 + getStrength(); // magic number for fist damage.. will fix later
}

int Player::getMeleeRange() const {
  auto rightHandItem{m_equipment.rightHand.lock()};
  if (auto rightHandWeapon = std::dynamic_pointer_cast<Weapon>(rightHandItem))
    if (rightHandWeapon->getWeaponType() == Weapon::melee)
      return rightHandWeapon->getRange();
  return 1; // magic number for fist damage.. will fix later
}

int Player::getDistanceDamage() const {
  auto rightHandItem{m_equipment.rightHand.lock()};
  if (auto rightHandWeapon = std::dynamic_pointer_cast<Weapon>(rightHandItem))
    if (rightHandWeapon->getWeaponType() == Weapon::ranged)
      return rightHandWeapon->getDamage();
  return 0; // no ranged weapon equipped
}

int Player::getDistanceRange() const {
  auto rightHandItem{m_equipment.rightHand.lock()};
  if (auto rightHandWeapon = std::dynamic_pointer_cast<Weapon>(rightHandItem))
    if (rightHandWeapon->getWeaponType() == Weapon::ranged)
      return rightHandWeapon->getRange();
  return 0; // no ranged weapon equipped
}

Action *Player::getAction(std::size_t index) {
  auto i{static_cast<std::size_t>(index)};
  if (i >= m_actions.size())
    return nullptr;
  return m_actions[i].get();
}

const std::vector<std::unique_ptr<Action>> &Player::getAllActions() {
  return m_actions;
}

void Player::displayCharacterSheet() const {
  std::cout << "Character Sheet:\n";
  std::cout << "Name: " << getName() << '\n';
  std::cout << "Level: " << getLevel() << '\n';
  std::cout << "XP: " << getCurrentXP() << '/' << getXpToNextLevel() << "\n\n";
  std::cout << "Health: " << getHealthPoints() << '/' << getMaxHealthPoints()
            << "\n\n\n";
  std::cout << "Armor: " << getArmor() << '\n';
  std::cout << "Strength: " << getStrength() << '\n';
  std::cout << "Dexterity: " << getDexterity() << '\n';
  std::cout << "Intelligence: " << getIntelligence() << '\n';
  std::cout << "Constitution: " << getConstitution() << "\n\n";
  std::cout << "Evasion: " << getEvasion() << "\n\n";
  std::cout << "Equipped items:\n";
  auto rightHandItem{m_equipment.rightHand.lock()};
  if (rightHandItem)
    std::cout << " Right hand: " << rightHandItem->getDisplayForMenu() << '\n';
  else
    std::cout << " Right hand: None\n";
  auto leftHandItem{m_equipment.leftHand.lock()};
  if (leftHandItem)
    std::cout << " Left hand: " << leftHandItem->getDisplayForMenu() << '\n';
  else
    std::cout << " Left hand: None\n";

  std::cout << "\n\nCurrent effects:\n";
  for (const auto &effect : m_passiveEffects) {
    std::cout << effect->display() << '\n';
  }
}

int Player::getCurrentXP() const { return m_currentXP; }
int Player::getLevel() const { return m_level; }
int Player::getXpToNextLevel() const { return m_xpToNextLevel; }
void Player::addXP(int xp) { m_currentXP += xp; }
bool Player::canLevelUp() const {
  return m_currentXP >= m_xpToNextLevel && !inCombat();
}

void Player::addAction(std::unique_ptr<Action> action) {
  std::string_view actionName{action->getName()};
  auto it{std::find_if(m_actions.begin(), m_actions.end(),
                       [actionName](const std::unique_ptr<Action> &a) {
                         return a->getName() == actionName;
                       })};
  if (it == m_actions.end()) {
    m_actions.emplace_back(std::move(action));
  }
}

void Player::levelUp(Stat stat) {
  m_level++;
  m_currentXP -= m_xpToNextLevel;
  switch (stat) {
  case Stat::Strength:
    m_stats.strength += 1;
    break;
  case Stat::Dexterity:
    m_stats.dexterity += 1;
    break;
  case Stat::Intelligence:
    m_stats.intelligence += 1;
    break;
  case Stat::Constitution:
    m_stats.constitution += 1;
    m_maxHealthPoints += m_level; // each con point gives 1 extra hp per level
    break;
  default:
    break;
  }
  m_maxHealthPoints += 5; // each level gives 5 extra hp
  SkillTree::addSkillStatSpread(*this);
}
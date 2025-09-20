#include "gameObjects/creatures/Player.h"
#include "Settings.h"
#include "gameObjects/items/Weapon.h"
#include "map/Map.h"
#include "map/Point.h"
#include "scripts/MeleeAttack.h"
#include "scripts/RangedAttack.h"
#include <memory>

Player::Player(const Point &position, std::string_view currentMap,
               int maxHealthPoints, Stats stats)
    : Creature('@', position, currentMap, maxHealthPoints, "you"),
      m_stats{stats}, m_shoveAction{} {}

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
    std::cout << i + 1 << ": "
              << *(m_inventory[page * Settings::g_itemListSize + i]) << '\n';
  }
}

int Player::numObjectsHeld() const {
  return static_cast<int>(m_inventory.size());
}

void Player::displayActions() const {
  std::cout << "Available actions: \n";
  for (std::size_t i{0}; i < m_actions.size(); ++i) {
    std::cout << i + 1 << ": " << *(m_actions[i]) << '\n';
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
        m_actions.insert(m_actions.begin(),
                         std::make_shared<RangedAttack>(
                             "Ranged attack with right hand weapon"));
        return;
      }
    }
  }
  // cases where no weapon or melee weapon equipped
  m_actions.insert(m_actions.begin(),
                   std::shared_ptr<MeleeAttack>(m_meleeAttack));
}

void Player::equipItem(std::shared_ptr<Item> item) {
  if (useActionPoints(1)) {
    if (item->getType() == Item::ItemType::oneHanded) {
      auto equippedRightHand{m_equipment.rightHand.lock()};
      if (equippedRightHand) {
        if (equippedRightHand->getType() == Item::ItemType::twoHanded) {
          m_equipment.leftHand.reset();
        }
        equippedRightHand->setUnequipped();
        m_equipment.rightHand.reset();
        if (equippedRightHand == item)
          return;
      }
      m_equipment.rightHand = item;
      item->setEquipped();
    } else if (item->getType() == Item::ItemType::twoHanded) {
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
    }
    updateActionsOnEquip();
  }
}

void Player::removeItem(std::shared_ptr<Item> item) {
  if (item->isEquipped()) {
    if (auto weapon{std::dynamic_pointer_cast<Weapon>(item)}) {
      if (weapon->getType() == Item::ItemType::twoHanded) {
        m_equipment.leftHand.reset();
      } else if (m_equipment.leftHand.lock() == item) {
        m_equipment.leftHand.reset();
      } else
        m_equipment.rightHand.reset();
    }
  }
  item->setUnequipped();
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

std::string Player::meleeAttack(GameSession &gameSession,
                                Directions::Direction direction) {
  return m_meleeAttack->playerExecute(gameSession, direction);
}

int Player::getStrength() const { return m_stats.strength; }
int Player::getDexterity() const { return m_stats.dexterity; }
int Player::getIntelligence() const { return m_stats.intelligence; }
int Player::getConstitution() const { return m_stats.constitution; }

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

std::shared_ptr<Action> Player::getAction(std::size_t index) const {
  auto i{static_cast<std::size_t>(index)};
  if (i >= m_actions.size())
    return nullptr;
  return m_actions[i];
}

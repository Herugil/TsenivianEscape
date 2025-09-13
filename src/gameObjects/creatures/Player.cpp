#include "gameObjects/creatures/Player.h"
#include "map/Map.h"
#include "map/Point.h"
#include "scripts/MeleeAttack.h"
#include <memory>

Player::Player(const Point &position, std::string_view currentMap,
               int maxHealthPoints)
    : Creature('@', position, currentMap, maxHealthPoints, "you"),
      m_shoveAction{} {}

void Player::takeItem(std::shared_ptr<Item> item) {
  if (item) {
    m_inventory.emplace_back(item);
  }
}

void Player::inventoryMenu() {
  displayInventory();
  CommandHandler::handleInventoryCommands(*this);
}

void Player::actionMenu(GameSession &gameSession) {
  displayActions();
  CommandHandler::handleActionCommands(gameSession);
}

void Player::displayInventory() const {
  std::cout << "Inventory: \n";
  for (std::size_t i{0}; i < m_inventory.size(); ++i) {
    std::cout << i + 1 << ": " << *(m_inventory[i]) << '\n';
  }
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

void Player::equipItem(std::shared_ptr<Item> item) {
  if (useActionPoints(1)) {
    if (item->getType() == Item::ItemType::oneHanded) {
      auto equippedRightHand{m_equipment.rightHand.lock()};
      if (equippedRightHand) {
        equippedRightHand->setEquipped();
        m_equipment.rightHand.reset();
        if (equippedRightHand == item)
          return;
      }
      m_equipment.rightHand = item;
      item->setEquipped();
    }
  }
}

std::shared_ptr<Item> Player::getItem(std::size_t index) const {
  auto i{static_cast<std::size_t>(index)};
  if (i >= m_inventory.size())
    return nullptr;
  return m_inventory[i];
}

void Player::shove(GameSession &gameSession, Directions::Direction direction) {
  m_shoveAction.playerExecute(gameSession, direction);
}

int Player::getMeleeDamage() const {
  auto rightHandWeapon{m_equipment.rightHand.lock()};
  if (rightHandWeapon)
    return rightHandWeapon->getDamage();
  else
    return 1; // magic number for fist damage.. will fix later
}

int Player::getMeleeRange() const {
  auto rightHandWeapon{m_equipment.rightHand.lock()};
  if (rightHandWeapon)
    return rightHandWeapon->getRange();
  else
    return 1; // magic number for fist damage.. will fix later
}

std::shared_ptr<Action> Player::getAction(std::size_t index) const {
  auto i{static_cast<std::size_t>(index)};
  if (i >= m_actions.size())
    return nullptr;
  return m_actions[i];
}

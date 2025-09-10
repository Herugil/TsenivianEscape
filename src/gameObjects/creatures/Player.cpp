#include "gameObjects/creatures/Player.h"
#include "map/Map.h"
#include "map/Point.h"
#include <memory>

Player::Player(const Point &position, int healthPoints)
    : Creature('@', position, healthPoints) {}
void Player::takeItem(std::shared_ptr<Item> item) {
  if (item) {
    m_inventory.emplace_back(item);
  }
}

void Player::inventoryMenu() {
  displayInventory();
  CommandHandler::handleInventoryCommands(*this);
}

void Player::displayInventory() const {
  std::cout << "Inventory: \n";
  for (std::size_t i{0}; i < m_inventory.size(); ++i) {
    std::cout << i + 1 << ": " << *(m_inventory[i]) << '\n';
  }
}

void Player::takeAllItems(Container &container) {
  auto containerItems = container.getContents(); // Get copies of shared_ptrs

  m_inventory.insert(m_inventory.end(), containerItems.begin(),
                     containerItems.end());
  container.clearContents();
}

void Player::equipItem(std::shared_ptr<Item> item) {
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

std::shared_ptr<Item> Player::getItem(std::size_t index) const {
  auto i{static_cast<std::size_t>(index)};
  if (i >= m_inventory.size())
    return nullptr;
  return m_inventory[i];
}

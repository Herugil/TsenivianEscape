#include "Player.h"
#include "Map.h"
#include "Point.h"
#include <memory>

Player::Player(const Point &position) : Creature('@', position) {}
void Player::takeItem(std::shared_ptr<Item> item) {
  if (item) {
    m_inventory.emplace_back(item);
  }
}

void Player::displayInventory() const {
  std::cout << "Inventory: \n";
  for (std::size_t i{0}; i < m_inventory.size(); ++i) {
    std::cout << i << ": " << *(m_inventory[i]) << '\n';
  }
}
void Player::takeAllItems(Container &container) {
  auto containerItems = container.getContents(); // Get copies of shared_ptrs

  m_inventory.insert(m_inventory.end(), containerItems.begin(),
                     containerItems.end());
  container.clearContents();
}

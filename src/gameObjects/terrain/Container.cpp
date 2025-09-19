#include "gameObjects/terrain/Container.h"
#include "core/GameState.h"
#include "input/Input.h"
#include "map/Point.h"
#include <iostream>
#include <memory>
#include <vector>

Container::Container(const Point &point, std::string_view currentMap,
                     std::string_view name, std::string_view description,
                     char symbol)
    : GameObject(true, false, symbol, currentMap, point, name, description) {}
Container::Container(std::vector<std::shared_ptr<Item>> items,
                     const Point &point, std::string_view currentMap,
                     std::string_view name, std::string_view description,
                     char symbol)
    : GameObject(true, false, symbol, currentMap, point, name, description),
      m_heldItems{std::move(items)} {}

void Container::displayContents() {
  if (m_heldItems.empty())
    std::cout << "This container is empty.\n";
  else
    std::cout << "Contains:\n";
  for (std::size_t i{0}; i < m_heldItems.size(); ++i) {
    std::cout << i + 1 << ": " << *(m_heldItems[i]) << '\n';
  }
}

InteractionResult Container::playerInteraction() {
  return {GameState::Container, this};
}

std::vector<std::shared_ptr<Item>> &Container::getContents() {
  return m_heldItems;
}

void Container::addItem(std::shared_ptr<Item> item) {
  m_heldItems.push_back(item);
}

std::shared_ptr<Item> Container::popItem(std::size_t index) {
  if (index >= m_heldItems.size())
    return nullptr;

  auto item = m_heldItems[index];
  m_heldItems.erase(m_heldItems.begin() + static_cast<int>(index));
  return item;
}

void Container::clearContents() { m_heldItems.clear(); }
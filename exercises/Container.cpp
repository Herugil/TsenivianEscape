#include "Container.h"
#include "Point.h"
#include <iostream>
#include <vector>

Container::Container(const Point &point, std::string_view description)
    : GameObject(false, true, 'c', point, description) {}
Container::Container(std::vector<std::shared_ptr<Item>> items,
                     const Point &point, std::string_view description)
    : GameObject(false, true, 'c', point, description),
      m_heldItems{std::move(items)} {}

void Container::displayContents() {
  if (m_heldItems.empty())
    std::cout << "This container is empty.\n";
  else
    std::cout << "Contains:\n";
  for (std::size_t i{0}; i < m_heldItems.size(); ++i) {
    std::cout << i << ": " << *(m_heldItems[i]) << '\n';
  }
}

void Container::playerInteraction() {
  std::cout << m_description << '\n';
  displayContents();
}
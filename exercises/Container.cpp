#include "Container.h"
#include "Point.h"
#include <iostream>
#include <vector>

Container::Container(const Point& point) : GameObject(false, false, 'c', point){}
Container::Container(std::vector<std::unique_ptr<Item>> items, const Point& point)
    : GameObject(false, false, 'c', point), m_heldItems{ std::move(items) } {}
void Container::displayContents() {
    for (std::size_t i{ 0 }; i < m_heldItems.size(); ++i) {
        std::cout << i << ": " << *(m_heldItems[i]) << '\n';
    }
}
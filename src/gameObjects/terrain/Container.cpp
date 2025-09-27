#include "gameObjects/terrain/Container.h"
#include "core/GameState.h"
#include "dataLoading/parseJson.h"
#include "input/Input.h"
#include "map/Point.h"
#include <iostream>
#include <memory>
#include <vector>

Container::Container(const Point &point, std::string_view currentMap,
                     std::string_view name, std::string_view description,
                     char symbol, bool locked, std::string_view keyId)
    : GameObject(true, false, symbol, currentMap, point, name, description,
                 locked, keyId) {}
Container::Container(std::vector<std::shared_ptr<Item>> &&items,
                     const Point &point, std::string_view currentMap,
                     std::string_view name, std::string_view description,
                     char symbol, bool locked, std::string_view keyId)
    : GameObject(true, false, symbol, currentMap, point, name, description,
                 locked, keyId),
      m_heldItems{std::move(items)} {}

void Container::displayContents() {
  if (m_heldItems.empty())
    std::cout << "This container is empty.\n";
  else
    std::cout << "Contains:\n";
  for (std::size_t i{0}; i < m_heldItems.size(); ++i) {
    std::cout << i + 1 << ": " << m_heldItems[i]->getDisplayForMenu() << '\n';
  }
}

InteractionResult Container::playerInteraction() {
  if (m_locked)
    return {GameState::UnlockMenu, this};
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

json Container::toJson() const {
  json j = GameObject::toJson();
  j["contents"] = json::array();
  for (const auto &item : m_heldItems) {
    j["contents"].push_back(item->toJson());
  }
  return j;
}

Container Container::loadFromJson(
    const json &j,
    const std::unordered_map<std::string, std::shared_ptr<Item>> &items,
    std::string_view mapToPlace) {
  Point pos{j["position"][0], j["position"][1]};
  std::vector<std::shared_ptr<Item>> loot{};
  char symbol{std::string(j["symbol"])[0]};
  std::string name{j["name"]};
  std::string desc{j["description"]};
  const auto &itemIds{j["contents"]};
  for (auto itemId : itemIds) {
    auto item{DataLoader::parseItem(itemId, items)};
    if (item)
      loot.push_back(item);
  }
  bool locked{false};
  std::string keyId{};
  if (j.contains("locked")) {
    locked = j["locked"];
    keyId = j.contains("keyId") ? std::string(j["keyId"]) : "";
  }
  return Container{
      std::move(loot), pos, mapToPlace, name, desc, symbol, locked, keyId,
  };
}
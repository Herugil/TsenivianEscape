#pragma once
#include "gameObjects/GameObject.h"
#include "gameObjects/items/Item.h"
#include "input/Input.h"
#include "map/Point.h"
#include <memory>
#include <string_view>
#include <vector>

struct InteractionResult;
class Player;

class Container : public GameObject {
private:
  std::vector<std::shared_ptr<Item>> m_heldItems{};

public:
  Container(const Point &point, std::string_view name,
            std::string_view currentMap, std::string_view description = "",
            char symbol = 'c');
  Container(std::vector<std::shared_ptr<Item>> items, const Point &point,
            std::string_view currentMap, std::string_view name,
            std::string_view description = "", char symbol = 'c');
  std::vector<std::shared_ptr<Item>> &getContents();
  void displayContents();
  void clearContents();
  InteractionResult playerInteraction() override;
  std::shared_ptr<Item> popItem(std::size_t index);
};

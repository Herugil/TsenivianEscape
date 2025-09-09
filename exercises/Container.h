#pragma once
#include "GameObject.h"
#include "Input.h"
#include "Item.h"
#include "Point.h"
#include <memory>
#include <string_view>
#include <vector>

class Player;

class Container : public GameObject {
private:
  std::vector<std::shared_ptr<Item>> m_heldItems{};

public:
  Container(const Point &point, std::string_view description = "");
  Container(std::vector<std::shared_ptr<Item>> items, const Point &point,
            std::string_view description = "");
  std::vector<std::shared_ptr<Item>> &getContents();
  void displayContents();
  void clearContents();
  void playerInteraction(Player &player) override;
  std::shared_ptr<Item> popItem(std::size_t index);
};

#pragma once
#include "GameObject.h"
#include "Item.h"
#include "Point.h"
#include <memory>
#include <string_view>
#include <vector>

class Container : public GameObject {
private:
  std::vector<std::shared_ptr<Item>> m_heldItems{};

public:
  Container(const Point &point, std::string_view description = "");
  Container(std::vector<std::shared_ptr<Item>> items, const Point &point,
            std::string_view description = "");
  void displayContents();
  void playerInteraction() override;
};

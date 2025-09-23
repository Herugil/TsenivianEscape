#pragma once
#include "gameObjects/items/Item.h"

class Creature;
class UsableItem : public Item {
protected:
  int m_costToUse{};
  bool m_isUnlimitedUse{false};
  int m_usesLeft{-1};

public:
  UsableItem(std::string_view name, std::string_view id, int costToUse = 1,
             bool isUnlimitedUse = false, int usesLeft = -1,
             std::string_view description = "")
      : Item{name, id, description}, m_costToUse{costToUse},
        m_isUnlimitedUse{isUnlimitedUse}, m_usesLeft{usesLeft} {}
  int getCostToUse() const { return m_costToUse; }
  bool isUnlimitedUse() const { return m_isUnlimitedUse; }
  int getUsesLeft() const { return m_usesLeft; }
  std::shared_ptr<Item> clone() const override = 0;
  virtual std::string use(Creature &user) = 0;
  virtual ~UsableItem() = default;
};
#pragma once
#include "gameObjects/items/UsableItem.h"

class InstantUsableItem : public UsableItem {
public:
  enum Type {
    healthRegen,
    movementRegen,
    actionPointRegen,
    nbTypes,
  };

private:
  int m_effectValue{};
  Type m_regenType{}; // this should be effect type (potion can have an
                      // action->use effect like potion of fire breath)

public:
  InstantUsableItem(std::string_view name, std::string_view id, Type regenType,
                    int effectValue, int costToUse = 1,
                    bool isUnlimitedUse = false, int usesLeft = -1,
                    std::string_view description = "");
  Type getRegenType() const;
  int getEffectValue() const;
  std::shared_ptr<Item> clone() const override;
  std::string use(Creature &user) override;
  virtual ~InstantUsableItem() = default;
};
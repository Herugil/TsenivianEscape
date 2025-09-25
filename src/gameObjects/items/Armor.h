#pragma once
#include "gameObjects/items/Equipment.h"

class Armor : public Equipment {
protected:
  int m_armorValue{};

public:
  Armor(std::string_view name, std::string_view id,
        std::string_view equipmentType = "chestArmor",
        std::string_view description = "", int armorValue = 1, int strReq = 0,
        int dexReq = 0, int intReq = 0, int conReq = 0)
      : Equipment{name,   id,     equipmentType, description,
                  strReq, dexReq, intReq,        conReq},
        m_armorValue{armorValue} {}
  std::shared_ptr<Item> clone() const override {
    return std::make_shared<Armor>(*this);
  }
  std::string getDisplayItem() const override {
    std::ostringstream res;
    res << m_name << '\n'
        << m_description << "\nArmor value: " << m_armorValue
        << (isEquipped() ? "\nEquipped" : "");
    return res.str();
  };
  int getArmorValue() const override { return m_armorValue; }
  virtual ~Armor() = default;
};
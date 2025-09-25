#pragma once
#include "gameObjects/items/Item.h"

class Equipment : public Item {
public:
  enum class EquipmentType {
    oneHanded,
    twoHanded,
    chestArmor,
    legArmor,
    helmet,
    boots,
    gloves,
    shield,
  };

protected:
  EquipmentType m_equipmentType{};
  bool m_isEquipped{false};

public:
  Equipment(std::string_view name, std::string_view id,
            std::string_view equipmentType, std::string_view description = "",
            int strReq = 0, int dexReq = 0, int intReq = 0, int conReq = 0)
      : Item{name, id, description, strReq, dexReq, intReq, conReq},
        m_equipmentType{getTypeFromStr(equipmentType)} {}
  EquipmentType getEquipmentType() const { return m_equipmentType; }
  bool isEquipped() const { return m_isEquipped; }
  void setEquipped() { m_isEquipped = true; }
  void setUnequipped() { m_isEquipped = false; }
  virtual const std::string getDisplayForMenu() const override {
    return m_name + (isEquipped() ? " (E)" : "");
  };
  virtual std::string getDisplayItem() const override {
    std::ostringstream res;
    res << m_name << '\n'
        << m_description << (isEquipped() ? "\nEquipped" : "");
    return res.str();
  };
  virtual int getArmorValue() const { return 0; }
  virtual std::shared_ptr<Item> clone() const override = 0;
  static EquipmentType getTypeFromStr(std::string_view typeName) {
    if (typeName == "oneHanded")
      return Equipment::EquipmentType::oneHanded;
    if (typeName == "twoHanded")
      return Equipment::EquipmentType::twoHanded;
    if (typeName == "chestArmor")
      return Equipment::EquipmentType::chestArmor;
    if (typeName == "legArmor")
      return Equipment::EquipmentType::legArmor;
    if (typeName == "helmet")
      return Equipment::EquipmentType::helmet;
    if (typeName == "boots")
      return Equipment::EquipmentType::boots;
    if (typeName == "gloves")
      return Equipment::EquipmentType::gloves;
    if (typeName == "shield")
      return Equipment::EquipmentType::shield;
    return Equipment::EquipmentType::oneHanded;
  }
  virtual ~Equipment() = default;
};
#pragma once
#include <iostream>
#include <sstream>
#include <string>

class Item {
public:
  enum class ItemType {
    oneHanded,
    twoHanded,
    armor,
    usableItem,
    instantUsableItem,
    other,
  };

protected:
  std::string m_name{};
  std::string m_id{};
  ItemType m_type{};
  bool m_isEquipped{};
  std::string m_description{};

public:
  Item(std::string_view name, std::string_view id,
       ItemType itemType = ItemType::other, std::string_view description = "")
      : m_name{name}, m_id{id}, m_type{itemType}, m_isEquipped{false},
        m_description{description} {}
  const std::string &getId() const { return m_id; }
  ItemType getType() const { return m_type; }
  static ItemType getTypeFromStr(std::string_view typeStr) {
    if (typeStr == "oneHanded")
      return ItemType::oneHanded;
    if (typeStr == "twoHanded")
      return ItemType::twoHanded;
    if (typeStr == "armor")
      return ItemType::armor;
    if (typeStr == "usableItem")
      return ItemType::usableItem;
    if (typeStr == "instantUsableItem")
      return ItemType::instantUsableItem;
    return ItemType::other;
  }
  bool isEquipped() const { return m_isEquipped; }
  void setEquipped() { m_isEquipped = true; }
  void setUnequipped() { m_isEquipped = false; }
  bool isEquipment() const {
    return m_type == ItemType::oneHanded || m_type == ItemType::twoHanded ||
           m_type == ItemType::armor;
  } // what is this design was I drunk ? Equipment should be its own class
    // OBVIOUSLY - this will do for now
  bool isUsable() const {
    return m_type == ItemType::usableItem ||
           m_type == ItemType::instantUsableItem;
  }
  virtual std::string getDisplayItem() const {
    std::ostringstream res;
    res << m_name << '\n'
        << m_description << (isEquipped() ? "\nEquipped" : "");
    return res.str();
  };
  virtual std::shared_ptr<Item> clone() const = 0;
  friend std::ostream &operator<<(std::ostream &out, const Item &item) {
    out << item.m_name;
    if (item.isEquipped())
      out << "    E";
    return out;
  }
  virtual ~Item() = default;
};

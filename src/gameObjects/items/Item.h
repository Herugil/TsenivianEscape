#pragma once
#include <iostream>
#include <string>
class Item {
public:
  enum class ItemType {
    oneHanded,
    twoHanded,
    armor,
    consumable,
    other,
  };

protected:
  std::string m_name{};
  ItemType m_type{};
  bool m_isEquipped{};

public:
  Item(std::string_view name, ItemType itemType = ItemType::other)
      : m_name{name}, m_type{itemType}, m_isEquipped{false} {}
  friend std::ostream &operator<<(std::ostream &out, const Item &item) {
    out << item.m_name;
    if (item.isEquipped())
      out << "    E";
    return out;
  }
  ItemType getType() const { return m_type; }
  bool isEquipped() const { return m_isEquipped; }
  void setEquipped() { m_isEquipped = !m_isEquipped; }
  // thanks to solene
  virtual int getDamage() const { return 0; };
  virtual int getRange() const { return 0; }
  virtual std::shared_ptr<Item> clone() const = 0;
  virtual ~Item() = default;
};

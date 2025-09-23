#pragma once
#include <iostream>
#include <sstream>
#include <string>

class Item {
protected:
  std::string m_name{};
  std::string m_id{};
  std::string m_description{};

public:
  Item(std::string_view name, std::string_view id,
       std::string_view description = "")
      : m_name{name}, m_id{id}, m_description{description} {}
  const std::string &getId() const { return m_id; }

  virtual const std::string getDisplayForMenu() const { return m_name; };
  virtual std::string getDisplayItem() const {
    std::ostringstream res;
    res << m_name << '\n' << m_description << '\n';
    return res.str();
  };
  virtual std::shared_ptr<Item> clone() const = 0;
  virtual ~Item() = default;
};

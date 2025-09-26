#pragma once
#include <iostream>
#include <sstream>
#include <string>

class Item {
protected:
  std::string m_name{};
  std::string m_id{};
  std::string m_description{};
  int m_strReq{0};
  int m_dexReq{0};
  int m_intReq{0};
  int m_conReq{0};

public:
  Item(std::string_view name, std::string_view id,
       std::string_view description = "", int strReq = 0, int dexReq = 0,
       int intReq = 0, int conReq = 0)
      : m_name{name}, m_id{id}, m_description{description}, m_strReq{strReq},
        m_dexReq{dexReq}, m_intReq{intReq}, m_conReq{conReq} {}
  const std::string &getId() const { return m_id; }

  virtual const std::string getDisplayForMenu() const { return m_name; };
  virtual std::string getDisplayItem() const {
    std::ostringstream res;
    res << m_name << '\n' << m_description << '\n';
    return res.str();
  };
  virtual std::shared_ptr<Item> clone() const {
    return std::make_shared<Item>(*this);
  };
  int getStrReq() const { return m_strReq; }
  int getDexReq() const { return m_dexReq; }
  int getIntReq() const { return m_intReq; }
  int getConReq() const { return m_conReq; }
  const std::string &getName() const { return m_name; }
  virtual ~Item() = default;
};

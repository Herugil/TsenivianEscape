#pragma once
#include "input/Directions.h"
#include "input/Input.h"
#include "map/Point.h"
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

using json = nlohmann::json;

struct InteractionResult;
class Player;

class GameObject {
protected:
  bool m_isMoveable{};
  bool m_traversable{};
  char m_symbol{};
  std::string m_currentMap{};
  Point m_position{};
  std::string m_name{};
  std::string m_description{};
  bool m_locked{false};
  std::string m_keyId{};

private:
public:
  explicit GameObject(bool isMoveable = false, bool isTraversable = false,
                      char symbol = ' ', std::string_view currentMap = "",
                      Point position = Point(), std::string_view name = "",
                      std::string_view description = "", bool locked = false,
                      std::string_view keyId = "");
  const Point &getPosition() const;
  const std::string &getCurrentMap() const;
  void setCurrentMap(std::string_view map);
  void setSymbol(char symbol);
  char getSymbol() const;
  const std::string &getName() const;
  virtual std::string getDescription() const;
  bool isTraversable() const;
  bool isMoveable() const;
  void setPosition(const Point &point);
  const std::string &getKeyId() const;
  virtual void unlock();
  virtual void setUsed();
  virtual InteractionResult playerInteraction(); // not const because some
                                                 // object states can be changed
                                                 // through this function

  virtual json toJson() const;

  virtual ~GameObject() = default;
};

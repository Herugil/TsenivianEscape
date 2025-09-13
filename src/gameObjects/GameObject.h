#pragma once
#include "input/Directions.h"
#include "input/Input.h"
#include "map/Point.h"
#include <string>
#include <string_view>

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

private:
public:
  explicit GameObject(bool isMoveable = false, bool isTraversable = false,
                      char symbol = ' ', std::string_view currentMap = "",
                      Point position = Point(), std::string_view name = "",
                      std::string_view description = "");
  const Point &getPosition() const;
  const std::string &getCurrentMap() const;
  void setCurrentMap(std::string_view map);
  char getSymbol() const;
  const std::string &getName() const;
  bool isTraversable() const;
  bool isMoveable() const;
  void setPosition(const Point &point);
  virtual void
  playerInteraction(Player &player); // not const because some object states can
                                     // be changed through this function
  virtual ~GameObject() = default;
};

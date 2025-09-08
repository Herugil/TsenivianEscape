#pragma once
#include "Directions.h"
#include "Point.h"
#include <string>
#include <string_view>

class Map;
class GameObject {
  bool m_isMoveable{};
  bool m_traversable{};
  char m_symbol{};

protected:
  Point m_position{};
  std::string m_description{};

private:
public:
  explicit GameObject(bool isMoveable = false, bool isTraversable = false,
                      char symbol = ' ', Point position = Point(),
                      std::string_view description = "");
  const Point &getPosition() const;
  char getSymbol() const;
  bool isTraversable() const;
  bool isMoveable() const;
  void setPosition(const Point &point);
  virtual void playerInteraction(); // not const because some object states can
                                    // be changed through this function
  virtual ~GameObject() = default;
};

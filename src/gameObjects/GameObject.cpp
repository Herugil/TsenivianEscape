#include "gameObjects/GameObject.h"
#include "map/Map.h"
#include <iostream>
#include <string_view>

GameObject::GameObject(bool isMoveable, bool isTraversable, char symbol,
                       Point position, std::string_view description)
    : m_isMoveable{isMoveable}, m_traversable{isTraversable}, m_symbol{symbol},
      m_position{position}, m_description{description} {}

const Point &GameObject::getPosition() const { return m_position; }

bool GameObject::isTraversable() const { return m_traversable; }

bool GameObject::isMoveable() const { return m_isMoveable; }

char GameObject::getSymbol() const { return m_symbol; }

void GameObject::setPosition(const Point &point) { m_position = point; }

void GameObject::playerInteraction([[maybe_unused]] Player &player) {
  std::cout << m_description;
}

std::ostream &operator<<(std::ostream &out, const GameObject &gameObject) {
  out << gameObject.getSymbol();
  return out;
}
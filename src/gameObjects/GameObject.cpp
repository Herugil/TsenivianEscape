#include "gameObjects/GameObject.h"
#include "core/GameStateManager.h"
#include <iostream>
#include <string_view>

GameObject::GameObject(bool isMoveable, bool isTraversable, char symbol,
                       std::string_view currentMap, Point position,
                       std::string_view name, std::string_view description)
    : m_isMoveable{isMoveable}, m_traversable{isTraversable}, m_symbol{symbol},
      m_currentMap{currentMap}, m_position{position}, m_name{name},
      m_description{description} {}

void GameObject::setSymbol(char symbol) { m_symbol = symbol; }

const Point &GameObject::getPosition() const { return m_position; }

bool GameObject::isTraversable() const { return m_traversable; }

bool GameObject::isMoveable() const { return m_isMoveable; }

char GameObject::getSymbol() const { return m_symbol; }

const std::string &GameObject::getCurrentMap() const { return m_currentMap; }
void GameObject::setCurrentMap(std::string_view map) { m_currentMap = map; }

void GameObject::setPosition(const Point &point) { m_position = point; }

InteractionResult GameObject::playerInteraction() {
  return {GameState::DisplayBlocking, this};
}
const std::string &GameObject::getName() const { return m_name; }
const std::string &GameObject::getDescription() const { return m_description; }

std::ostream &operator<<(std::ostream &out, const GameObject &gameObject) {
  out << gameObject.getSymbol();
  return out;
}
#include "gameObjects/GameObject.h"
#include "core/GameStateManager.h"
#include <iostream>
#include <string_view>

using json = nlohmann::json;

GameObject::GameObject(bool isMoveable, bool isTraversable, char symbol,
                       std::string_view currentMap, Point position,
                       std::string_view name, std::string_view description,
                       bool locked, std::string_view keyId)
    : m_isMoveable{isMoveable}, m_traversable{isTraversable}, m_symbol{symbol},
      m_currentMap{currentMap}, m_position{position}, m_name{name},
      m_description{description}, m_locked{locked}, m_keyId{keyId} {}

void GameObject::setSymbol(char symbol) { m_symbol = symbol; }

const Point &GameObject::getPosition() const { return m_position; }

bool GameObject::isTraversable() const { return m_traversable; }

bool GameObject::isMoveable() const { return m_isMoveable; }

char GameObject::getSymbol() const { return m_symbol; }

const std::string &GameObject::getCurrentMap() const { return m_currentMap; }
void GameObject::setCurrentMap(std::string_view map) { m_currentMap = map; }

void GameObject::setPosition(const Point &point) { m_position = point; }

const std::string &GameObject::getKeyId() const { return m_keyId; }
void GameObject::unlock() { m_locked = false; }

InteractionResult GameObject::playerInteraction() {
  if (m_locked) {
    return {GameState::UnlockMenu, this};
  }
  return {GameState::DisplayBlocking, this};
}
const std::string &GameObject::getName() const { return m_name; }
std::string GameObject::getDescription() const { return m_description; }

std::ostream &operator<<(std::ostream &out, const GameObject &gameObject) {
  out << gameObject.getSymbol();
  return out;
}

void GameObject::setUsed() {
  // for now, specific to resting places
  return;
}

json GameObject::toJson() const {
  json j;
  j["name"] = m_name;
  j["description"] = m_description;
  j["symbol"] = std::string(1, m_symbol);
  j["currentMap"] = m_currentMap;
  j["position"] = {m_position.getX(), m_position.getY()};
  j["isMoveable"] = m_isMoveable;
  j["traversable"] = m_traversable;
  j["locked"] = m_locked;
  if (m_locked)
    j["keyId"] = m_keyId;
  return j;
}
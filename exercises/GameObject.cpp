#include "GameObject.h"
#include <iostream>
#include "Map.h"

GameObject::GameObject(bool isMoveable, bool isTraversable,
	char symbol, Point position): m_isMoveable{isMoveable}, m_traversable{isTraversable},
	m_symbol{symbol}, m_position{position}{ }

const Point &GameObject::getPosition() const {
	return m_position;
}

bool GameObject::isTraversable() const {
	return m_traversable;
}

bool GameObject::isMoveable() const {
	return m_isMoveable;
}

char GameObject::getSymbol() const { return m_symbol; }

void GameObject::setPosition(const Point& point) {
	m_position = point;
}

std::ostream& operator<<(std::ostream& out, const GameObject& gameObject) {
	out << gameObject.getSymbol();
	return out;
}
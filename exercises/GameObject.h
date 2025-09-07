#pragma once
#include "Point.h"
#include "Directions.h"
class Map;
class GameObject
{
	bool m_isMoveable{};
	bool m_traversable{};
	char m_symbol{};
protected:
	Point m_position{};
public:
	explicit GameObject(bool isMoveable = false, bool isTraversable = false, char symbol = ' ', Point position = Point());
	const Point& getPosition() const;
	char getSymbol() const;
	bool isTraversable() const;
	bool isMoveable() const;
	void setPosition(const Point& point);
};


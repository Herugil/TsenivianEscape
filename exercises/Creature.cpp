#include "Creature.h"
#include "GameObject.h"
#include "Point.h"
Creature::Creature(char symbol,const Point &position) : GameObject{ true, false, symbol , position }{}
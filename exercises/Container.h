#pragma once
#include <memory>
#include <vector>
#include "GameObject.h"
#include "Item.h"
#include "Point.h"

class Container :
    public GameObject
{
private:
    std::vector<std::unique_ptr<Item>> m_heldItems{};
public:
    Container(const Point& point);
    Container(std::vector<std::unique_ptr<Item>> items, const Point& point);
    void displayContents();
};


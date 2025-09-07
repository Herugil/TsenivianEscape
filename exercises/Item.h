#pragma once
#include <iostream>
#include <string>
class Item
{
	std::string m_name{};
public:
	Item(std::string_view name) : m_name{ name }{}
	friend std::ostream& operator<<(std::ostream& out, const Item& item) {
		out << item.m_name;
		return out;
	}
};


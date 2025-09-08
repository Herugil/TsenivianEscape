#pragma once
#include <vector>
// Layer class to manage both map layers and ease access with [] overload

template <typename PtrType>
class Layer
{
	int m_width{};
	int m_height{};
	std::vector<std::vector<PtrType>> m_grid;
public:
	explicit Layer(int width=10, int height=10);
	const PtrType& operator[] (int x, int y) const;
	PtrType& operator[] (int x, int y);
	int getWidth() const;
	int getHeight() const;
};


template <typename PtrType>
Layer<PtrType>::Layer(int width, int height) : m_width{ width }, m_height{ height },
m_grid(static_cast<std::size_t>(height)) {  
	for (auto& row : m_grid) {
		row.resize(static_cast<std::size_t>(width));
	}
}

template <typename PtrType>
const PtrType& Layer<PtrType>::operator[](int x, int y) const {
	return m_grid[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
}

template <typename PtrType>
PtrType& Layer<PtrType>::operator[](int x, int y) {
	return m_grid[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
}

template <typename PtrType>
int Layer< PtrType>::getWidth() const { return m_width; }

template <typename PtrType>
int Layer< PtrType>::getHeight() const { return m_height; }
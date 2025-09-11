#include <queue>

template <class T, class Container = std::vector<T>,
          class Compare = std::less<typename Container::value_type>>
class MyQueue : public std::priority_queue<T, Container, Compare> {
public:
  using std::priority_queue<T, Container, Compare>::priority_queue;
  bool contains(const T &value) const {
    return std::find(this->c.begin(), this->c.end(), value) != this->c.end();
  }
};

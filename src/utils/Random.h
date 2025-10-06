#pragma once

#include <chrono>
#include <optional>
#include <random>

namespace Random {
namespace {
inline std::unique_ptr<std::mt19937> mt_ptr;
}

inline void init() {
  std::random_device rd{};
  std::seed_seq ss{
      static_cast<std::seed_seq::result_type>(
          std::chrono::steady_clock::now().time_since_epoch().count()),
      rd(),
      rd(),
      rd(),
      rd(),
      rd(),
      rd(),
      rd()};
  mt_ptr = std::make_unique<std::mt19937>(ss);
}
inline void seed(uint32_t s) { mt_ptr = std::make_unique<std::mt19937>(s); }

inline std::mt19937 &mt() {
  if (!mt_ptr)
    init();
  return *mt_ptr;
}

inline int get(int min, int max) {
  return std::uniform_int_distribution{min, max}(mt());
}

template <typename T> inline T get(T min, T max) {
  return std::uniform_int_distribution<T>{min, max}(mt());
}

inline int rollD100() { return get(1, 100); }

} // namespace Random
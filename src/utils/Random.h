#pragma once

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4365) // signed/unsigned mismatch
#endif

#include <boost/random/uniform_int_distribution.hpp> // used for reproducible tests

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif
#include <chrono>
#include <random>

namespace Random {

class Generator {
private:
  std::mt19937 m_mt;

public:
  Generator() : m_mt(generateSeed()) {}
  explicit Generator(unsigned int seed) : m_mt(seed) {}
  int get(int min, int max) {
    return boost::random::uniform_int_distribution<int>{min, max}(m_mt);
  }
  template <typename T> T get(T min, T max) {
    return boost::random::uniform_int_distribution<T>{min, max}(m_mt);
  }
  int rollD100() { return get(1, 100); }
  void setSeed(unsigned int seed) { m_mt.seed(seed); }

private:
  static std::mt19937 generateSeed() {
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
    return std::mt19937{ss};
  }
};

inline Generator g_generator;
inline int get(int min, int max) { return g_generator.get(min, max); }
template <typename T> inline T get(T min, T max) {
  return g_generator.get<T>(min, max);
}

inline int rollD100() { return g_generator.rollD100(); }
inline void setSeed(unsigned int seed) { g_generator.setSeed(seed); }

} // namespace Random
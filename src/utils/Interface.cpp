#include "utils/Interface.h"
#include "utils/ScreenUtils.h"
#include <chrono>
#include <thread>

void Interface::displayCombatInterface(const Player &player) {
  std::cout << "Movement:";
  for (int i{0}; i < player.getMovementPoints(); ++i) {
    std::cout << 'X';
  }
  for (int i{0}; i < player.getMaxMovementPoints() - player.getMovementPoints();
       ++i)
    std::cout << ' ';
  std::cout << "   ";
  std::cout << "Actions:";
  for (int i{0}; i < player.getActionPoints(); ++i) {
    std::cout << 'X';
  }
  for (int i{0}; i < player.getMaxActionPoints() - player.getActionPoints();
       ++i)
    std::cout << ' ';
  std::cout << '\n'
            << "HP: " << player.getHealthPoints() << '/'
            << player.getMaxHealthPoints() << '\n';
}

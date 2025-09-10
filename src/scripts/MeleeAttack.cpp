#include "scripts/MeleeAttack.h"
#include "map/Point.h"
#include <memory>

MeleeAttack::MeleeAttack(std::string_view name) : Action(name, true) {}

void MeleeAttack::execute([[maybe_unused]] GameSession &gameSession,
                          Creature &actor, Creature &target) {
  if (actor.canAct(m_cost)) {
    int m_damage{actor.getMeleeDamage()};
    target.takeDamage(m_damage);
    std::cout << m_damage << "  damage dealt to " << target.getName() << " by "
              << actor.getName() << ".\n";
  }
}

void MeleeAttack::playerExecute(GameSession &gameSession,
                                Directions::Direction direction) {
  auto &player{gameSession.getPlayer()};
  int range{player.getMeleeRange()};
  auto &map{gameSession.getMap()};
  auto point{player.getPosition()};
  for (int i = 1; i <= range; ++i) {
    point = point.getAdjacentPoint(direction);

    auto topObject = map.getTopObject(point);
    if (auto creature = std::dynamic_pointer_cast<Creature>(topObject)) {
      execute(gameSession, player, *creature);
      break;
    }
    if (!map.isAvailable(point))
      return;
  }
  gameSession.cleanDeadNpcs();
}
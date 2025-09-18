#include "scripts/MeleeAttack.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include <memory>

MeleeAttack::MeleeAttack(std::string_view name) : Action(name, true, false) {}

std::ostringstream
MeleeAttack::execute([[maybe_unused]] GameSession &gameSession, Creature &actor,
                     Creature &target) const {
  std::ostringstream result;
  if (actor.useActionPoints()) {
    if (GeometryUtils::distanceL1(actor.getPosition(), target.getPosition()) >
        actor.getMeleeRange())
      return result;
    int m_damage{actor.getMeleeDamage()};
    target.takeDamage(m_damage);
    result << m_damage << "  damage dealt to " << target.getName() << " by "
           << actor.getName() << ".\n";
  }
  return result;
}

std::ostringstream
MeleeAttack::playerExecute(GameSession &gameSession,
                           Directions::Direction direction) const {
  std::ostringstream result;
  auto &player{gameSession.getPlayer()};
  int range{player.getMeleeRange()};
  auto &map{gameSession.getMap()};
  auto point{player.getPosition()};
  for (int i = 1; i <= range; ++i) {
    point = point.getAdjacentPoint(direction);

    auto topObject = map.getTopObject(point);
    if (auto creature = std::dynamic_pointer_cast<Creature>(topObject)) {
      result << execute(gameSession, player, *creature).str();
      break;
    }
    if (!map.isAvailable(point))
      break;
  }
  return result;
}
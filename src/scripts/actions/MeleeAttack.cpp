#include "scripts/actions/MeleeAttack.h"
#include "core/GameSession.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include "utils/Random.h"
#include <memory>

MeleeAttack::MeleeAttack(std::string_view name) : Action(name, true, true) {}

std::string MeleeAttack::execute([[maybe_unused]] GameSession &gameSession,
                                 Creature &actor, Creature &target) const {
  std::ostringstream result;
  if (actor.useActionPoints()) {
    if (GeometryUtils::distanceL2(actor.getPosition(), target.getPosition()) >
        actor.getMeleeRange())
      return result.str();
    if (Random::rollD100() > actor.getMeleeHitChance() - target.getEvasion()) {
      result << actor.getName() << " missed " << target.getName() << ".\n";
    } else {
      int m_damage{actor.getMeleeDamage()};
      target.takeDamage(m_damage);
      result << m_damage << "  damage dealt to " << target.getName() << " by "
             << actor.getName() << ".\n";
    }
  }
  return result.str();
}

std::string MeleeAttack::playerExecute(GameSession &gameSession,
                                       Creature &target) const {
  return execute(gameSession, gameSession.getPlayer(), target);
}

std::string MeleeAttack::playerExecute(GameSession &gameSession,
                                       Directions::Direction direction) const {
  std::string result;
  auto &player{gameSession.getPlayer()};
  int range{player.getMeleeRange()};
  auto &map{gameSession.getMap()};
  auto point{player.getPosition()};
  for (int i = 1; i <= range; ++i) {
    point = point.getAdjacentPoint(direction);

    auto topObject = map.getTopObject(point);
    if (auto creature = std::dynamic_pointer_cast<Creature>(topObject)) {
      result = execute(gameSession, player, *creature);
      break;
    }
    if (!map.isAvailable(point))
      break;
  }
  return result;
}
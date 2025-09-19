#include "scripts/RangedAttack.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include <memory>

RangedAttack::RangedAttack(std::string_view name) : Action(name, false, true) {}

std::string RangedAttack::execute([[maybe_unused]] GameSession &gameSession,
                                  Creature &actor, Creature &target) const {
  std::ostringstream result{};
  if (gameSession.getMap().isPointVisible(actor.getPosition(),
                                          target.getPosition()) == false) {
    result << "Target not visible.\n";
    return result.str();
  }
  if (actor.useActionPoints()) {
    if (GeometryUtils::distanceL2(actor.getPosition(), target.getPosition()) >
        actor.getDistanceRange()) {
      result << "Target was out of range, attack failed!\n";
      return result.str();
    }
    int m_damage{actor.getDistanceDamage()};
    target.takeDamage(m_damage);
    result << m_damage << "  damage dealt to " << target.getName() << " by "
           << actor.getName() << ".\n";
  }
  return result.str();
}

std::string RangedAttack::playerExecute(GameSession &gameSession,
                                        Creature &target) const {
  return execute(gameSession, gameSession.getPlayer(), target);
}
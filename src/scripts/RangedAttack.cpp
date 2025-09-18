#include "scripts/RangedAttack.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include <memory>

RangedAttack::RangedAttack(std::string_view name) : Action(name, false, true) {}

void RangedAttack::execute([[maybe_unused]] GameSession &gameSession,
                           Creature &actor, Creature &target) const {
  if (gameSession.getMap().isPointVisible(actor.getPosition(),
                                          target.getPosition()) == false) {
    std::cout << "Target not visible.\n";
    return;
  }
  if (actor.useActionPoints()) {
    if (GeometryUtils::distanceL2(actor.getPosition(), target.getPosition()) >
        actor.getDistanceRange()) {
      std::cout << "Target was out of range, attack failed!\n";
      return;
    }
    int m_damage{actor.getDistanceDamage()};
    target.takeDamage(m_damage);
    std::cout << m_damage << "  damage dealt to " << target.getName() << " by "
              << actor.getName() << ".\n";
  }
}

void RangedAttack::playerExecute(GameSession &gameSession,
                                 Creature &target) const {
  execute(gameSession, gameSession.getPlayer(), target);
}
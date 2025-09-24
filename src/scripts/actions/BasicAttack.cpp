#include "scripts/actions/BasicAttack.h"
#include "core/GameSession.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include "utils/Random.h"
#include <memory>

BasicAttack::BasicAttack(std::string_view name, Stat usedStat,
                         std::vector<std::unique_ptr<PassiveEffect>> applyOnHit)
    : Action(name, true, true, usedStat) {
  for (auto &effect : applyOnHit) {
    m_applyOnHit.emplace_back(effect->clone());
  }
}

std::string BasicAttack::execute([[maybe_unused]] GameSession &gameSession,
                                 Creature &actor, Creature &target) const {
  std::ostringstream result;
  int hitChance{0};
  int damage{0};
  int range{0};
  if (m_usedStat == Stat::Strength) {
    hitChance = actor.getMeleeHitChance();
    damage = actor.getMeleeDamage();
    range = actor.getMeleeRange();
  } else if (m_usedStat == Stat::Dexterity) {
    hitChance = actor.getDistanceHitChance();
    damage = actor.getDistanceDamage();
    range = actor.getDistanceRange();
  }
  if (actor.useActionPoints(m_cost)) {
    if (GeometryUtils::distanceL2(actor.getPosition(), target.getPosition()) >
        range)
      return result.str();
    if (gameSession.getMap().isPointVisible(actor.getPosition(),
                                            target.getPosition()) == false)
      result << actor.getName()
             << " cannot see the target and misses the attack.\n";
    else if (Random::rollD100() > hitChance - target.getEvasion()) {
      result << actor.getName() << " missed " << target.getName() << ".\n";
    } else {
      int m_damage{damage};
      target.takeDamage(m_damage);
      result << m_damage << "  damage dealt to " << target.getName() << " by "
             << actor.getName() << ".\n";
      for (const auto &effect : m_applyOnHit) {
        target.addPassiveEffect(*effect);
        result << target.getName() << " is affected by " << effect->getName()
               << " for " << effect->getRoundsLeft() << " rounds.\n";
      }
    }
  }
  return result.str();
}

std::string BasicAttack::playerExecute(GameSession &gameSession,
                                       Creature &target) const {
  return execute(gameSession, gameSession.getPlayer(), target);
}

std::string BasicAttack::playerExecute(GameSession &gameSession,
                                       Directions::Direction direction) const {
  int range{0};
  if (m_usedStat == Stat::Strength) {
    range = gameSession.getPlayer().getMeleeRange();
  } else if (m_usedStat == Stat::Dexterity) {
    range = gameSession.getPlayer().getDistanceRange();
  }
  std::string result;
  auto &player{gameSession.getPlayer()};
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
#include "scripts/actions/BasicAttack.h"
#include "core/GameSession.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include "utils/Random.h"
#include <memory>

using json = nlohmann::json;

BasicAttack::BasicAttack(std::string_view name, Stat usedStat,
                         std::vector<std::unique_ptr<PassiveEffect>> applyOnHit,
                         int cost, int maxCharges, int cooldown)
    : Action(name, true, true, usedStat, cost, maxCharges, cooldown) {
  for (auto &effect : applyOnHit) {
    m_applyOnHit.emplace_back(effect->clone());
  }
}

std::string BasicAttack::execute([[maybe_unused]] GameSession &gameSession,
                                 Creature &actor, Creature &target) {
  std::ostringstream result;
  if (useActionResources(actor)) {
    if (GeometryUtils::distanceL2(actor.getPosition(), target.getPosition()) >
        getRange(actor))
      return result.str();
    if (gameSession.getMap().isPointVisible(actor.getPosition(),
                                            target.getPosition()) == false)
      result << actor.getName()
             << " cannot see the target and misses the attack.\n";
    else if (Random::rollD100() > getHitChance(actor, target)) {
      result << actor.getName() << " missed " << target.getName() << ".\n";
    } else {
      int inflictedDamage{target.takeDamage(getDamage(actor))};
      result << inflictedDamage << "  damage dealt to " << target.getName()
             << " by " << actor.getName() << ".\n";
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
                                       Creature &target) {
  return execute(gameSession, gameSession.getPlayer(), target);
}

std::string BasicAttack::playerExecute(GameSession &gameSession,
                                       Directions::Direction direction) {
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

int BasicAttack::getRange(const Creature &actor) const {
  if (m_usedStat == Stat::Strength) {
    return actor.getMeleeRange();
  } else if (m_usedStat == Stat::Dexterity) {
    return actor.getDistanceRange();
  }
  return 0;
}

int BasicAttack::getDamage(const Creature &actor) const {
  if (m_usedStat == Stat::Strength) {
    return actor.getMeleeDamage();
  } else if (m_usedStat == Stat::Dexterity) {
    return actor.getDistanceDamage();
  }
  return 0;
}

int BasicAttack::getHitChance(const Creature &actor,
                              const Creature &target) const {
  int hitChance{0};
  if (m_usedStat == Stat::Strength) {
    hitChance = actor.getMeleeHitChance();
  } else if (m_usedStat == Stat::Dexterity) {
    hitChance = actor.getDistanceHitChance();
  }
  return hitChance - target.getEvasion();
}

json BasicAttack::toJson() const {
  json j;
  j["name"] = m_name;
  j["type"] = (m_usedStat == Stat::Strength) ? "melee" : "ranged";
  j["isBasicAttack"] = true;
  j["effects"] = json::array();
  for (const auto &effect : m_applyOnHit) {
    j["effects"].push_back(effect->toJson());
  }
  return j;
}
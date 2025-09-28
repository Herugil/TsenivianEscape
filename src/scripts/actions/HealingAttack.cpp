#include "scripts/actions/HealingAttack.h"
#include "core/GameSession.h"
#include "utils/GeometryUtils.h"
#include "utils/Random.h"

HealingAttack::HealingAttack(
    const std::string &name, int cost, int charges, int cooldown, Stat usedStat,
    std::function<int(const Creature &actor, const Creature &target)>
        healAmountFunc,
    std::vector<std::unique_ptr<PassiveEffect>> applyOnHit)
    : BasicAttack(name, usedStat, std::move(applyOnHit), cost, charges,
                  cooldown),
      m_healAmountFunc(std::move(healAmountFunc)) {
  m_maxCharges = charges;
  m_cooldown = cooldown;
  m_cost = cost;
}

std::string HealingAttack::execute(GameSession &gameSession, Creature &actor,
                                   Creature &target) {
  std::ostringstream result;
  if (GeometryUtils::distanceL2(actor.getPosition(), target.getPosition()) >
      getRange(actor))
    return result.str();
  if (gameSession.getMap().isPointVisible(actor.getPosition(),
                                          target.getPosition()) == false) {
    result << actor.getName()
           << " cannot see the target and misses the attack.\n";
    return result.str();
  }
  // same code as basic attack execute.. But since it returns a string there's
  // no nice way to know if it hit or not without parsing the string which is
  // also terrible
  if (useActionResources(actor)) {
    if (Random::rollD100() > getHitChance(actor, target)) {
      result << actor.getName() << " missed " << target.getName() << ".\n";
    } else {
      int inflictedDamage{target.takeDamage(getDamage(actor))};
      result << inflictedDamage << "  damage dealt to " << target.getName()
             << " by " << actor.getName() << ".\n";
      int healAmount{m_healAmountFunc(actor, target)};
      actor.addHealthPoints(healAmount);
      result << actor.getName() << " heals " << healAmount << " HP.\n";
      for (const auto &effect : m_applyOnHit) {
        target.addPassiveEffect(*effect);
        result << target.getName() << " is affected by " << effect->getName()
               << " for " << effect->getRoundsLeft() << " rounds.\n";
      }
    }
  }
  return result.str();
}

std::string HealingAttack::playerExecute(GameSession &gameSession,
                                         Creature &target) {
  return execute(gameSession, gameSession.getPlayer(), target);
}

int HealingAttack::getRange(const Creature &actor) const {
  if (m_usedStat == Stat::Strength) {
    return actor.getMeleeRange();
  } else if (m_usedStat == Stat::Dexterity) {
    return actor.getDistanceRange();
  }
  return 0;
}

int HealingAttack::getDamage(const Creature &actor) const {
  if (m_usedStat == Stat::Strength) {
    return actor.getMeleeDamage();
  } else if (m_usedStat == Stat::Dexterity) {
    return actor.getDistanceDamage();
  }
  return 0;
}

int HealingAttack::getHitChance(const Creature &actor,
                                const Creature &target) const {
  int hitChance{0};
  if (m_usedStat == Stat::Strength) {
    hitChance = actor.getMeleeHitChance();
  } else if (m_usedStat == Stat::Dexterity) {
    hitChance = actor.getDistanceHitChance();
  }
  return hitChance - target.getEvasion();
}

json HealingAttack::toJson() const {
  json j;
  j["name"] = m_name;
  j["currentCharges"] = m_currentCharges;
  return j;
}
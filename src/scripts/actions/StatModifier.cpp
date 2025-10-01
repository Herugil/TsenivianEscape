#include "scripts/actions/StatModifier.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "utils/GeometryUtils.h"

StatModifier::StatModifier(std::string_view name, PassiveEffect::Type type,
                           int value, int rounds, int cooldown, int cost,
                           int maxCharges, int range)
    : Action{name, false, true, Stat::nbStats, cost, maxCharges, cooldown},
      m_effect{type, value, rounds, std::string(name), name}, m_range{range} {
  if (value >= 0)
    m_targetType = TargetType::friendTarget;
  else
    m_targetType = TargetType::enemyTarget;
  switch (type) {
  case PassiveEffect::Type::ArmorBonus:
  case PassiveEffect::Type::ConstitutionBonus:
  case PassiveEffect::Type::EvasionBonus:
    if (value >= 0)
      m_types.push_back(ActionType::defenseBuff);
    else
      m_types.push_back(ActionType::defenseDebuff);
    break;
  case PassiveEffect::Type::DistanceDamageBonus:
  case PassiveEffect::Type::DistanceHitBonus:
  case PassiveEffect::Type::MeleeDamageBonus:
  case PassiveEffect::Type::MeleeHitBonus:
  case PassiveEffect::Type::StrengthBonus:
  case PassiveEffect::Type::DexterityBonus:
  case PassiveEffect::Type::IntelligenceBonus:
    if (value >= 0)
      m_types.push_back(ActionType::offenseBuff);
    else
      m_types.push_back(ActionType::offenseDebuff);
    break;
  default:
    if (value >= 0) {
      m_types.push_back(ActionType::offenseBuff);
      m_types.push_back(ActionType::defenseBuff);
    } else {
      m_types.push_back(ActionType::offenseDebuff);
      m_types.push_back(ActionType::defenseDebuff);
    }
    break;
  }
}

std::string StatModifier::execute(GameSession &gameSession, Creature &actor,
                                  Creature &target) {
  std::string result{};
  if (useActionResources(actor)) {
    if (GeometryUtils::distanceL2(actor.getPosition(), target.getPosition()) >
        getRange(actor))
      return result;
    if (gameSession.getMap().isPointVisible(actor.getPosition(),
                                            target.getPosition()) == false)
      result += actor.getName() + " cannot see the target.\n";
    target.addPassiveEffect(m_effect);
    result += target.getName() + " is affected by " + actor.getName() + "'s " +
              static_cast<std::string>(getName()) + ".\n";
  }
  return result;
}

std::string StatModifier::playerExecute(GameSession &gameSession,
                                        Creature &target) {
  return execute(gameSession, gameSession.getPlayer(), target);
}

int StatModifier::getRange([[maybe_unused]] const Creature &actor) const {
  return m_range;
}

json StatModifier::toJson() const {
  // this will only reconstruct the action using its name
  json j{};
  j["name"] = m_name;
  j["currentCharges"] = m_currentCharges;
  return j;
}
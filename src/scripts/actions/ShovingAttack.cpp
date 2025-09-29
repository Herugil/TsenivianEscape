#include "scripts/actions/ShovingAttack.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/Creature.h"
#include "utils/GeometryUtils.h"
#include "utils/Random.h"

using json = nlohmann::json;

ShovingAttack::ShovingAttack(
    std::string_view name,
    std::function<int(const Creature &actor, const Creature &target)>
        shoveDistanceFormula,
    std::function<int(const Creature &actor)> damageFormula,
    std::function<int(const Creature &actor, const Creature &target)>
        hitChanceFormula,
    std::function<int(const Creature &actor)> rangeFormula, int cost,
    int maxCharges, int cooldown)
    : Action{name, false, true, Stat::Strength, cost, maxCharges, cooldown},
      m_shoveDistanceFormula{shoveDistanceFormula},
      m_damageFormula{damageFormula}, m_hitChanceFormula{hitChanceFormula},
      m_rangeFormula{rangeFormula} {
  m_types.clear();
  m_types = {ActionType::attack, ActionType::control};
  m_targetType = enemyTarget;
}

std::string ShovingAttack::execute(GameSession &gameSession, Creature &actor,
                                   Creature &target) {
  std::string result;
  Directions::Direction shoveDirection{GeometryUtils::getRequiredDirection(
      actor.getPosition(), target.getPosition())};
  if ((GeometryUtils::distanceL1(actor.getPosition(), target.getPosition()) >
       m_rangeFormula(actor)) ||
      shoveDirection == Directions::nbDirections) {
    return {};
  }
  if (useActionResources(actor)) {
    if (gameSession.getMap().isPointVisible(actor.getPosition(),
                                            target.getPosition()) == false) {
      result = actor.getName() +
               " cannot see the target and misses the shoving attack.\n";
    } else if (Random::rollD100() >
               m_hitChanceFormula(actor, target) - target.getEvasion()) {
      result = actor.getName() + " missed " + target.getName() + ".\n";
    } else {
      result = std::to_string(target.takeDamage(getDamage(actor))) +
               " damage dealt to " + target.getName() + " by " +
               actor.getName() + ".\n";
      int shoveDistance{m_shoveDistanceFormula(actor, target)};
      for (int i = 0; i < shoveDistance; ++i) {
        auto creature{gameSession.getMap().getTopObject(target.getPosition())};
        if (!creature)
          break;
        gameSession.moveCreature(creature, shoveDirection, true);
      }
    }
  }
  return result;
}

std::string ShovingAttack::playerExecute(GameSession &gameSession,
                                         Creature &target) {
  return execute(gameSession, gameSession.getPlayer(), target);
}

std::unique_ptr<Action> ShovingAttack::clone() const {
  return std::make_unique<ShovingAttack>(*this);
}

int ShovingAttack::getRange(const Creature &actor) const {
  return m_rangeFormula(actor);
}
int ShovingAttack::getDamage(const Creature &actor) const {
  return m_damageFormula(actor);
}
int ShovingAttack::getHitChance(const Creature &actor,
                                const Creature &target) const {
  return m_hitChanceFormula(actor, target);
}

json ShovingAttack::toJson() const {
  json j;
  j["name"] = m_name;
  j["currentCharges"] = m_currentCharges;
  return j;
}
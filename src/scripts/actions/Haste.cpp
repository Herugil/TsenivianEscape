#include "scripts/actions/Haste.h"
#include "core/GameSession.h"
#include "utils/GeometryUtils.h"

Haste::Haste(int value, int cost, int maxCharges, int cooldown,
             std::string_view name)
    : Action{name, false, false, Stat::nbStats, cost, maxCharges, cooldown},
      m_value{value} {}

std::string Haste::execute(GameSession &gameSession, Creature &actor,
                           Creature &target) {

  if (!gameSession.getMap().isPointVisible(actor.getPosition(),
                                           target.getPosition()))
    return "Target not visible\n";
  if (useActionResources(actor)) {
    target.addActionPoints(m_value);
    return actor.getName() + " uses " + static_cast<std::string>(getName()) +
           " on " + target.getName() + ", granting " + std::to_string(m_value) +
           " action points.\n";
  }
  return "";
}

std::string Haste::playerExecute(GameSession &gameSession) {
  return execute(gameSession, gameSession.getPlayer(), gameSession.getPlayer());
}

std::unique_ptr<Action> Haste::clone() const {
  return std::make_unique<Haste>(*this);
}

json Haste::toJson() const {
  json j;
  j["name"] = m_name;
  j["currentCharges"] = m_currentCharges;
  return j;
}
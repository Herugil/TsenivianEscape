#include "scripts/actions/CubeAoe.h"
#include "core/GameSession.h"

CubeAoe::CubeAoe(std::string_view name, int radius,
                 std::function<int(const Creature &actor)> damageFormula,
                 int cost, int maxCharges, int cooldown)
    : Action(name, false, false, Stat::Intelligence, cost, maxCharges,
             cooldown),
      m_radius{radius}, m_damageFormula{damageFormula} {}

std::string CubeAoe::playerExecute(GameSession &gameSession) {
  return execute(gameSession, gameSession.getPlayer(), gameSession.getPlayer());
}

std::string CubeAoe::execute(GameSession &gameSession, Creature &actor,
                             [[maybe_unused]] Creature &target) {
  std::ostringstream result;
  if (useActionResources(actor)) {
    auto &map{gameSession.getMap()};
    auto center{actor.getPosition()};
    for (int dx = -m_radius; dx <= m_radius; ++dx) {
      for (int dy = -m_radius; dy <= m_radius; ++dy) {
        Point targetPoint{center.getX() + dx, center.getY() + dy};
        if (targetPoint == center)
          continue;
        if (map.isPointVisible(center, targetPoint)) {
          if (auto creature{std::dynamic_pointer_cast<Creature>(
                  map.getTopObject(targetPoint))}) {
            int inflictedDamage{creature->takeDamage(getDamage(actor))};
            result << inflictedDamage << " damage dealt to "
                   << creature->getName() << " by " << actor.getName() << "'s "
                   << m_name << ".\n";
          }
        }
      }
    }
  }
  return result.str();
}

int CubeAoe::getDamage(const Creature &actor) const {
  return m_damageFormula(actor);
}

json CubeAoe::toJson() const {
  json j;
  j["name"] = m_name;
  j["currentCharges"] = m_currentCharges;
  return j;
}
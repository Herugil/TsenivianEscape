#include "scripts/actions/CubeAoe.h"
#include "core/GameSession.h"

CubeAoe::CubeAoe(std::string_view name, int radius,
                 std::function<int(const Creature &actor)> damageFormula,
                 int cost, int maxCharges, int cooldown)
    : Action(name, false, false, Stat::Intelligence, cost, maxCharges,
             cooldown),
      m_radius{radius}, m_damageFormula{damageFormula} {}

std::string CubeAoe::playerExecute(GameSession &gameSession) {
  return execute(gameSession, gameSession.getPlayer());
}

std::string CubeAoe::execute(GameSession &gameSession, Creature &actor) {
  std::ostringstream result;
  if (useActionResources(actor)) {
    auto &map{gameSession.getMap()};
    auto center{actor.getPosition()};
    int damage{m_damageFormula(actor)};
    for (int dx = -m_radius; dx <= m_radius; ++dx) {
      for (int dy = -m_radius; dy <= m_radius; ++dy) {
        Point targetPoint{center.getX() + dx, center.getY() + dy};
        if (map.isPointVisible(center, targetPoint)) {
          if (auto creature{std::dynamic_pointer_cast<Creature>(
                  map.getTopObject(targetPoint))}) {
            if (creature.get() != &actor) {
              creature->takeDamage(damage);
              result << damage << " damage dealt to " << creature->getName()
                     << " by " << actor.getName() << "'s " << m_name << ".\n";
            }
          }
        }
      }
    }
  }
  return result.str();
}
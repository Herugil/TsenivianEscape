#include "scripts/actions/Dodge.h"
#include "core/GameSession.h"
#include "scripts/passives/PassiveEffect.h"
#include <sstream>

Dodge::Dodge(std::string_view name) : Action{name, false, false} {
  m_cooldown = 1;
  m_currentCooldown = 0;
}

std::string Dodge::execute([[maybe_unused]] GameSession &gameSession,
                           [[maybe_unused]] Creature &actor,
                           [[maybe_unused]] Creature &target) {
  return {};
}

std::string Dodge::execute(Creature &actor) {
  std::ostringstream res{};
  if (useActionResources(actor)) {
    actor.addPassiveEffect(PassiveEffect{PassiveEffect::Type::EvasionBonus, 10,
                                         2, "dodge", "dodge", false});
    res << actor.getName() << " starts dodging.\n";
    m_currentCooldown = m_cooldown;
  }
  return res.str();
}
std::string Dodge::playerExecute(GameSession &gameSession) {
  if (gameSession.getPlayer().inCombat())
    return execute(gameSession.getPlayer());
  return {};
}
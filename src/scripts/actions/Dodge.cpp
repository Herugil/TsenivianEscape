#include "scripts/actions/Dodge.h"
#include "core/GameSession.h"
#include "scripts/passives/PassiveEffect.h"
#include <sstream>

Dodge::Dodge(std::string_view name) : Action{name, false, false} {}

std::string Dodge::execute([[maybe_unused]] GameSession &gameSession,
                           [[maybe_unused]] Creature &actor,
                           [[maybe_unused]] Creature &target) const {
  return {};
}

std::string Dodge::execute(Creature &actor) const {
  std::ostringstream res{};
  if (actor.useActionPoints(m_cost)) {
    actor.addPassiveEffect(
        PassiveEffect{PassiveEffect::Type::EvasionBonus, 10, 2});
    res << actor.getName() << " starts dodging.\n";
  }
  return res.str();
}
std::string Dodge::playerExecute(GameSession &gameSession) const {
  if (gameSession.getPlayer().inCombat())
    return execute(gameSession.getPlayer());
  return {};
}
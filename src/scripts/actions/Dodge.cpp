#include "scripts/actions/Dodge.h"
#include "core/GameSession.h"
#include "scripts/passives/PassiveEffect.h"
#include <sstream>

using json = nlohmann::json;

Dodge::Dodge(std::string_view name) : Action{name, false, false} {
  m_cooldown = 1;
  m_currentCooldown = 0;
  m_types.push_back(ActionType::defenseBuff);
  m_targetType = selfTarget;
}

std::string Dodge::execute([[maybe_unused]] GameSession &gameSession,
                           [[maybe_unused]] Creature &actor,
                           [[maybe_unused]] Creature &target) {
  return execute(actor);
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

json Dodge::toJson() const {
  json j;
  j["name"] = m_name;
  return j;
}
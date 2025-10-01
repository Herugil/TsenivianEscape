
#include "scripts/actions/Action.h"
#include "gameObjects/creatures/Creature.h"

bool Action::useActionResources(Creature &creature) {
  if (m_currentCooldown > 0)
    return false;
  // action is on cooldown
  if (m_maxCharges != -1) {
    // action has limited charges
    if (m_currentCharges == 0)
      // no charges left
      return false;
    if (m_currentCooldown == 0 && creature.useActionPoints(m_cost)) {
      // enough action points and not on cooldown
      --m_currentCharges;             // use a charge
      m_currentCooldown = m_cooldown; // set cooldown
      return true;
    }
  } else if (m_currentCooldown == 0) {
    m_currentCooldown = m_cooldown;          // set cooldown
    return creature.useActionPoints(m_cost); // enough action points and not on
    // cooldown
  }
  return false;
}

bool Action::canBeUsed(const Creature &creature) const {
  // check if the action can be used by the creature
  // but does not use the resources/ set cooldown
  if (m_currentCooldown > 0)
    return false; // action is on cooldown
  if (m_maxCharges != -1) {
    // action has limited charges
    if (m_currentCharges == 0)
      return false;                 // no charges left
    return creature.canAct(m_cost); // enough action points and not on cooldown
  }
  return true;
}
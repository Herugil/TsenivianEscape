#include "scripts/passives/CompositePassiveEffect.h"
#include <memory>
#include <vector>

CompositePassiveEffect::CompositePassiveEffect(
    std::vector<std::unique_ptr<PassiveEffect>> effects, int rounds,
    std::string_view id, std::string_view name, bool stackable)
    : PassiveEffect(Type::Composite, 0, rounds, id, name, stackable),
      m_effects{std::move(effects)} {}

void CompositePassiveEffect::applyEffect(Creature &target) const {
  for (const auto &effect : m_effects) {
    effect->applyEffect(target);
  }
}

std::unique_ptr<PassiveEffect> CompositePassiveEffect::clone() const {
  std::vector<std::unique_ptr<PassiveEffect>> clonedEffects;
  for (const auto &effect : m_effects) {
    clonedEffects.emplace_back(effect->clone());
  }
  return std::make_unique<CompositePassiveEffect>(std::move(clonedEffects),
                                                  getRoundsLeft(), getId(),
                                                  getName(), isStackable());
}

int CompositePassiveEffect::getStatModifier(Stat stat) const {
  int totalModifier{0};
  for (const auto &effect : m_effects) {
    totalModifier += effect->getStatModifier(stat);
  }
  return totalModifier;
}

std::string CompositePassiveEffect::display() const {
  std::string result = getName() + " (";
  for (size_t i = 0; i < m_effects.size(); ++i) {
    result += m_effects[i]->display();
    if (i < m_effects.size() - 1)
      result += ", ";
  }
  result += ") - " + std::to_string(getRoundsLeft()) + " rounds left";
  return result;
}
#pragma once

#include "scripts/passives/PassiveEffect.h"
#include <memory>
#include <vector>

using json = nlohmann::json;

class CompositePassiveEffect : public PassiveEffect {
protected:
  std::vector<std::unique_ptr<PassiveEffect>> m_effects;

public:
  CompositePassiveEffect(std::vector<std::unique_ptr<PassiveEffect>> effects,
                         int rounds, std::string_view id, std::string_view name,
                         bool stackable = false);
  void applyEffect(Creature &target) const override;
  std::unique_ptr<PassiveEffect> clone() const override;
  int getStatModifier(Stat stat) const override;
  std::string display() const override;

  json toJson() const override;

  ~CompositePassiveEffect() override = default;
};
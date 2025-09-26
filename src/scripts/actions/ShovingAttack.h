#pragma once

#include "scripts/actions/Action.h"
#include <functional>

class ShovingAttack : public Action {
protected:
  std::function<int(const Creature &actor, const Creature &target)>
      m_shoveDistanceFormula;
  std::function<int(const Creature &actor)> m_damageFormula;
  std::function<int(const Creature &actor, const Creature &target)>
      m_hitChanceFormula;
  std::function<int(const Creature &actor)> m_rangeFormula;
  // these should be part of base class for the most part. Would allow
  // to display them in action menu

public:
  ShovingAttack(
      std::string_view name,
      std::function<int(const Creature &actor, const Creature &target)>
          shoveDistanceFormula,
      std::function<int(const Creature &actor)> damageFormula,
      std::function<int(const Creature &actor, const Creature &target)>
          hitChanceFormula,
      std::function<int(const Creature &actor)> rangeFormula, int cost = 1,
      int maxCharges = -1, int cooldown = 0);
  std::string execute(GameSession &gameSession, Creature &actor,
                      Creature &target) override;
  std::string playerExecute(GameSession &gameSession,
                            Creature &target) override;
  std::unique_ptr<Action> clone() const override;
  int getRange(Creature &actor) const override;
  int getDamage(Creature &actor) const override;
  int getHitChance(Creature &actor, Creature &target) const override;
  virtual ~ShovingAttack() = default;
};
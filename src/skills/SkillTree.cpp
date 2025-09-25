#include "skills/SkillTree.h"
#include "gameObjects/creatures/Player.h"
#include "scripts/actions/CubeAoe.h"
#include "scripts/actions/Haste.h"
#include "scripts/actions/HealingAttack.h"
#include "scripts/actions/ShovingAttack.h"

std::vector<std::unique_ptr<Action>>
SkillTree::getSkillsStatSpread(const Stats &stat) {
  std::vector<std::unique_ptr<Action>> actions;
  if (stat.strength == 2 && stat.dexterity == 1 && stat.intelligence == 1 &&
      stat.constitution == 1) {
    actions.emplace_back(std::make_unique<ShovingAttack>(
        "Backbreaker",
        [](const Creature &actor, const Creature &target) {
          return actor.getStrength() - target.getStrength();
        },
        [](const Creature &actor, const Creature &target) {
          return actor.getMeleeDamage() + actor.getStrength() -
                 target.getConstitution(); // placeholder will be armor later
        },
        [](const Creature &actor, const Creature &target) {
          return actor.getMeleeHitChance() - target.getEvasion();
        },
        [](const Creature &actor) { return actor.getStrength(); }, 1, 4, 1));
  } else if (stat.strength == 1 && stat.dexterity == 1 &&
             stat.intelligence == 2 && stat.constitution == 1) {
    actions.emplace_back(std::make_unique<CubeAoe>(
        "Swirling flames", 1,
        [](const Creature &actor) { return actor.getIntelligence() + 3; }, 2, 2,
        2));
  } else if (stat.strength == 1 && stat.dexterity == 2 &&
             stat.intelligence == 1 && stat.constitution == 1) {
    actions.emplace_back(std::make_unique<Haste>(1, 0, 2, 1, "Haste"));
  } else if (stat.strength == 1 && stat.dexterity == 1 &&
             stat.intelligence == 1 && stat.constitution == 2) {
    actions.emplace_back(std::make_unique<HealingAttack>(
        "Rejuvenating Strike", 1, 3, 1, Stat::Strength,
        [](const Creature &actor, [[maybe_unused]] const Creature &target) {
          return actor.getConstitution() + 2;
        },
        std::vector<std::unique_ptr<PassiveEffect>>{}));
  }
  return actions;
}

void SkillTree::addSkillStatSpread(Player &player) {
  auto actions{getSkillsStatSpread(player.getStats())};
  for (auto &action : actions) {
    player.addAction(std::move(action));
  }
}
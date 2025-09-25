#include "skills/SkillTree.h"
#include "gameObjects/creatures/Player.h"
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
  }
  return actions;
}

void SkillTree::addSkillStatSpread(Player &player) {
  auto actions{getSkillsStatSpread(player.getStats())};
  for (auto &action : actions) {
    player.addAction(std::move(action));
  }
}
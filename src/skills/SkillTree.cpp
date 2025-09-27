#include "skills/SkillTree.h"
#include "gameObjects/creatures/Player.h"
#include "scripts/actions/CubeAoe.h"
#include "scripts/actions/Haste.h"
#include "scripts/actions/HealingAttack.h"
#include "scripts/actions/ShovingAttack.h"

std::vector<std::unique_ptr<Action>>
SkillTree::getSkillsStatSpread(const Stats &stat) {
  std::vector<std::unique_ptr<Action>> actions;
  if (stat.strength >= 2)
    actions.push_back(createBackbreakerAction());
  if (stat.intelligence >= 2)
    actions.push_back(createSwirlingFlamesAction());
  if (stat.constitution >= 2)
    actions.push_back(createRejuvenatingStrikeAction());
  if (stat.dexterity >= 2)
    actions.push_back(createHasteAction());
  return actions;
}

void SkillTree::addSkillStatSpread(Player &player) {
  auto actions{getSkillsStatSpread(player.getStats())};
  for (auto &action : actions) {
    player.addAction(std::move(action));
  }
}

std::unique_ptr<Action> SkillTree::createBackbreakerAction() {
  return std::make_unique<ShovingAttack>(
      "Backbreaker",
      [](const Creature &actor, const Creature &target) {
        return actor.getStrength() - target.getStrength();
      },
      [](const Creature &actor) {
        return actor.getMeleeDamage() + actor.getStrength();
      },
      [](const Creature &actor, const Creature &target) {
        return actor.getMeleeHitChance() - target.getEvasion();
      },
      [](const Creature &actor) { return actor.getStrength(); }, 1, 4, 1);
}

std::unique_ptr<Action> SkillTree::createSwirlingFlamesAction() {
  return std::make_unique<CubeAoe>(
      "Swirling flames", 1,
      [](const Creature &actor) { return actor.getIntelligence() + 3; }, 2, 2,
      2);
}

std::unique_ptr<Action> SkillTree::createRejuvenatingStrikeAction() {
  return std::make_unique<HealingAttack>(
      "Rejuvenating Strike", 1, 3, 1, Stat::Strength,
      [](const Creature &actor, [[maybe_unused]] const Creature &target) {
        return actor.getConstitution() + 2;
      },
      std::vector<std::unique_ptr<PassiveEffect>>{});
}

std::unique_ptr<Action> SkillTree::createHasteAction() {
  return std::make_unique<Haste>(1, 0, 2, 1, "Haste");
}

std::unique_ptr<Action> SkillTree::createActionByName(std::string_view name) {
  if (name == "Backbreaker")
    return SkillTree::createBackbreakerAction();
  if (name == "Swirling flames")
    return SkillTree::createSwirlingFlamesAction();
  if (name == "Rejuvenating Strike")
    return SkillTree::createRejuvenatingStrikeAction();
  if (name == "Haste")
    return SkillTree::createHasteAction();
  return nullptr;
}
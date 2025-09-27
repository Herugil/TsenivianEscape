#pragma once
#include "gameObjects/creatures/Stats.h"
#include "scripts/actions/Action.h"
#include <memory>
#include <vector>

class Player;

namespace SkillTree {
void addSkillStatSpread(Player &player);
std::vector<std::unique_ptr<Action>> getSkillsStatSpread(const Stats &stat);
std::unique_ptr<Action> createBackbreakerAction();
std::unique_ptr<Action> createSwirlingFlamesAction();
std::unique_ptr<Action> createRejuvenatingStrikeAction();
std::unique_ptr<Action> createHasteAction();
std::unique_ptr<Action> createActionByName(std::string_view name);
} // namespace SkillTree
#pragma once
#include "gameObjects/creatures/Stats.h"
#include "scripts/actions/Action.h"
#include <memory>
#include <vector>

class Player;

namespace SkillTree {
void addSkillStatSpread(Player &player);
std::vector<std::unique_ptr<Action>> getSkillsStatSpread(const Stats &stat);
} // namespace SkillTree
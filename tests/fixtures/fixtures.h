#pragma once

#include "core/GameSession.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/creatures/Player.h"
#include "gameObjects/items/Armor.h"
#include "gameObjects/items/InstantUsableItem.h"
#include "skills/SkillTree.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(_MSC_VER)
#pragma warning(push, 0) // Disable all warnings (level 0)
#endif

#include <gtest/gtest.h>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

std::unique_ptr<NonPlayableCharacter> createTestNPC();
std::shared_ptr<Weapon> createTestWeapon();
std::shared_ptr<Armor> createTestArmor();
std::shared_ptr<InstantUsableItem> createTestHealthPotion();
std::shared_ptr<Weapon> createTestBow();
Player createTestPlayer();
GameSession createTestGameSession();

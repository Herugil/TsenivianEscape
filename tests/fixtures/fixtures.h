#pragma once

#include "core/GameSession.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/creatures/Player.h"
#include "gameObjects/items/Armor.h"
#include "gameObjects/items/InstantUsableItem.h"
#include "skills/SkillTree.h"
#include <gtest/gtest.h>

std::unique_ptr<NonPlayableCharacter> createTestNPC();
std::shared_ptr<Weapon> createTestWeapon();
std::shared_ptr<Armor> createTestArmor();
std::shared_ptr<InstantUsableItem> createTestHealthPotion();
std::shared_ptr<Weapon> createTestBow();
Player createTestPlayer();
GameSession createTestGameSession();

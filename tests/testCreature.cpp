#include "fixtures/fixtures.h"
#include <gtest/gtest.h>

class CreatureTest : public testing::Test {
protected:
  std::unique_ptr<NonPlayableCharacter> npc;
  void SetUp() override { npc = createTestNPC(); }
};

TEST_F(CreatureTest, getStat) {
  EXPECT_EQ(npc->getStrength(), 1);
  npc->addPassiveEffect(PassiveEffect(PassiveEffect::Type::StrengthBonus, 2, 3,
                                      "buff1", "Strength Buff", true));
  EXPECT_EQ(npc->getStrength(), 3);
  npc->addPassiveEffect(PassiveEffect(PassiveEffect::Type::StrengthBonus, 2, 3,
                                      "buff1", "Strength Buff", true));
  EXPECT_EQ(npc->getStrength(), 5);
  npc->addPassiveEffect(PassiveEffect(PassiveEffect::Type::ArmorBonus, -1, 3,
                                      "buff2", "Armor Debuff", false));
  EXPECT_EQ(npc->getArmor(), 0);
  npc->addPassiveEffect(PassiveEffect(PassiveEffect::Type::ArmorBonus, -1, 3,
                                      "buff2", "Armor Debuff", false));
  EXPECT_EQ(npc->getArmor(), 0);
}

TEST_F(CreatureTest, canAct) {
  EXPECT_TRUE(npc->canAct(10));
  npc->setCombat();
  EXPECT_FALSE(npc->canAct(10));
  EXPECT_TRUE(npc->useActionPoints(1));
  EXPECT_EQ(npc->getActionPoints(), 1);
  EXPECT_FALSE(npc->useActionPoints(2));
  EXPECT_EQ(npc->getActionPoints(), 1);
}

TEST_F(CreatureTest, canMove) {
  EXPECT_TRUE(npc->canMove(10));
  npc->setCombat();
  EXPECT_FALSE(npc->canMove(15));
  EXPECT_TRUE(npc->useMovementPoints(1));
  EXPECT_EQ(npc->getActionPoints(), 1);

  npc->useActionPoints();
  EXPECT_EQ(npc->getMovementPoints(), npc->getMaxMovementPoints() - 1);
  EXPECT_FALSE(npc->useMovementPoints(npc->getMaxMovementPoints()));
  EXPECT_EQ(npc->getActionPoints(), 0);
}

TEST_F(CreatureTest, takeDamage) {
  EXPECT_EQ(npc->getHealthPoints(), 10);
  npc->takeDamage(5);
  // armor of 1
  EXPECT_EQ(npc->getHealthPoints(), 6);
  npc->takeDamage(15);
  EXPECT_EQ(npc->getHealthPoints(), 0);
  EXPECT_TRUE(npc->isDead());
}
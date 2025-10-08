#include "fixtures/fixtures.h"
#include "scripts/NpcCombatAI.h"
#include "utils/Random.h"
#include <gtest/gtest.h>

class CombatTest : public testing::Test {
protected:
  std::unique_ptr<GameSession> gameSession;
  void SetUp() override {
    gameSession = std::make_unique<GameSession>(createTestGameSession());
    gameSession->addNpc(createTestNPC());
    Random::seed(42);
  }
};

TEST_F(CombatTest, CombatTurns) {
  ASSERT_TRUE(gameSession->enemiesInMap());
  gameSession->initializeTurnOrder();
  ASSERT_EQ(gameSession->getCurrentTurn(), 1);
  ASSERT_EQ(gameSession->getTurnOrder().size(), 2);
  ASSERT_EQ(gameSession->getActiveCreature().lock(),
            gameSession->getPlayerPtr());
  gameSession->incrementTurnIndex();
  ASSERT_EQ(gameSession->getActiveCreature().lock(),
            gameSession->getTurnOrder()[1].lock());
  gameSession->incrementTurnIndex();
  ASSERT_EQ(gameSession->getActiveCreature().lock(),
            gameSession->getPlayerPtr());
  ASSERT_EQ(gameSession->getCurrentTurn(), 2);
}

TEST_F(CombatTest, MultipleNpcCombatTurns) {
  gameSession->addNpc(createTestNPC());
  ASSERT_TRUE(gameSession->enemiesInMap());
  gameSession->initializeTurnOrder();
  ASSERT_EQ(gameSession->getCurrentTurn(), 1);
  ASSERT_EQ(gameSession->getTurnOrder().size(), 3);
  ASSERT_EQ(gameSession->getActiveCreature().lock(),
            gameSession->getPlayerPtr());
  gameSession->incrementTurnIndex();
  ASSERT_EQ(gameSession->getActiveCreature().lock(),
            gameSession->getTurnOrder()[1].lock());
  gameSession->incrementTurnIndex();
  ASSERT_EQ(gameSession->getActiveCreature().lock(),
            gameSession->getTurnOrder()[2].lock());
  ASSERT_EQ(gameSession->getCurrentTurn(), 1);
  gameSession->incrementTurnIndex();
  ASSERT_EQ(gameSession->getActiveCreature().lock(),
            gameSession->getPlayerPtr());
  ASSERT_EQ(gameSession->getCurrentTurn(), 2);
}

TEST_F(CombatTest, NoNpcCombatTurns) {
  gameSession->initializeTurnOrder();
  gameSession->getNpcs()[0]->takeDamage(100);
  gameSession->cleanDeadNpcs();
  ASSERT_FALSE(gameSession->enemiesInMap());
  ASSERT_EQ(gameSession->getCurrentTurn(), 1);
  ASSERT_EQ(gameSession->getTurnOrder().size(), 1);
  ASSERT_EQ(gameSession->getActiveCreature().lock(),
            gameSession->getPlayerPtr());
  gameSession->incrementTurnIndex();
  ASSERT_EQ(gameSession->getActiveCreature().lock(),
            gameSession->getPlayerPtr());
  ASSERT_EQ(gameSession->getCurrentTurn(), 2);
}

TEST_F(CombatTest, BaseEnemyActions) {
  gameSession->initializeTurnOrder();
  gameSession->incrementTurnIndex();
  auto activeCreature{gameSession->getActiveCreature().lock()};
  ASSERT_NE(activeCreature, nullptr);
  auto npc{std::dynamic_pointer_cast<NonPlayableCharacter>(activeCreature)};
  ASSERT_NE(npc, nullptr);
  NpcCombatAI::npcActCombat(*gameSession, npc);
  ASSERT_EQ(npc->getCurrentAction()->getName(), "Precise Strikes");
}
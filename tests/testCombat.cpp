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

// TEST_F(CombatTest, BaseEnemyActions) {
//   std::cerr << "Test starting\n" << std::flush;
//   gameSession->initializeTurnOrder();
//   std::cerr << "Turn order initialized\n" << std::flush;
//   gameSession->incrementTurnIndex();
//   std::cerr << "Turn index incremented\n" << std::flush;
//   auto activeCreature{gameSession->getActiveCreature().lock()};
//   std::cerr << "Got active creature\n" << std::flush;
//   ASSERT_NE(activeCreature, nullptr);
//   std::cerr << "Active creature not null\n";
//   auto npc{std::dynamic_pointer_cast<NonPlayableCharacter>(activeCreature)};
//   ASSERT_NE(npc, nullptr);
//   std::cerr << "NPC not null\n" << std::flush;
//   auto actions{npc->getUsableActionFromType(Action::offenseBuff)};
//   for (const auto &action : actions) {
//     std::cerr << "NPC action: " << action->getName() << "\n";
//   }
//   auto actions2{npc->getUsableActionFromType(Action::defenseBuff)};
//   for (const auto &action : actions2) {
//     std::cerr << "NPC actiondefense: " << action->getName() << "\n";
//   }
//   NpcCombatAI::npcActCombat(*gameSession, npc);
//   std::cerr << "NPC acted\n" << std::flush;
//   ASSERT_EQ(npc->getCurrentAction()->getName(), "Precise Strikes");
// }
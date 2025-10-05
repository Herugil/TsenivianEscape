#include "fixtures.h"

std::unique_ptr<NonPlayableCharacter> createTestNPC() {
  std::vector<std::unique_ptr<Action>> actions{};
  actions.emplace_back(SkillTree::createActionByName("Dodge"));
  actions.emplace_back(SkillTree::createActionByName("Precise Attacks"));
  std::vector<std::shared_ptr<Item>> inventory{
      std::make_shared<Item>("item1", "id", "A simple item"),
  };
  return std::make_unique<NonPlayableCharacter>(
      "npcTest1", 'E', Point{1, 1}, "testMap", 10, "Johnny", 15, 50, 60, 2, 1,
      std::move(actions), std::move(inventory), "a test npc",
      "Johnny, but dead", "support", 12, Stats{1, 2, 1, 1}, 1);
}

std::shared_ptr<Weapon> createTestWeapon() {
  return std::make_shared<Weapon>(
      "sword", "testSword", "oneHanded", "melee",
      "a very powerful sword you'll only get in tests", 3, 1);
}

std::shared_ptr<Armor> createTestArmor() {
  return std::make_shared<Armor>("steel chestplate", "testArmor", "chestArmor",
                                 "Heavy steel chestplate", 2);
}

std::shared_ptr<InstantUsableItem> createTestHealthPotion() {
  return std::make_shared<InstantUsableItem>("health potion", "healthPotion",
                                             InstantUsableItem::healthRegen, 10,
                                             1, false, 2);
}

std::shared_ptr<Weapon> createTestBow() {
  return std::make_shared<Weapon>(
      "bow", "testBow", "twoHanded", "ranged",
      "a very powerful bow you'll only get in tests", 2, 10);
}

Player createTestPlayer() {
  std::vector<std::unique_ptr<Action>> actions{};
  return Player(Point(5, 5), "testMap", 10, "Hero", Stats{1, 1, 1, 1});
}
GameSession createTestGameSession() {
  auto player{createTestPlayer()};
  GameSession session{std::make_shared<Player>(player)};
  Map testMap{"testMap", 10, 10};
  session.addMap(std::move(testMap));
  session.setCurrentMap("testMap");
  session.getPlayer().setCurrentMap("testMap");
  return session;
}

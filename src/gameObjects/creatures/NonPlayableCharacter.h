#pragma once
#include "Settings.h"
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/items/Item.h"
#include "map/Point.h"
#include <nlohmann/json.hpp>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

using json = nlohmann::json;

class GameSession;

class NonPlayableCharacter : public Creature {
public:
  enum Behaviors {
    basicAttack,
    attack,
    selfHeal,
    defenseBuff,
    offenseBuff,
    skipTurn,
    flee,
    defaultBehavior,
  };
  enum AITypes {
    aggressiveMelee,
    waryMelee,
    aggressiveRanged,
    boss,
    defaultAI,
  };

protected:
  std::string m_id{};
  std::string m_deadDescription{};
  int m_meleeHitChance{};
  int m_distanceHitChance{};
  int m_meleeRange{Settings::g_baseEnemyRange};
  int m_meleeDamage{Settings::g_baseEnemyDamage};
  int m_distanceRange{Settings::g_baseEnemyDistanceRange};
  int m_distanceDamage{Settings::g_baseEnemyDistanceDamage};
  std::deque<Point> m_currentPath{};
  Behaviors m_currentBehavior{defaultBehavior};
  AITypes m_AIType{defaultAI};
  int m_xpValue{}; // xp given to player on kill
  int m_armor{0};
  bool m_hasActed{false};
  Action *m_currentAction{nullptr};

public:
  NonPlayableCharacter(std::string_view id, char symbol, const Point &point,
                       std::string_view currentMap, int maxHealthPoints,
                       std::string_view name, int evasion = 0,
                       int meleeHitChance = Settings::g_baseHitChance,
                       int distanceHitChance = Settings::g_baseHitChance,
                       int meleeDamage = Settings::g_baseEnemyDamage,
                       int distanceDamage = Settings::g_baseEnemyDamage,
                       std::vector<std::unique_ptr<Action>> &&actions = {},
                       std::vector<std::shared_ptr<Item>> &&inventory = {},
                       std::string_view description = "enemy",
                       std::string_view deadDescription = "",
                       std::string_view aiType = "defaultAI", int xpValue = 50,
                       Stats stats = Stats{}, int armor = 0);
  NonPlayableCharacter(const NonPlayableCharacter &other);
  std::shared_ptr<NonPlayableCharacter> clone() const;
  std::string_view getDeadDescription() const;
  void addItemToInventory(std::shared_ptr<Item> item);
  std::string executeBasicAttack(Creature &target, GameSession &gameSession);
  int getMeleeHitChance() const override;
  int getDistanceHitChance() const override;
  int getMeleeDamage() const override;
  int getMeleeRange() const override;
  int getDistanceDamage() const override;
  int getDistanceRange() const override;
  Behaviors getCurrentBehavior() const;
  std::string setCurrentBehavior(GameSession &gameSession);
  void setSkipTurn();
  void setDefaultBehavior();
  std::deque<Point> &getCurrentPath();
  void setCurrentPath(GameSession &gameSession);
  std::deque<Point> getPathFlee(GameSession &gameSession);
  std::deque<Point> getPathAttack(GameSession &gameSession);
  void clearCurrentPath() { m_currentPath.clear(); }
  static AITypes stringToAIType(std::string_view str);
  AITypes getAIType() const { return m_AIType; }
  int getXpValue() const;
  int getArmor() const override;
  int getStrength() const override;
  int getDexterity() const override;
  int getIntelligence() const override;
  int getConstitution() const override;
  int getBasicActionRange() const;
  bool hasActed() const { return m_hasActed; }
  void setHasActed() { m_hasActed = true; }
  void resetHasActed() { m_hasActed = false; }
  Action *getBasicAction() const;
  std::vector<Action *> getUsableActionFromType(Action::ActionType type) const;
  Behaviors setFighterBossBehavior(GameSession &gameSession);
  Action *determineCurrentAction(Action::ActionType type) const;
  void setCurrentAction(Action *action);
  Action *getCurrentAction() const;

  json toJson() const override;

  void updateFromJson(
      const json &j,
      const std::unordered_map<std::string, std::shared_ptr<Item>> &items,
      std::string_view mapToPlace);

  virtual ~NonPlayableCharacter() = default;
};
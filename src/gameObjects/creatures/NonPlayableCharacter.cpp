#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "AISettings.h"
#include "core/GameSession.h"
#include "dataLoading/parseJson.h"
#include "gameObjects/creatures/Creature.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/items/InstantUsableItem.h"
#include "gameObjects/items/Item.h"
#include "gameObjects/terrain/Container.h"
#include "gameObjects/terrain/MapChanger.h"
#include "input/Directions.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include "utils/Random.h"
#include <algorithm>
#include <memory>
#include <nlohmann/json.hpp>
#include <queue>
#include <thread>
#include <vector>

NonPlayableCharacter::NonPlayableCharacter(
    std::string_view id, char symbol, const Point &point,
    std::string_view currentMap, int maxHealthPoints, std::string_view name,
    int evasion, int meleeHitChance, int distanceHitChance, int meleeDamage,
    int distanceDamage, std::vector<std::unique_ptr<Action>> &&actions,
    std::vector<std::shared_ptr<Item>> &&items, std::string_view description,
    std::string_view deadDescription, std::string_view aiType, int xpValue,
    Stats stats, int armor)
    : Creature{symbol,  point, currentMap, maxHealthPoints,
               evasion, stats, name,       description},
      m_id{id}, m_deadDescription{deadDescription},
      m_meleeHitChance{meleeHitChance}, m_distanceHitChance{distanceHitChance},
      m_meleeDamage{meleeDamage}, m_distanceDamage{distanceDamage},
      m_AIType{stringToAIType(aiType)}, m_xpValue{xpValue}, m_armor{armor} {
  if (!items.empty())
    m_inventory = std::move(items);
  if (!actions.empty()) {
    m_actions.clear();
    m_actions.reserve(actions.size());
    for (auto &action : actions) {
      m_actions.emplace_back(std::move(action));
    }
  }
}

NonPlayableCharacter::NonPlayableCharacter(const NonPlayableCharacter &other)
    : Creature(other), m_id{other.m_id},
      m_deadDescription(other.m_deadDescription),
      m_meleeHitChance(other.m_meleeHitChance),
      m_distanceHitChance(other.m_distanceHitChance),
      m_meleeRange(other.m_meleeRange), m_meleeDamage(other.m_meleeDamage),
      m_distanceRange(other.m_distanceRange),
      m_distanceDamage(other.m_distanceDamage),
      m_currentPath(other.m_currentPath),
      m_currentBehavior(other.m_currentBehavior), m_AIType(other.m_AIType),
      m_xpValue{other.m_xpValue}, m_armor{other.m_armor} {}

std::shared_ptr<NonPlayableCharacter> NonPlayableCharacter::clone() const {
  return std::make_shared<NonPlayableCharacter>(*this);
}

std::string_view NonPlayableCharacter::getDeadDescription() const {
  return m_deadDescription;
}

void NonPlayableCharacter::addItemToInventory(std::shared_ptr<Item> item) {
  m_inventory.push_back(std::move(item));
}

// for the damage/getter functions below, not using stat modifiers for now
// considering using only specific stat modifiers such as melee hit chance
// modifier, but not strength modifier
// will depend on balancing probably
int NonPlayableCharacter::getMeleeHitChance() const { return m_meleeHitChance; }
int NonPlayableCharacter::getDistanceHitChance() const {
  return m_distanceHitChance;
}
int NonPlayableCharacter::getMeleeDamage() const { return m_meleeDamage; }
int NonPlayableCharacter::getMeleeRange() const { return m_meleeRange; }
int NonPlayableCharacter::getDistanceDamage() const { return m_distanceDamage; }
int NonPlayableCharacter::getDistanceRange() const { return m_distanceRange; }
int NonPlayableCharacter::getArmor() const {
  return m_armor + getStatModifier(Stat::Armor);
}
int NonPlayableCharacter::getStrength() const {
  return m_stats.strength + getStatModifier(Stat::Strength);
}
int NonPlayableCharacter::getDexterity() const {
  return m_stats.dexterity + getStatModifier(Stat::Dexterity);
}
int NonPlayableCharacter::getIntelligence() const {
  return m_stats.intelligence + getStatModifier(Stat::Intelligence);
}
int NonPlayableCharacter::getConstitution() const {
  return m_stats.constitution + getStatModifier(Stat::Constitution);
}

std::string NonPlayableCharacter::executeBasicAttack(Creature &target,
                                                     GameSession &gameSession) {
  auto basicAction{getBasicAction()};
  return basicAction->execute(gameSession, *this, target);
}

int NonPlayableCharacter::getBasicActionRange() const {
  return getBasicAction()->getRange(*this);
}

std::vector<Action *>
NonPlayableCharacter::getUsableActionFromType(Action::ActionType type) const {
  std::vector<Action *> availableActions{};
  for (auto &action : m_actions) {
    if (action->canBeUsed(*this) && action->isType(type))
      availableActions.push_back(action.get());
  }
  return availableActions;
}

Action *NonPlayableCharacter::getBasicAction() const {
  for (const auto &action : m_actions) {
    if (action->isType(Action::defaultAttack)) {
      return action.get();
    }
  }
  return nullptr;
}

std::string NonPlayableCharacter::setCurrentBehavior(
    [[maybe_unused]] GameSession &gameSession) {
  std::ostringstream res{};
  if (getActionPoints() == 0 && getMovementPoints() == 0) {
    m_currentBehavior = skipTurn;
    m_hasActed = false;
    return res.str();
  }
  switch (m_AIType) {
  case aggressiveMelee:
  case aggressiveRanged:
    m_currentBehavior = basicAttack;
    m_currentTarget = &gameSession.getPlayer();
    break;
  case waryMelee:
    if (m_healthPoints <=
        m_maxHealthPoints * AISettings::g_waryMeleeFleeHealthPercent / 100) {
      if (Random::rollD100() < AISettings::g_waryMeleeFleeChance) {
        m_currentBehavior = flee;
        res << getName() << " starts fleeing!\n";
      } else {
        m_currentBehavior = basicAttack;
        m_currentTarget = &gameSession.getPlayer();
      }
    } else {
      m_currentBehavior = basicAttack;
      m_currentTarget = &gameSession.getPlayer();
    }
    break;
  case boss:
    m_currentBehavior = setFighterBossBehavior(gameSession);
    break;
  case support:
    m_currentBehavior = setSupportBehavior(gameSession);
    break;
  default:
    m_currentBehavior = defaultBehavior;
  }
  return res.str();
}

Action *NonPlayableCharacter::determineCurrentAction(Action::ActionType type,
                                                     GameSession &gameSession) {
  auto availableActions{getUsableActionFromType(type)};
  auto r{Random::get<std::size_t>(0, availableActions.size() - 1)};
  if (!availableActions.empty()) {
    m_currentAction = availableActions[r];
    m_currentTarget = pickTargetForAction(gameSession, m_currentAction);
    if (m_currentTarget)
      return m_currentAction;
  }
  return nullptr;
}

Action *NonPlayableCharacter::getCurrentAction() const {
  return m_currentAction;
}

Creature *NonPlayableCharacter::pickTargetForAction(GameSession &gameSession,
                                                    Action *action) {
  if (action->getTargetType() == Action::selfTarget)
    return this;
  if (action->getTargetType() == Action::enemyTarget)
    return &gameSession.getPlayer();
  if (action->getTargetType() == Action::friendTarget) {
    if (this->getAIType() == NonPlayableCharacter::boss)
      // for now bosses are considered "lone fighter" types
      return this;
    auto npcList{gameSession.getEnemiesInMap()};
    return npcList[Random::get<std::size_t>(0, npcList.size())].lock().get();
    // This might return a non accessible npc. Ideally, a function should be
    // called here to check if a npc can be accessed considering the action
    // range and visibility constraints. For now, buffs will have a large range
  }
  if (action->getTargetType() == Action::aoe &&
      (action->isType(Action::defenseBuff) ||
       action->isType(Action::offenseBuff))) {
    auto npcList{gameSession.getEnemiesInMap()};
    return npcList[Random::get<std::size_t>(0, npcList.size())].lock().get();
  }
  if (action->getTargetType() == Action::aoe)
    return &gameSession.getPlayer();
  return nullptr;
}

Creature *NonPlayableCharacter::getCurrentTarget() const {
  return m_currentTarget;
}

NonPlayableCharacter::Behaviors
NonPlayableCharacter::setFighterBossBehavior(GameSession &gameSession) {
  Action *availableAction{};
  if (m_healthPoints <= m_maxHealthPoints / 2) {
    availableAction = determineCurrentAction(Action::selfHeal, gameSession);
    if (availableAction)
      return selfHeal;
  }
  if (gameSession.getCurrentTurn() <= 1) {
    // this should be checking for current usable buffs, current passives
    // on the creature, etc...
    availableAction = determineCurrentAction(Action::defenseBuff, gameSession);
    if (availableAction) {
      return defenseBuff;
    }
    availableAction = determineCurrentAction(Action::offenseBuff, gameSession);
    if (availableAction) {
      return offenseBuff;
    }
  }
  availableAction = determineCurrentAction(Action::attack, gameSession);
  if (availableAction) {
    return attack;
  }
  m_currentTarget = &gameSession.getPlayer();
  return basicAttack; // no action found
}

NonPlayableCharacter::Behaviors
NonPlayableCharacter::setSupportBehavior(GameSession &gameSession) {
  Action *availableAction{};
  if (Random::rollD100() < 50) {
    availableAction = determineCurrentAction(Action::defenseBuff, gameSession);
    if (availableAction) {
      return defenseBuff;
    }
    availableAction = determineCurrentAction(Action::offenseBuff, gameSession);
    if (availableAction) {
      return offenseBuff;
    }
  } else {
    availableAction = determineCurrentAction(Action::offenseBuff, gameSession);
    if (availableAction)
      return selfHeal;
    availableAction = determineCurrentAction(Action::defenseBuff, gameSession);
    if (availableAction)
      return defenseBuff;
  }
  return basicAttack;
}

NonPlayableCharacter::Behaviors
NonPlayableCharacter::getCurrentBehavior() const {
  return m_currentBehavior;
}
void NonPlayableCharacter::setSkipTurn() {
  m_currentBehavior = skipTurn;
  m_hasActed = false;
}
void NonPlayableCharacter::setDefaultBehavior() {
  m_currentBehavior = defaultBehavior;
}
std::deque<Point> &NonPlayableCharacter::getCurrentPath() {
  return m_currentPath;
}

void NonPlayableCharacter::setCurrentPath(GameSession &gameSession,
                                          const Creature &target) {
  switch (m_currentBehavior) {
  case basicAttack:
  case attack:
  case selfHeal: // this depends on the target (self heal)
                 // can be an attack targetting the player for ex
                 // this function needs a target parameter probably
    m_currentPath = getPathToTarget(gameSession, target);
    return;
  case flee:
    m_currentPath = getPathFlee(gameSession);
    return;
  default: // actor has no goal, so he doesnt really need to move..
    m_currentPath.clear(); // will remain in place
  }
}

std::deque<Point>
NonPlayableCharacter::getPathFlee(GameSession &gameSession) const {
  std::vector<Point> possiblePoints{};
  std::vector<Point> safePoints{};
  std::vector<Point> mapChangers{};
  std::deque<Point> path{};
  for (int i{0}; i < gameSession.getMap().getWidth(); ++i) {
    for (int j{0}; j < gameSession.getMap().getHeight(); ++j) {
      Point potentialDestination{i, j};
      if (!gameSession.getMap().isAvailable(potentialDestination))
        continue; // cant go there
      if (GeometryUtils::distanceL2(potentialDestination,
                                    gameSession.getPlayerPos()) <=
          GeometryUtils::distanceL2(getPosition(), gameSession.getPlayerPos()))
        continue; // would get closer to player
      if (gameSession.getMap().getFloorObject(potentialDestination)) {
        if (auto{dynamic_cast<MapChanger *>(
                gameSession.getMap().getFloorObject(potentialDestination))})
          mapChangers.push_back(potentialDestination);
        else
          continue;
      }
      safePoints.push_back(potentialDestination);
    }
  }
  std::sort(mapChangers.begin(), mapChangers.end(),
            [this](const Point &p1, const Point &p2) {
              return (GeometryUtils::distanceL2(getPosition(), p1) >=
                      GeometryUtils::distanceL2(getPosition(), p2));
            });
  path = GeometryUtils::sortPointsAndFindPath(mapChangers, getPosition(),
                                              gameSession, getPosition(), true);
  // this leads to the closest map changer
  if (!path.empty())
    return path;
  else
    return GeometryUtils::sortPointsAndFindPath(
        safePoints, getPosition(), gameSession, gameSession.getPlayerPos(),
        false);
}

std::deque<Point>
NonPlayableCharacter::getPathToTarget(GameSession &gameSession,
                                      const Creature &target) const {
  std::vector<Point> possiblePoints{};
  for (int i{0}; i < gameSession.getMap().getWidth(); ++i) {
    for (int j{0}; j < gameSession.getMap().getHeight(); ++j) {
      Point potentialDestination{i, j};
      if (!gameSession.getMap().isAvailable(potentialDestination))
        continue; // cant go there
      if (GeometryUtils::distanceL2(potentialDestination,
                                    target.getPosition()) >=
          GeometryUtils::distanceL2(getPosition(), target.getPosition()))
        continue; // would get farther to target
      possiblePoints.push_back(potentialDestination);
    }
  }
  return GeometryUtils::sortPointsAndFindPath(
      possiblePoints, getPosition(), gameSession, target.getPosition());
  return {};
}

NonPlayableCharacter::AITypes
NonPlayableCharacter::stringToAIType(std::string_view str) {
  if (str == "aggressiveMelee")
    return aggressiveMelee;
  else if (str == "waryMelee")
    return waryMelee;
  else if (str == "aggressiveRanged")
    return aggressiveRanged;
  else if (str == "boss")
    return boss;
  else if (str == "support")
    return support;
  return defaultAI;
}

int NonPlayableCharacter::getXpValue() const { return m_xpValue; }

json NonPlayableCharacter::toJson() const {
  json j;
  j["id"] = m_id;
  j["healthPoints"] = m_healthPoints;
  j["inventory"] = json::array();
  j["currentMap"] = m_currentMap;
  j["symbol"] = std::string(1, m_symbol);
  j["position"] = json::array({m_position.getX(), m_position.getY()});
  for (const auto &item : m_inventory) {
    j["inventory"].push_back(item->toJson());
  }
  for (const auto &action : m_actions) {
    if (action->getMaxCharges() != -1) {
      j["actions"].push_back({{"name", action->getName()},
                              {"currentCharges", action->getCurrentCharges()}});
    }
  }
  return j;
}

void NonPlayableCharacter::updateFromJson(
    const json &j,
    const std::unordered_map<std::string, std::shared_ptr<Item>> &items,
    std::string_view mapToPlace) {
  char symbol{std::string(j["symbol"])[0]};
  Point pos{j["position"][0], j["position"][1]};
  if (j.contains("inventory"))
    for (auto itemJson : j["inventory"]) {
      auto item{DataLoader::parseItem(itemJson, items)};
      if (item)
        addItemToInventory(item);
    }
  setSymbol(symbol);
  setPosition(pos);
  setCurrentMap(mapToPlace);
  if (j.contains("healthPoints"))
    m_healthPoints = j["healthPoints"];
  if (j.contains("actions")) {
    for (auto &actionJ : j["actions"]) {
      auto action{DataLoader::parseAction(actionJ)};
      if (action)
        m_actions.push_back(std::move(action));
    }
  }
}
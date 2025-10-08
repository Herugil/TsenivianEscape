#include "scripts/NpcCombatAI.h"
#include "AISettings.h"
#include "core/GameSession.h"
#include "core/UserInterface.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/terrain/MapChanger.h"
#include "input/Directions.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include "utils/Random.h"
#include "utils/ScreenUtils.h"
#include <algorithm>
#include <memory>
#include <queue>
#include <sstream>

std::string
NpcCombatAI::npcActCombat(GameSession &gameSession,
                          std::shared_ptr<NonPlayableCharacter> actor) {
  std::ostringstream res;
  Creature *target{};
  if (!(actor->hasActed())) {
    res << actor->setCurrentBehavior(gameSession);
    target = actor->getCurrentTarget();
    if (!target)
      target = gameSession.getPlayerPtr().get();
    actor->setCurrentPath(gameSession, *target);
    actor->setHasActed();
  }
  target = actor->getCurrentTarget();
  if (!target)
    target = gameSession.getPlayerPtr().get();
  std::vector<Action *> availableActions{};
  switch (actor->getCurrentBehavior()) {
  case NonPlayableCharacter::basicAttack:
    if (checkTargetAvailable(gameSession, actor, *target,
                             actor->getBasicAction())) {
      res << tryCreatureAction(gameSession, actor, *target,
                               actor->getBasicAction());
    } else {
      if (!actor->getCurrentPath().empty())
        res << tryCreatureMove(gameSession, actor);
      else
        // no path to player, so just wait
        actor->setSkipTurn();
    }
    return res.str();

  case NonPlayableCharacter::attack:
  case NonPlayableCharacter::selfHeal:
  case NonPlayableCharacter::offenseBuff:
  case NonPlayableCharacter::defenseBuff:
    return useAction(gameSession, actor);

  case NonPlayableCharacter::flee:
    if (!actor->getCurrentPath().empty()) {
      res << tryCreatureMove(gameSession, actor);
      return res.str();
    } else {
      // no more path to flee, so just wait
      actor->setSkipTurn();
      return res.str();
    }

  default:
    if (actor->getActionPoints() > 0)
      res << actor->getName() << " doesn't know what to do!\n";
    actor->setSkipTurn();
    break;
  }
  return res.str();
}

std::string
NpcCombatAI::tryCreatureMove(GameSession &gameSession,
                             std::shared_ptr<NonPlayableCharacter> actor) {
  std::string res;
  Point nextPoint{actor->getCurrentPath().front()};
  Directions::Direction direction{
      GeometryUtils::getRequiredDirection(actor->getPosition(), nextPoint)};
  int costNextPoint{1}; // will be cost to get to nextPoint
  if (actor->canMove(costNextPoint)) {
    gameSession.moveCreature(actor, direction);
    actor->getCurrentPath().pop_front(); // remove current point from path
    if (actor->getCurrentBehavior() != NonPlayableCharacter::flee &&
        actor->getCurrentBehavior() != NonPlayableCharacter::basicAttack &&
        (!actor->getCurrentAction() ||
         !actor->canAct(actor->getCurrentAction()->getCost())))
      actor->resetHasActed();
  } else {
    actor->resetHasActed(); // this should make npc skip turn
  }
  return res;
}

std::string
NpcCombatAI::tryCreatureAction(GameSession &gameSession,
                               std::shared_ptr<NonPlayableCharacter> actor,
                               Creature &target, Action *action) {
  std::string res;
  if (!actor->canAct()) {
    actor->setSkipTurn();
  } else {
    res += action->execute(gameSession, *actor, target);
    actor->resetHasActed();
  }
  return res;
}

bool NpcCombatAI::checkTargetAvailable(
    GameSession &gameSession, std::shared_ptr<NonPlayableCharacter> actor,
    Creature &target, Action *action) {
  return (
      GeometryUtils::distanceL2(actor->getPosition(), target.getPosition()) <=
          action->getRange(*actor) &&
      gameSession.getMap().isPointVisible(actor->getPosition(),
                                          target.getPosition()));
}

std::string
NpcCombatAI::useAction(GameSession &gameSession,
                       std::shared_ptr<NonPlayableCharacter> actor) {
  std::string res;
  auto *selectedAction{actor->getCurrentAction()};
  if (!selectedAction) {
    actor->setSkipTurn();
    return res;
  }
  auto target{actor->getCurrentTarget()};
  if (checkTargetAvailable(gameSession, actor, *target, selectedAction)) {
    res = tryCreatureAction(gameSession, actor, *target, selectedAction);
  } else if (!actor->getCurrentPath().empty()) {
    res = tryCreatureMove(gameSession, actor);
  } else
    actor->setSkipTurn();
  return res;
}

std::deque<Point> NpcCombatAI::getPathFlee(GameSession &gameSession,
                                           NonPlayableCharacter &actor) {
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
          GeometryUtils::distanceL2(actor.getPosition(),
                                    gameSession.getPlayerPos()))
        continue; // would get closer to player
      if (gameSession.getMap().getFloorObject(potentialDestination)) {
        if (dynamic_cast<MapChanger *>(
                gameSession.getMap().getFloorObject(potentialDestination)))
          mapChangers.push_back(potentialDestination);
        else
          continue;
      }
      safePoints.push_back(potentialDestination);
    }
  }
  std::sort(mapChangers.begin(), mapChangers.end(),
            [&actor](const Point &p1, const Point &p2) {
              return (GeometryUtils::distanceL2(actor.getPosition(), p1) >=
                      GeometryUtils::distanceL2(actor.getPosition(), p2));
            });
  path = GeometryUtils::sortPointsAndFindPath(
      mapChangers, actor.getPosition(), gameSession, actor.getPosition(), true);
  // this leads to the closest map changer
  if (!path.empty())
    return path;
  else
    return GeometryUtils::sortPointsAndFindPath(
        safePoints, actor.getPosition(), gameSession,
        gameSession.getPlayerPos(), false);
}

std::deque<Point> NpcCombatAI::getPathToTarget(GameSession &gameSession,
                                               const Creature &actor,
                                               const Creature &target) {
  std::vector<Point> possiblePoints{};
  for (int i{0}; i < gameSession.getMap().getWidth(); ++i) {
    for (int j{0}; j < gameSession.getMap().getHeight(); ++j) {
      Point potentialDestination{i, j};
      if (!gameSession.getMap().isAvailable(potentialDestination))
        continue; // cant go there
      if (GeometryUtils::distanceL2(potentialDestination,
                                    target.getPosition()) >=
          GeometryUtils::distanceL2(actor.getPosition(), target.getPosition()))
        continue; // would get farther to target
      possiblePoints.push_back(potentialDestination);
    }
  }
  return GeometryUtils::sortPointsAndFindPath(
      possiblePoints, actor.getPosition(), gameSession, target.getPosition());
}

Creature *NpcCombatAI::pickTargetForAction(GameSession &gameSession,
                                           const NonPlayableCharacter &actor,
                                           Action *action) {
  if (action->getTargetType() == Action::selfTarget)
    return const_cast<NonPlayableCharacter *>(&actor);
  if (action->getTargetType() == Action::enemyTarget)
    return &gameSession.getPlayer();
  if (action->getTargetType() == Action::friendTarget) {
    if (actor.getAIType() == NonPlayableCharacter::boss)
      // for now bosses are considered "lone fighter" types
      return const_cast<NonPlayableCharacter *>(&actor);
    auto npcList{gameSession.getEnemiesInMap()};
    Creature *bestTarget = nullptr;
    int currChance{0};
    std::vector<int> cumBuffChances{};
    std::vector<NonPlayableCharacter *> candidates{};
    for (auto &weakNpc : npcList) {
      auto npc = weakNpc.lock().get();
      if (!npc)
        continue;
      int chance = npc->getChanceToBuff();
      if (npc == &actor)
        chance -= AISettings::g_selfBuffLikelihoodPenalty;
      if (gameSession.getMap().isPointVisible(npc->getPosition(),
                                              actor.getPosition()) == false) {
        chance -= AISettings::g_nonVisibleTargetPenalty;
      }
      cumBuffChances.emplace_back(currChance += chance);
      candidates.push_back(npc);
    }
    int roll{Random::get(0, currChance - 1)};
    for (std::size_t i{0}; i < cumBuffChances.size(); ++i) {
      if (roll < cumBuffChances[i]) {
        bestTarget = candidates[i];
        break;
      }
    }
    return bestTarget;
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
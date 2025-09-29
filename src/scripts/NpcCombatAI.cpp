#include "scripts/NpcCombatAI.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "input/Directions.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include "utils/Interface.h"
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
  auto &player{gameSession.getPlayer()};
  if (!(actor->hasActed())) {
    res << actor->setCurrentBehavior(gameSession); // thisll need to check if
    // any available target before returning a behavior
    actor->setCurrentPath(gameSession); // thisll soon need to take target param
    actor->setHasActed();
  }
  std::vector<Action *> availableActions{};
  switch (actor->getCurrentBehavior()) {
  case NonPlayableCharacter::basicAttack:
    if (checkTargetAvailable(gameSession, actor, player,
                             actor->getBasicAction())) {
      res << tryCreatureAction(gameSession, actor, gameSession.getPlayer(),
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
        !actor->canAct(actor->getCurrentAction()->getCost()))
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

Creature *NpcCombatAI::getTarget(GameSession &gameSession,
                                 std::shared_ptr<NonPlayableCharacter> actor,
                                 Action *action) {
  if (action->getTargetType() == Action::selfTarget)
    return actor.get();
  if (action->getTargetType() == Action::enemyTarget)
    return &gameSession.getPlayer();
  if (action->getTargetType() == Action::friendTarget) {
    if (actor->getAIType() == NonPlayableCharacter::boss)
      // for now bosses are considered "lone fighter" types
      return actor.get();
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

std::string
NpcCombatAI::useAction(GameSession &gameSession,
                       std::shared_ptr<NonPlayableCharacter> actor) {
  std::string res;
  auto *selectedAction{actor->getCurrentAction()};
  if (!selectedAction) {
    actor->setSkipTurn();
    return res;
  }
  auto target = getTarget(gameSession, actor, selectedAction);
  if (checkTargetAvailable(gameSession, actor, *target, selectedAction)) {
    res = tryCreatureAction(gameSession, actor, *target, selectedAction);
  } else if (!actor->getCurrentPath().empty()) {
    res = tryCreatureMove(gameSession, actor);
  } else
    actor->setSkipTurn();
  return res;
}
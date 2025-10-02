#include "scripts/NpcCombatAI.h"
#include "core/GameSession.h"
#include "core/UserInterface.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
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
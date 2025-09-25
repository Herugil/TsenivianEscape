#include "scripts/NpcCombatAI.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "input/Directions.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include "utils/Interface.h"
#include "utils/ScreenUtils.h"
#include <algorithm>
#include <memory>
#include <queue>
#include <sstream>

std::string
NpcCombatAI::npcActCombat(GameSession &gameSession,
                          std::shared_ptr<NonPlayableCharacter> actor) {
  std::ostringstream res;
  if (actor->getActionPoints() >= actor->getMaxActionPoints()) {
    // this is equivalent to starting a new turn
    res << actor->setCurrentBehavior(gameSession);
    actor->setCurrentPath(gameSession);
  }
  int range{};
  if (actor->getAIType() == NonPlayableCharacter::aggressiveMelee)
    range = actor->getMeleeRange();
  else if (actor->getAIType() == NonPlayableCharacter::aggressiveRanged)
    range = actor->getDistanceRange();
  else
    range = actor->getMeleeRange();
  switch (actor->getCurrentBehavior()) {
  case NonPlayableCharacter::basicAttack:
    if (GeometryUtils::distanceL2(actor->getPosition(),
                                  gameSession.getPlayerPos()) <= range &&
        gameSession.getMap().isPointVisible(actor->getPosition(),
                                            gameSession.getPlayerPos())) {
      if (!actor->canAct()) {
        actor->setSkipTurn();
        return res.str();
      }
      res << actor->executeBasicAttack(gameSession.getPlayer(), gameSession);
      res << actor->setCurrentBehavior(gameSession);
      actor->setCurrentPath(gameSession);
      // after attack, recalculate behavior and path
      break;
    } else {
      if (!actor->getCurrentPath().empty()) {
        Point nextPoint{actor->getCurrentPath().front()};
        Directions::Direction direction{GeometryUtils::getRequiredDirection(
            actor->getPosition(), nextPoint)};
        int costNextPoint{1}; // will be cost to get to nextPoint
        if (actor->canMove(costNextPoint)) {
          gameSession.moveCreature(actor, direction);
          actor->getCurrentPath().pop_front(); // remove current point from path
        } else {
          res << actor->setCurrentBehavior(gameSession);
          actor->setCurrentPath(gameSession);
          return res.str();
        }
        break;
      }
    }
  case NonPlayableCharacter::flee:
    // the next part is so not dry, need to refactor
    if (!actor->getCurrentPath().empty()) {
      Point nextPoint{actor->getCurrentPath().front()};
      Directions::Direction direction{
          GeometryUtils::getRequiredDirection(actor->getPosition(), nextPoint)};
      int costNextPoint{1}; // will be cost to get to nextPoint
      if (actor->canMove(costNextPoint)) {
        gameSession.moveCreature(actor, direction);
        actor->getCurrentPath().pop_front(); // remove current point from path
      } else {
        res << actor->setCurrentBehavior(gameSession);
        actor->setCurrentPath(gameSession);
        return res.str();
      }
      break;
    } else {
      // no more path to flee, so just wait
      actor->setSkipTurn();
      return res.str();
    }
  default:
    res << actor->getName() << " doesn't know what to do!\n";
    actor->setSkipTurn();
    break;
  }
  return res.str();
}

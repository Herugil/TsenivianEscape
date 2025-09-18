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

std::ostringstream
NpcCombatAI::npcActCombat(GameSession &gameSession,
                          std::shared_ptr<NonPlayableCharacter> actor) {
  std::ostringstream res;
  if (actor->getActionPoints() >= actor->getMaxActionPoints()) {
    // this is equivalent to starting a new turn
    actor->setCurrentBehavior(gameSession);
    actor->setCurrentPath(gameSession);
  }
  switch (actor->getCurrentBehavior()) {
  case NonPlayableCharacter::basicAttack:
    if (GeometryUtils::distanceL2(actor->getPosition(),
                                  gameSession.getPlayerPos()) <=
        actor->getMeleeRange()) {
      if (!actor->canAct()) {
        actor->setSkipTurn();
        return res;
      }
      res << actor->executeBasicAttack(gameSession.getPlayer(), gameSession)
                 .str();
      actor->clearCurrentPath();
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
          actor->setCurrentBehavior(gameSession);
          return res;
        }
        break;
      }
    }
  default:
    res << actor->getName() << " doesn't know what to do!\n";
    actor->setSkipTurn();
    break;
  }
  return res;
}

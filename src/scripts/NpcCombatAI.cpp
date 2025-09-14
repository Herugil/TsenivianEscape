#include "scripts/NpcCombatAI.h"
#include "core/GameSession.h"
#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "input/Directions.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include "utils/Interface.h"
#include "utils/ScreenUtils.h"
#include <algorithm>
#include <chrono>
#include <memory>
#include <queue>
#include <thread>

std::deque<Point>
NpcCombatAI::getDestination(Behaviors currentGoal,
                            const GameSession &gameSession,
                            const NonPlayableCharacter &actor) {
  std::vector<Point> possiblePoints{};
  switch (currentGoal) {
  case basicAttack:
    if (GeometryUtils::distanceL1(actor.getPosition(),
                                  gameSession.getPlayerPos()) <=
        actor.getMeleeRange())
      return {};
    // enemy is already in range, no movement needed
    for (int i{0};
         static_cast<Directions::Direction>(i) < Directions::nbDirections;
         ++i) {
      Point potentialDestination{gameSession.getPlayerPos().getAdjacentPoint(
          static_cast<Directions::Direction>(i))};
      if (!gameSession.getMap().isAvailable(potentialDestination))
        continue; // cant go there
      possiblePoints.push_back(potentialDestination);
    }
    std::sort(possiblePoints.begin(), possiblePoints.end(),
              [actor](const Point &p1, const Point &p2) {
                return GeometryUtils::distanceL1(actor.getPosition(), p1) <=
                       GeometryUtils::distanceL1(actor.getPosition(), p2);
              });
    for (auto possiblePoint : possiblePoints) {
      std::deque<Point> res{
          gameSession.getMap().findPath(actor.getPosition(), possiblePoint)};
      if (!res.empty()) {
        res.pop_front(); // this is the current point! unneeded
        return res;
      }
    } // found a path, return it, this means AI will prefer moving
      // top,
      // bot, left and right in that order to attack

  default:     // actor has no goal, so he doesnt really need to move..
    return {}; // will remain in place
  }
}

void NpcCombatAI::npcActCombat(GameSession &gameSession,
                               std::shared_ptr<NonPlayableCharacter> actor) {
  Behaviors currentGoal{Behaviors::basicAttack};
  Interface::timeAndDisplayInterface(gameSession, *actor);
  while (true) {
    // this loop is in case the npc changes behavior after his first action
    // eg it could heal itself then decide to go hit something
    if (actor->getActionPoints() <= 0) {
      // if no ap before changing behaviour, reset ap and pass
      actor->resetTurn();
      return;
    }
    // will need to update currentGoal with getCurrentGoal
    // function based on game state
    // for now all enemies do is basic attack or fail state
    std::deque<Point> newDestinationPath{
        getDestination(currentGoal, gameSession, *actor)};

    if (currentGoal == Behaviors::basicAttack) {
      // first, try to hit target if in range
      while (true) {
        if (actor->isDead())
          return; // can die during its own turn, need to avoid refilling AP
        // so return instead of break
        if ((GeometryUtils::distanceL1(actor->getPosition(),
                                       gameSession.getPlayerPos()) <=
             actor->getMeleeRange())) {
          Interface::timeAndDisplayInterface(gameSession, *actor,
                                             Settings::g_timeEnemyActionMS);

          if (actor->getActionPoints() <= 0) {
            break;
          }
          // no AP anymore, skips turn because already in range (this should
          // maybe check against Action.m_cost but considering getCurrentGoal
          // will check that, this may or may not be necessary
          actor->executeBasicAttack(gameSession.getPlayer(), gameSession);
        } else if (!newDestinationPath.empty()) {
          // moves toward destination
          Point nextPoint{newDestinationPath.front()};
          Directions::Direction direction{GeometryUtils::getRequiredDirection(
              actor->getPosition(), nextPoint)};
          int costNextPoint{1}; // will be cost to get to nextPoint
          if (actor->canMove(costNextPoint)) {
            gameSession.moveCreature(actor, direction);
            newDestinationPath.pop_front(); // remove current point from path
            Interface::timeAndDisplayInterface(gameSession, *actor);
          } else
            break; // npc isnt in range and cant move, early return
        } else {
          currentGoal = Behaviors::nbBehaviors;
          break; // npc isnt in range, and
                 // it cant act either (either cause no available path or no
                 // AP) so we set fail state
        }
      }
    } else if (currentGoal == Behaviors::nbBehaviors) {
      std::cout << actor->getName() << " doesn't know what to do!\n";
      Interface::timeAndDisplayInterface(gameSession, *actor,
                                         Settings::g_timeEnemyActionMS);
      break; // undefined behavior
    }
  }
}

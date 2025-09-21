#include "scripts/actions/Shove.h"
#include "core/GameSession.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include <memory>

Shove::Shove(std::string_view name)
    : Action(name, true, false, Stat::Strength) {}

std::string Shove::execute([[maybe_unused]] GameSession &gameSession,
                           Creature &actor,
                           std::shared_ptr<GameObject> target) const {
  std::ostringstream result;
  Directions::Direction shoveDirection{GeometryUtils::getRequiredDirection(
      actor.getPosition(), target->getPosition())};
  if ((GeometryUtils::distanceL1(actor.getPosition(), target->getPosition()) >
       1) || // shove has range of 1. magic number, to put somewhere else...
      (shoveDirection == Directions::nbDirections)) {
    return result.str();
  }
  if (actor.useActionPoints()) {
    if (target->isMoveable()) {
      gameSession.moveCreature(target, shoveDirection, true);
      result << target->getName() << " shoved by " << actor.getName() << ".\n";
    }
  }
  return result.str();
}

std::string Shove::playerExecute(GameSession &gameSession,
                                 Directions::Direction direction) const {
  std::ostringstream result;
  if (direction == Directions::nbDirections)
    return result.str();
  auto &player{gameSession.getPlayer()};
  auto &map{gameSession.getMap()};
  auto point{player.getPosition()};
  Point targetPoint{point.getAdjacentPoint(direction)};
  auto topObject = map.getTopObject(targetPoint);
  if (!topObject)
    return result.str();
  if (auto cont{std::dynamic_pointer_cast<Container>(topObject)}) {
    if (cont->getContents().empty() &&
        (!gameSession.getMap().isAvailable(
            targetPoint.getAdjacentPoint(direction))) &&
        gameSession.getPlayer().useActionPoints(1)) {
      // edge case: container is empty and destination shove is blocked
      gameSession.removeContainer(cont);
      return result.str();
    } else if (!cont->getContents().empty() &&
               (!gameSession.getMap().isAvailable(
                   targetPoint.getAdjacentPoint(direction)))) {
      result << "This container isn't empty! It will get destroyed if you "
                "shove it. Please empty it before shoving it.\n";
      return result.str();
    }
  }
  return execute(gameSession, player, topObject);
}

std::string Shove::execute([[maybe_unused]] GameSession &gameSession,
                           [[maybe_unused]] Creature &actor,
                           [[maybe_unused]] Creature &target) const {
  return {};
}

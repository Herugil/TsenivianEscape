#include "scripts/actions/Shove.h"
#include "core/GameSession.h"
#include "map/Point.h"
#include "utils/GeometryUtils.h"
#include <memory>

using json = nlohmann::json;

Shove::Shove(std::string_view name)
    : Action(name, true, false, Stat::Strength) {
  m_types.push_back(ActionType::control);
  // this is probably terrible for npcs since it
  // pushes the the target out of range
  m_targetType = enemyTarget;
}

std::string Shove::execute([[maybe_unused]] GameSession &gameSession,
                           Creature &actor,
                           std::shared_ptr<GameObject> target) {
  std::ostringstream result;
  Directions::Direction shoveDirection{GeometryUtils::getRequiredDirection(
      actor.getPosition(), target->getPosition())};
  if ((GeometryUtils::distanceL1(actor.getPosition(), target->getPosition()) >
       1) || // shove has range of 1. magic number, to put somewhere else...
      (shoveDirection == Directions::nbDirections)) {
    return result.str();
  }
  if (useActionResources(actor)) {
    if (target->isMoveable()) {
      if (auto creatureTarget = std::dynamic_pointer_cast<Creature>(target)) {
        if (creatureTarget->getStrength() > actor.getStrength()) {
          result << actor.getName() << " is not strong enough to shove "
                 << target->getName() << ".\n";
          return result.str();
        }
      }
      gameSession.moveCreature(target, shoveDirection, true);
      result << target->getName() << " shoved by " << actor.getName() << ".\n";
    }
  }
  return result.str();
}

std::string Shove::playerExecute(GameSession &gameSession,
                                 Directions::Direction direction) {
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
                           [[maybe_unused]] Creature &target) {
  return {};
}

json Shove::toJson() const {
  json j;
  j["name"] = m_name;
  return j;
}
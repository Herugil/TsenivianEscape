#include "core/GameSession.h"
#include "core/MenuSystems.h"
#include "utils/ScreenUtils.h"

MenuSystems::ItemInspectResult
MenuSystems::inspectItem(GameSession &gameSession, std::weak_ptr<Item> item) {
  ScreenUtils::clearScreen();
  GameState returnState{GameState::Inventory};
  if (item.expired()) {
    item.reset();
    return {"", GameState::Inventory};
  }
  std::string result;
  auto inspectedItem{item.lock()};
  std::cout << inspectedItem->getDisplayItem() << "\n";
  if (auto equipment = std::dynamic_pointer_cast<Equipment>(inspectedItem))
    std::cout << "E: Equip/Unequip   R: Drop\n";
  else if (auto usable = std::dynamic_pointer_cast<UsableItem>(inspectedItem))
    std::cout << "E: Use   R: Drop\n";
  else
    std::cout << "R: Drop\n";
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  if (CommandHandler::isInteractionCommand(command)) {
    auto &player{gameSession.getPlayer()};
    if (auto equipment = std::dynamic_pointer_cast<Equipment>(inspectedItem))
      player.equipItem(equipment);
    else if (auto usable =
                 std::dynamic_pointer_cast<UsableItem>(inspectedItem)) {
      result += player.useItem(usable) + gameSession.cleanDeadNpcs();
      if (!result.empty()) {
        returnState = GameState::Display;
      }
    }
  } else if (CommandHandler::isShoveCommand(command)) {
    auto &player{gameSession.getPlayer()};
    if (gameSession.dropItem(inspectedItem, player.getPosition()))
      player.removeItem(inspectedItem);
    else {
      result += "Could not drop item! No space around you.\n";
      returnState = GameState::Display;
    }
  }
  inspectedItem.reset();
  return {result, returnState};
}

std::string MenuSystems::actionMenu(GameSession &gameSession) {
  std::string result;
  Player &player{gameSession.getPlayer()};
  player.displayActions();
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  if (CommandHandler::isHotkeyCommand(command)) {
    auto pressedKey{
        static_cast<std::size_t>(CommandHandler::getPressedKey(command))};
    auto action{player.getAction(pressedKey)};
    if (!action)
      return "";
    if (!action->canBeUsed(player))
      result += "You cannot use this action right now.\n";
    else if (action->needsHotkeyInput()) {
      gameSession.displayEnemiesInMap([player, action](const Creature &c) {
        return action->getHitChance(player, c);
      });
      auto hotkeyCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
      if (CommandHandler::isHotkeyCommand(hotkeyCommand)) {
        auto pressedKey{static_cast<std::size_t>(
            CommandHandler::getPressedKey(hotkeyCommand))};
        if (pressedKey >= gameSession.getEnemiesInMap().size())
          return "";
        auto targetCreature{gameSession.getEnemiesInMap()[pressedKey].lock()};
        if (targetCreature) {
          result += action->playerExecute(gameSession, *targetCreature);
        }
      }
    } else if ((action->needsDirectionalInput())) {
      auto directionCommand{
          CommandHandler::getCommand(Input::getKeyBlocking())};
      if (CommandHandler::isMovementCommand(directionCommand)) {
        auto log{action->playerExecute(
            gameSession, static_cast<Directions::Direction>(directionCommand))};
        result += log;
      }
    } else {
      auto log{action->playerExecute(gameSession)};
      result += log;
    }
  }
  return result += gameSession.cleanDeadNpcs();
}

GameState MenuSystems::containerMenu(GameSession &gameSession,
                                     GameObject *container) {
  ScreenUtils::clearScreen();
  if (!container)
    return GameState::Exploration;
  GameState returnState{GameState::Container};
  bool tookItems{false};
  if (auto containerPtr = dynamic_cast<Container *>(container)) {
    auto &player{gameSession.getPlayer()};
    std::cout << containerPtr->getDescription() << "\n";
    containerPtr->displayContents();
    auto containerCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (CommandHandler::isTakeAllCommand(containerCommand)) {
      player.takeAllItems(*containerPtr);
      tookItems = true;
    } else if (CommandHandler::isHotkeyCommand(containerCommand)) {
      auto pressedKey{static_cast<std::size_t>(
          CommandHandler::getPressedKey(containerCommand))};
      auto item{containerPtr->popItem(pressedKey)};
      if (item) {
        player.takeItem(item);
        containerPtr->displayContents();
        tookItems = true;
      }
    } else
      returnState = GameState::Exploration;
    if (tookItems && containerPtr->getContents().empty()) {
      if (containerPtr->getName() == "Left items bag") {
        auto containers = gameSession.getSessionOwnedContainers();
        auto it =
            std::find_if(containers.begin(), containers.end(),
                         [containerPtr](const std::shared_ptr<Container> &ptr) {
                           return ptr.get() == containerPtr;
                         });
        if (it != containers.end())
          gameSession.removeContainer(*it);
      }
      returnState = GameState::Exploration;
    }
  }
  return returnState;
}

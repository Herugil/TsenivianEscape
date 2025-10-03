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

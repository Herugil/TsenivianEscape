#include "core/GameStateManager.h"
#include "core/GameState.h"
#include "input/Input.h"
#include "utils/ScreenUtils.h"

GameStateManager::GameStateManager(GameSession &&gameSession)
    : m_gameSession{std::move(gameSession)} {
  m_gameSession.respawnPlayer();
}

void GameStateManager::mainLoop() {
  while (true) {
    switch (m_currentState) {
    case GameState::Exploration:
      handleExploration();
      break;
    case GameState::Display:
      handleDisplay();
      break;
    case GameState::Inventory:
      handleInventory();
      break;
    case GameState::Container:
      handleContainer();
      break;
    case GameState::ActionMenu:
      handleActions();
      break;
    default:
      m_currentState = GameState::Exploration;
      break;
    }
  }
}

void GameStateManager::handleDisplay() {
  ScreenUtils::clearScreen();
  if (m_interactionResult.interactedObject) {
    std::cout << m_interactionResult.interactedObject->getDescription()
              << ".\n";
    m_interactionResult.interactedObject = nullptr;
  }
  if (!m_logsToDisplay.str().empty()) {
    std::cout << m_logsToDisplay.str();
    m_logsToDisplay.str("");
  }
  Input::getKeyBlocking();
  ScreenUtils::clearScreen();
  m_currentState = GameState::Exploration;
}

void GameStateManager::handleExploration() {
  ScreenUtils::clearScreen();
  m_gameSession.displayMap();
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  if (CommandHandler::isMovementCommand(command)) {
    m_gameSession.moveCreature(m_gameSession.getPlayerPtr(),
                               static_cast<Directions::Direction>(command));
  } else if (CommandHandler::isShoveCommand(command)) {
    auto directionCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (CommandHandler::isMovementCommand(directionCommand)) {
      m_gameSession.getPlayer().shove(
          m_gameSession, static_cast<Directions::Direction>(directionCommand));
    }
  } else if (CommandHandler::isInteractionCommand(command)) {
    auto directionCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (CommandHandler::isMovementCommand(directionCommand)) {
      auto player{m_gameSession.getPlayerPtr()};
      InteractionResult res{m_gameSession.getMap().interactPoint(
          player->getPosition().getAdjacentPoint(
              static_cast<Directions::Direction>(directionCommand)))};
      m_interactionResult = res;
      m_currentState = res.nextState;
    }
  } else if (CommandHandler::isInventoryCommand(command)) {
    m_currentState = GameState::Inventory;
  } else if (CommandHandler::isActionMenuCommand(command)) {
    m_currentState = GameState::ActionMenu;
  } else if (CommandHandler::isShowEnemiesCommand(command)) {
    m_gameSession.displayEnemiesInMap();
    Input::getKeyBlocking();
  }
}

void GameStateManager::handleInventory() {
  ScreenUtils::clearScreen();
  auto &player{m_gameSession.getPlayer()};
  player.displayInventory(m_inventoryPage);
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  if (command == Command::left && m_inventoryPage > 0)
    m_inventoryPage--;
  else if (command == Command::right &&
           m_inventoryPage < static_cast<std::size_t>(player.numObjectsHeld()) /
                                 Settings::g_itemListSize)
    m_inventoryPage++;
  else if (CommandHandler::isHotkeyCommand(command)) {
    auto pressedKey{
        static_cast<std::size_t>(CommandHandler::getPressedKey(command))};
    auto item{player.getItem(m_inventoryPage * Settings::g_itemListSize +
                             pressedKey)};
    if (item) {
      player.equipItem(item);
    }
  } else
    m_currentState = GameState::Exploration;
}

void GameStateManager::handleContainer() {
  ScreenUtils::clearScreen();
  if (m_interactionResult.interactedObject) {
    auto container{
        dynamic_cast<Container *>(m_interactionResult.interactedObject)};
    if (container) {
      auto &player{m_gameSession.getPlayer()};
      std::cout << container->getDescription() << "\n";
      container->displayContents();
      auto containerCommand{
          CommandHandler::getCommand(Input::getKeyBlocking())};
      if (CommandHandler::isTakeAllCommand(containerCommand)) {
        player.takeAllItems(*container);
        m_currentState = GameState::Exploration;
      } else if (CommandHandler::isHotkeyCommand(containerCommand)) {
        auto pressedKey{static_cast<std::size_t>(
            CommandHandler::getPressedKey(containerCommand))};
        auto item{container->popItem(pressedKey)};
        if (item) {
          player.takeItem(item);
          container->displayContents();
        }
      } else
        m_currentState = GameState::Exploration;
    }
  }
}

void GameStateManager::handleActions() {
  Player &player{m_gameSession.getPlayer()};
  player.displayActions();
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  if (CommandHandler::isHotkeyCommand(command)) {
    auto pressedKey{
        static_cast<std::size_t>(CommandHandler::getPressedKey(command))};
    auto action{player.getAction(pressedKey)};
    if (action) {
      if (action->needsDirectionalInput()) {
        auto directionCommand{
            CommandHandler::getCommand(Input::getKeyBlocking())};
        if (CommandHandler::isMovementCommand(directionCommand)) {
          auto log{action
                       ->playerExecute(
                           m_gameSession,
                           static_cast<Directions::Direction>(directionCommand))
                       .str()};
          m_logsToDisplay << log;
        }
      }
      if (action->needsHotkeyInput()) {
        m_gameSession.displayEnemiesInMap();
        auto hotkeyCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
        if (CommandHandler::isHotkeyCommand(hotkeyCommand)) {
          auto pressedKey{static_cast<std::size_t>(
              CommandHandler::getPressedKey(hotkeyCommand))};
          if (pressedKey >= m_gameSession.getEnemiesInMap().size())
            return;
          auto targetCreature{
              m_gameSession.getEnemiesInMap()[pressedKey].lock()};
          if (targetCreature) {
            m_logsToDisplay
                << action->playerExecute(m_gameSession, *targetCreature).str();
          }
        }
      } else
        ; // TODO: non directional actions (easy but not implemented)
    }
  }
  m_logsToDisplay << m_gameSession.cleanDeadNpcs().str();
  if (!m_logsToDisplay.str().empty())
    m_currentState = GameState::Display;
  else
    m_currentState = GameState::Exploration;
}
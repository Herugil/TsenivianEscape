#include "core/GameStateManager.h"
#include "Settings.h"
#include "core/GameState.h"
#include "input/Input.h"
#include "scripts/NpcCombatAI.h"
#include "utils/Interface.h"
#include "utils/ScreenUtils.h"
#include <chrono>
#include <thread>

GameStateManager::GameStateManager(GameSession &&gameSession)
    : m_gameSession{std::move(gameSession)} {
  m_gameSession.respawnPlayer();
}

void GameStateManager::mainLoop() {
  while (true) {
    switch (m_currentState) {
    case GameState::Exploration:
      m_interactionResult.interactedObject = nullptr;
      if (m_gameSession.enemiesInMap()) {
        setCombatState();
        break;
      } else
        m_gameSession.getPlayer().unsetCombat();
      ScreenUtils::clearScreen();
      m_gameSession.displayMap();
      HandleWorld();
      break;
    case GameState::DisplayBlocking:
      handleDisplayBlocking();
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
    case GameState::CombatPlayerTurn:
      handleCombatPlayerTurn();
      break;
    case GameState::CombatEnemyTurn:
      handleCombatEnemyTurn();
      break;
    default:
      m_currentState = GameState::Exploration;
      break;
    }
  }
}

void GameStateManager::handleDisplayBlocking() {
  ScreenUtils::clearScreen();
  if (m_gameSession.getPlayer().inCombat()) {
    m_gameSession.displayMap();
    Interface::displayCombatInterface(m_gameSession.getPlayer());
  }
  if (!m_logsToDisplay.str().empty()) {
    std::cout << m_logsToDisplay.str();
    m_logsToDisplay.str("");
  }
  if (m_interactionResult.interactedObject) {
    std::cout << m_interactionResult.interactedObject->getDescription()
              << ".\n";
    m_interactionResult.interactedObject = nullptr;
  }
  Input::getKeyBlocking();
  ScreenUtils::clearScreen();
  m_currentState = GameState::Exploration;
}

void GameStateManager::handleDisplay() {
  ScreenUtils::clearScreen();
  if (m_gameSession.getPlayer().inCombat()) {
    m_gameSession.displayMap();
    Interface::displayCombatInterface(m_gameSession.getPlayer());
  }
  if (!m_logsToDisplay.str().empty()) {
    std::cout << m_logsToDisplay.str();
    m_logsToDisplay.str("");
  }
  if (m_interactionResult.interactedObject) {
    std::cout << m_interactionResult.interactedObject->getDescription()
              << ".\n";
    m_interactionResult.interactedObject = nullptr;
  }
  std::this_thread::sleep_for(
      std::chrono::milliseconds(Settings::g_timeLogsDisplayMS));
  m_currentState = GameState::Exploration;
}

void GameStateManager::HandleWorld() {
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  if (CommandHandler::isMovementCommand(command)) {
    m_gameSession.moveCreature(m_gameSession.getPlayerPtr(),
                               static_cast<Directions::Direction>(command));
  } else if (CommandHandler::isShoveCommand(command)) {
    auto directionCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (CommandHandler::isMovementCommand(directionCommand)) {
      m_logsToDisplay << m_gameSession.getPlayer()
                             .shove(m_gameSession,
                                    static_cast<Directions::Direction>(
                                        directionCommand))
                             .str();
      if (!m_logsToDisplay.str().empty())
        m_currentState = GameState::Display;
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
  } else if (CommandHandler::isSkipTurnCommand(command)) {
    if (m_currentState == GameState::CombatPlayerTurn) {
      m_gameSession.getPlayer().resetTurn();
      m_gameSession.incrementTurnIndex();
      m_currentState = GameState::Exploration;
    }
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
      } else {
        m_currentState = GameState::Exploration;
      }
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

void GameStateManager::setCombatState() {
  m_gameSession.initializeTurnOrder();
  auto activeCreature{m_gameSession.getActiveCreature().lock()};
  if (activeCreature && activeCreature == m_gameSession.getPlayerPtr()) {
    m_currentState = GameState::CombatPlayerTurn;
  } else {
    m_currentState = GameState::CombatEnemyTurn;
  }
}

void GameStateManager::handleCombatPlayerTurn() {
  if (!m_gameSession.enemiesInMap()) {
    m_gameSession.getPlayer().unsetCombat();
    m_currentState = GameState::Exploration;
    return;
  }
  auto &player{m_gameSession.getPlayer()};
  ScreenUtils::clearScreen();
  m_gameSession.displayMap();
  std::cout << "Your turn: \n";
  Interface::displayCombatInterface(player);
  HandleWorld();
}

void GameStateManager::handleCombatEnemyTurn() {
  if (!m_gameSession.enemiesInMap()) {
    m_gameSession.getPlayer().unsetCombat();
    m_currentState = GameState::Exploration;
    m_gameSession.resetInitiative();
    return;
  }
  auto activeCreature{m_gameSession.getActiveCreature().lock()};
  if (activeCreature) {
    if (auto enemy{
            std::dynamic_pointer_cast<NonPlayableCharacter>(activeCreature)}) {
      ScreenUtils::clearScreen();
      m_gameSession.displayMap();
      std::cout << enemy->getName() << " turn: \n";
      Interface::displayCombatInterface(m_gameSession.getPlayer());
      std::this_thread::sleep_for(
          std::chrono::milliseconds(Settings::g_timeEnemyActionMS));
      if (enemy->getCurrentBehavior() == NonPlayableCharacter::skipTurn) {
        enemy->resetTurn();
        enemy->setDefaultBehavior();
        m_gameSession.incrementTurnIndex();
        if (m_logsToDisplay.str().empty()) {
          m_currentState = GameState::Exploration;
        } else
          m_currentState = GameState::Display;
        return;
      } else {
        m_logsToDisplay << NpcCombatAI::npcActCombat(m_gameSession, enemy).str()
                        << m_gameSession.cleanDeadNpcs().str();
      }
    }
  } else
    m_currentState = GameState::Exploration;
}
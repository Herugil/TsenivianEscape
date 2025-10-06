#include "core/GameStateManager.h"
#include "Settings.h"
#include "core/Combat.h"
#include "core/GameState.h"
#include "core/MenuSystems.h"
#include "core/SaveManager.h"
#include "core/UserInterface.h"
#include "gameObjects/creatures/Stats.h"
#include "gameObjects/items/UsableItem.h"
#include "gameObjects/terrain/RestingPlace.h"
#include "input/Input.h"
#include "scripts/NpcCombatAI.h"
#include "skills/SkillTree.h"
#include "utils/ScreenUtils.h"
#include <chrono>
#include <fstream>
#include <thread>

// TODO: this class probably does too much low level stuff, will
// need to be broken down

GameStateManager::GameStateManager(GameSession &&gameSession)
    : m_gameSession{std::move(gameSession)},
      m_interactionResult{GameState::Exploration, nullptr} {
  m_gameSession.respawnPlayer();
}

void GameStateManager::mainLoop() {
  while (true) {
    if (m_gameSession.getPlayer().isDead()) {
      m_currentState = GameState::GameOver;
    }
    switch (m_currentState) {
    case GameState::Exploration:
      m_interactionResult.interactedObject = nullptr;
      if (m_gameSession.enemiesInMap()) {
        m_currentState = Combat::getCombatState(m_gameSession);
        break;
      } else {
        m_gameSession.getPlayer().unsetCombat();
        m_gameSession.resetInitiative();
        m_gameSession.getPlayer().resetOutOfCombat();
      }
      ScreenUtils::clearScreen();
      m_gameSession.displayMap();
      if (m_gameSession.getPlayer().canLevelUp()) {
        std::cout << "You have enough XP to level up! Press 'c' to open "
                     "character sheet and level up.\n";
      }
      handleWorld();
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
      m_currentState = MenuSystems::containerMenu(
          m_gameSession, m_interactionResult.interactedObject);
      break;
    case GameState::ActionMenu: {
      std::string result{MenuSystems::actionMenu(m_gameSession)};
      m_logsToDisplay << result;
      if (!m_logsToDisplay.str().empty())
        m_currentState = GameState::Display;
      else
        m_currentState = GameState::Exploration;
      break;
    }
    case GameState::CombatPlayerTurn:
      m_currentState = Combat::playerTurn(m_gameSession);
      handleWorld();
      break;
    case GameState::CombatEnemyTurn:
      m_currentState = Combat::enemyTurn(m_gameSession, m_logsToDisplay);
      if (m_currentState == GameState::Exploration)
        m_interactionResult.interactedObject = nullptr;
      break;
    case GameState::ItemInspect: {
      auto result{MenuSystems::inspectItem(m_gameSession, m_inspectedItem)};
      m_logsToDisplay << result.logs;
      m_currentState = result.nextState;
      break;
    }
    case GameState::CharacterSheet:
      handleCharacterSheet();
      break;
    case GameState::RestMenu:
      handleRestMenu();
      break;
    case GameState::GameOver:
      handleGameOver();
      return;
    case GameState::LevelUp:
      handleLevelUp();
      break;
    case GameState::UnlockMenu:
      handleUnlockMenu();
      break;
    case GameState::MainMenu:
      handleMainMenu();
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
    UserInterface::displayCombatInterface(m_gameSession.getPlayer());
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
    UserInterface::displayCombatInterface(m_gameSession.getPlayer());
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

void GameStateManager::handleWorld() {
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  if (CommandHandler::isMovementCommand(command)) {
    m_gameSession.moveCreature(m_gameSession.getPlayerPtr(),
                               static_cast<Directions::Direction>(command));
  } else if (CommandHandler::isShoveCommand(command)) {
    auto directionCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (CommandHandler::isMovementCommand(directionCommand)) {
      m_logsToDisplay << m_gameSession.getPlayer().shove(
          m_gameSession, static_cast<Directions::Direction>(directionCommand));
      m_logsToDisplay << m_gameSession.cleanDeadNpcs();
      if (!m_logsToDisplay.str().empty())
        m_currentState = GameState::Display;
    }
  } else if (CommandHandler::isAttackCommand(command)) {
    auto directionCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (CommandHandler::isMovementCommand(directionCommand)) {
      m_logsToDisplay << m_gameSession.getPlayer().getAction(0)->playerExecute(
          m_gameSession, static_cast<Directions::Direction>(directionCommand));
      m_logsToDisplay << m_gameSession.cleanDeadNpcs();
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
  } else if (CommandHandler::isCharacterSheetCommand(command)) {
    m_currentState = GameState::CharacterSheet;
  } else if (CommandHandler::isSkipTurnCommand(command)) {
    if (m_currentState == GameState::CombatPlayerTurn) {
      m_gameSession.getPlayer().resetTurn();
      m_gameSession.getPlayer().reduceCooldowns();
      m_gameSession.incrementTurnIndex();
      m_currentState = GameState::Exploration;
    }
  } else if (CommandHandler::isMainMenuCommand(command)) {
    m_currentState = GameState::MainMenu;
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
                                 (Settings::g_itemListSize + 1))
    m_inventoryPage++;
  else if (CommandHandler::isHotkeyCommand(command)) {
    auto pressedKey{
        static_cast<std::size_t>(CommandHandler::getPressedKey(command))};
    auto item{player.getItem(m_inventoryPage * Settings::g_itemListSize +
                             pressedKey)};
    if (item) {
      m_inspectedItem = item;
      m_currentState = GameState::ItemInspect;
    }
  } else
    m_currentState = GameState::Exploration;
}

void GameStateManager::handleCharacterSheet() {
  ScreenUtils::clearScreen();
  auto &player{m_gameSession.getPlayer()};
  player.displayCharacterSheet();
  if (player.canLevelUp()) {
    std::cout << "Press 'e' to level up!\n";
  }
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  if (CommandHandler::isInteractionCommand(command) && player.canLevelUp()) {
    m_currentState = GameState::LevelUp;
    return;
  }
  m_currentState = GameState::Exploration;
}

void GameStateManager::handleGameOver() {
  ScreenUtils::clearScreen();
  std::cout << "Game Over! You have died.\n";
  std::cout << "Press any key to exit.\n";
  Input::getKeyBlocking();
  m_currentState = GameState::MainMenu;
}

void GameStateManager::handleRestMenu() {
  ScreenUtils::clearScreen();
  auto &player{m_gameSession.getPlayer()};
  if (auto restingPlace =
          dynamic_cast<RestingPlace *>(m_interactionResult.interactedObject)) {
    std::cout << restingPlace->getDescription() << "\n";
    std::cout << "R: Rest   Any other key: Leave\n";
    auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (CommandHandler::isShoveCommand(command)) {
      restingPlace->rest(player);
      std::cout << "You wake up feeling refreshed.\n";
      m_currentState = GameState::Exploration;
      m_interactionResult.interactedObject = nullptr;
      return;
    }
  }
  m_interactionResult.interactedObject = nullptr;
  m_currentState = GameState::Exploration;
}

void GameStateManager::handleUnlockMenu() {
  auto interactedObject{m_interactionResult.interactedObject};
  std::cout << interactedObject->getDescription() << "\n";
  std::cout << "This object is locked. You need a key to unlock it.\n";
  std::string keyId{interactedObject->getKeyId()};
  auto &player{m_gameSession.getPlayer()};
  auto it{std::find_if(player.getInventory().begin(),
                       player.getInventory().end(),
                       [&keyId](const std::shared_ptr<Item> &item) {
                         return item->getId() == keyId;
                       })};
  if (it != player.getInventory().end()) {
    std::cout << "Use " << (*it)->getName()
              << " to unlock? (E: yes, any other "
                 "key: no)\n";
    auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (CommandHandler::isInteractionCommand(command)) {
      m_interactionResult.interactedObject->unlock();
      player.removeItem(*it);
      m_logsToDisplay << "You unlocked " << interactedObject->getName()
                      << ".\n";
      m_interactionResult.interactedObject = nullptr;
      m_currentState = GameState::Display;
      return;
    }
  } else {
    Input::getKeyBlocking();
  }
  m_interactionResult.interactedObject = nullptr;
  m_currentState = GameState::Exploration;
}

// Level up functions could be moved out

void GameStateManager::handleLevelUp() {
  ScreenUtils::clearScreen();
  auto &player{m_gameSession.getPlayer()};
  Stats playerStats{player.getStats()};
  if (player.canLevelUp()) {
    std::cout << "Pick a stat to increase:\n";
    std::cout << "1: Strength" << playerStats.strength << "->"
              << playerStats.strength + 1 << '\n';
    std::cout << "2: Dexterity" << playerStats.dexterity << "->"
              << playerStats.dexterity + 1 << '\n';
    std::cout << "3: Intelligence" << playerStats.intelligence << "->"
              << playerStats.intelligence + 1 << '\n';
    std::cout << "4: Constitution" << playerStats.constitution << "->"
              << playerStats.constitution + 1 << '\n';
    auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (CommandHandler::isHotkeyCommand(command)) {
      auto pressedKey{CommandHandler::getPressedKey(command)};
      if (pressedKey >= 0 && pressedKey <= 3) {
        switch (pressedKey) {
        case 0:
          confirmLevelUp(player, Stat::Strength, playerStats);
          break;
        case 1:
          confirmLevelUp(player, Stat::Dexterity, playerStats);
          break;
        case 2:
          confirmLevelUp(player, Stat::Intelligence, playerStats);
          break;
        case 3:
          confirmLevelUp(player, Stat::Constitution, playerStats);
          break;
        default:
          break;
        }
      }
    }
  }
  m_currentState = GameState::CharacterSheet;
}

void GameStateManager::confirmLevelUp(Player &player, Stat stat,
                                      Stats playerStats) {
  auto &actions{player.getAllActions()};
  switch (stat) {
  case Stat::Strength:
    playerStats.strength += 1;
    break;
  case Stat::Dexterity:
    playerStats.dexterity += 1;
    break;
  case Stat::Intelligence:
    playerStats.intelligence += 1;
    break;
  case Stat::Constitution:
    playerStats.constitution += 1;
    break;
  default:
    return;
  }
  std::cout << "Confirm? (press E) \nYou would get: \n";
  for (const auto &action : SkillTree::getSkillsStatSpread(playerStats)) {
    auto isNew{std::find_if(actions.begin(), actions.end(),
                            [actionName = action->getName()](
                                const std::unique_ptr<Action> &a) {
                              return a->getName() == actionName;
                            }) == actions.end()};
    if (isNew) {
      std::cout << action->getName() << '\n';
    }
  }
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  if (CommandHandler::isInteractionCommand(command)) {
    player.levelUp(stat);
    return;
  }
}

void GameStateManager::handleMainMenu() {
  bool inSession{m_gameSession.isGameActive()};
  int pressedKey{UserInterface::mainMenu(inSession)};
  if (pressedKey == 0) {
    UserInterface::NewGameResult result{UserInterface::newGame(inSession)};
    if (!result.confirmed)
      return;
    m_gameSession = SaveManager::newGame(result.playerName);
    m_currentState = GameState::Exploration;
  } else if (inSession && pressedKey == 1) {
    m_currentState = GameState::Exploration;
  } else if (inSession && pressedKey == 2) {
    if (UserInterface::saveGameMenu(m_gameSession.isGameActive(),
                                    m_gameSession.getPlayer().inCombat()))
      SaveManager::saveGame(m_gameSession);
    else
      return;
  } else if ((inSession && pressedKey == 3) ||
             (!inSession && pressedKey == 1)) {
    auto result{UserInterface::loadGameMenu(SaveManager::getAvailableSaves())};
    {
      try {
        m_gameSession = SaveManager::loadGame(result.saveSlot);
        m_currentState = GameState::Exploration;
      } catch (const std::exception &e) {
        std::cerr << "Error loading game: " << e.what() << '\n';
      }
    }
  } else if ((inSession && pressedKey == 4) ||
             (!inSession && pressedKey == 2)) {
    std::cout << "Are you sure you want to exit? All unsaved progress will be "
                 "lost. Press 'e' to confirm.\n";
    auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (!CommandHandler::isInteractionCommand(command))
      return;
    ScreenUtils::clearScreen();
    std::exit(0);
  }
}

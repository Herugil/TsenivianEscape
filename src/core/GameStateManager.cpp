#include "core/GameStateManager.h"
#include "Settings.h"
#include "core/GameState.h"
#include "gameObjects/creatures/Stats.h"
#include "gameObjects/items/UsableItem.h"
#include "gameObjects/terrain/RestingPlace.h"
#include "input/Input.h"
#include "scripts/NpcCombatAI.h"
#include "skills/SkillTree.h"
#include "utils/Interface.h"
#include "utils/ScreenUtils.h"
#include <chrono>
#include <fstream>
#include <thread>

GameStateManager::GameStateManager(GameSession &&gameSession)
    : m_gameSession{std::move(gameSession)} {
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
        setCombatState();
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
    case GameState::ItemInspect:
      handleItemInspect();
      break;
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
    ScreenUtils::clearScreen();
    handleMainMenu();
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

void GameStateManager::handleItemInspect() {
  ScreenUtils::clearScreen();
  if (auto item{m_inspectedItem.lock()}) {
    std::cout << item->getDisplayItem() << "\n";
    if (auto equipment = std::dynamic_pointer_cast<Equipment>(item))
      std::cout << "E: Equip/Unequip   R: Drop\n";
    else if (auto usable = std::dynamic_pointer_cast<UsableItem>(item))
      std::cout << "E: Use   R: Drop\n";
    else
      std::cout << "R: Drop\n";
    auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (CommandHandler::isInteractionCommand(command)) {
      auto &player{m_gameSession.getPlayer()};
      if (auto equipment = std::dynamic_pointer_cast<Equipment>(item))
        player.equipItem(equipment);
      else if (auto usable = std::dynamic_pointer_cast<UsableItem>(item)) {
        m_logsToDisplay << player.useItem(usable);
        m_logsToDisplay << m_gameSession.cleanDeadNpcs();
        if (!m_logsToDisplay.str().empty()) {
          m_currentState = GameState::Display;
          return;
        }
      }
    } else if (CommandHandler::isShoveCommand(command)) {
      auto &player{m_gameSession.getPlayer()};
      if (m_gameSession.dropItem(item, player.getPosition()))
        player.removeItem(item);
      else {
        m_logsToDisplay << "Could not drop item! No space around you.\n";
        m_currentState = GameState::Display;
        return;
      }
    }
  }
  m_inspectedItem.reset();
  m_currentState = GameState::Inventory;
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
    if (!action)
      return;
    if (action->needsHotkeyInput()) {
      m_gameSession.displayEnemiesInMap(action->getUsedStat());
      auto hotkeyCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
      if (CommandHandler::isHotkeyCommand(hotkeyCommand)) {
        auto pressedKey{static_cast<std::size_t>(
            CommandHandler::getPressedKey(hotkeyCommand))};
        if (pressedKey >= m_gameSession.getEnemiesInMap().size())
          return;
        auto targetCreature{m_gameSession.getEnemiesInMap()[pressedKey].lock()};
        if (targetCreature) {
          m_logsToDisplay << action->playerExecute(m_gameSession,
                                                   *targetCreature);
        }
      }
    } else if ((action->needsDirectionalInput())) {
      auto directionCommand{
          CommandHandler::getCommand(Input::getKeyBlocking())};
      if (CommandHandler::isMovementCommand(directionCommand)) {
        auto log{action->playerExecute(
            m_gameSession,
            static_cast<Directions::Direction>(directionCommand))};
        m_logsToDisplay << log;
      }
    } else {
      auto log{action->playerExecute(m_gameSession)};
      m_logsToDisplay << log;
    }
  }
  m_logsToDisplay << m_gameSession.cleanDeadNpcs();
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
  std::cout << "Round " << m_gameSession.getCurrentTurn() << "\n";
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
      std::cout << "Round " << m_gameSession.getCurrentTurn() << "\n";
      std::cout << enemy->getName() << " turn: \n";
      Interface::displayCombatInterface(m_gameSession.getPlayer());
      std::this_thread::sleep_for(
          std::chrono::milliseconds(Settings::g_timeEnemyActionMS));
      if (enemy->getCurrentBehavior() == NonPlayableCharacter::skipTurn) {
        enemy->resetTurn();
        enemy->reduceCooldowns();
        enemy->setDefaultBehavior();
        m_gameSession.incrementTurnIndex();
        if (m_logsToDisplay.str().empty()) {
          m_currentState = GameState::Exploration;
        } else
          m_currentState = GameState::Display;
        return;
      }
      m_logsToDisplay << NpcCombatAI::npcActCombat(m_gameSession, enemy)
                      << m_gameSession.cleanDeadNpcs();
      if (!m_logsToDisplay.str().empty())
        m_currentState = GameState::Display;
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

// Note: below this, a bunch of functions probably belong to another class
// altogether

void GameStateManager::handleMainMenu() {
  ScreenUtils::clearScreen();
  std::cout << "Main Menu\n";
  std::cout << "1: New Game\n";
  std::cout << "2: Continue Game\n";
  std::cout << "3: Save Game\n";
  std::cout << "4: Load Game\n";
  std::cout << "5: Exit to Desktop\n";
  std::cout << "Press the corresponding number key to choose an option.\n";
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  auto pressedKey{CommandHandler::getPressedKey(command)};
  if (pressedKey == 0) {
    ScreenUtils::clearScreen();
    std::string name;
    while (true) {
      std::cout << "Enter your character's name: ";
      std::getline(std::cin, name);
      if (!name.empty()) {
        break;
      }
    }
    newGame(name);
    m_currentState = GameState::Exploration;
  } else if (pressedKey == 1) {
    if (m_gameSession.getPlayer().getName().empty() ||
        m_gameSession.getPlayer().isDead()) {
      std::cout << "You currently are not in a game session.\n";
      std::cout << "Press any key to return to main menu.\n";
      Input::getKeyBlocking();
      return;
    }
    m_currentState = GameState::Exploration;
  } else if (pressedKey == 2) {
    if (m_gameSession.getPlayer().getName().empty() ||
        m_gameSession.getPlayer().isDead()) {
      std::cout << "You currently are not in a game session.\n";
      std::cout << "Press any key to return to main menu.\n";
      Input::getKeyBlocking();
      return;
    }
    if (m_gameSession.getPlayer().inCombat()) {
      std::cout << "You cannot save the game while in combat!\n";
      std::cout << "Press any key to return to main menu.\n";
      Input::getKeyBlocking();
      return;
    } else
      saveGame();
  } else if (pressedKey == 3) {
    auto saveList{getAvailableSaves()};
    if (saveList.empty()) {
      std::cout << "No saves available.\n";
      std::cout << "Press any key to return to main menu.\n";
      Input::getKeyBlocking();
      return;
    }
    std::cout << "Available saves:\n";
    for (std::size_t i = 0; i < saveList.size(); ++i) {
      std::cout << i + 1 << ": " << saveList[i] << '\n';
    }
    std::cout << "Select a save to load: ";
    auto loadCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
    auto loadPressedKey{
        static_cast<std::size_t>(CommandHandler::getPressedKey(loadCommand))};
    if (loadPressedKey >= 0 && loadPressedKey < saveList.size()) {
      loadGame(saveList[loadPressedKey]);
      m_currentState = GameState::Exploration;
    }
  } else if (pressedKey == 4) {
    ScreenUtils::clearScreen();
    std::exit(0);
  }
}

void GameStateManager::saveGame() const {
  if (m_gameSession.getPlayer().getName().empty()) {
    std::cout << "You currently are not in a game session.\n";
    std::cout << "Press any key to return to main menu.\n";
    Input::getKeyBlocking();
    return;
  }
  auto saveList{getAvailableSaves()};
  std::cout << "Available saves:\n";
  std::cout << "1: Create new save\n";
  for (std::size_t i = 0; i < saveList.size(); ++i) {
    std::cout << i + 2 << ": " << saveList[i] << '\n';
  }
  std::cout << "Select a save to overwrite or create a new one:\n ";
  auto command{CommandHandler::getCommand(Input::getKeyBlocking())};
  auto pressedKey{
      static_cast<std::size_t>(CommandHandler::getPressedKey(command))};
  std::string filename;
  if (pressedKey == 0) {
    if (saveList.size() >= Settings::g_maxSaves) {
      std::cout << "You have reached the maximum number of saves ("
                << Settings::g_maxSaves
                << "). Please overwrite an existing save instead.\n";
      std::cout << "Press any key to return to main menu.\n";
      Input::getKeyBlocking();
      return;
    }
    filename = getSaveFileName();
  } else if (pressedKey > 0 && pressedKey <= saveList.size()) {
    std::cout << "Are you sure you want to overwrite "
              << saveList[pressedKey - 1] << "? (E: yes, any other key: no)\n";
    auto confirmCommand{CommandHandler::getCommand(Input::getKeyBlocking())};
    if (CommandHandler::isInteractionCommand(confirmCommand)) {
      filename = getSaveFileName(saveList[pressedKey - 1]);
      if (!deleteSave(saveList[pressedKey - 1])) {
        std::cout << "Failed to delete existing save. Save aborted.\n";
        std::cout << "Press any key to return to main menu.\n";
        Input::getKeyBlocking();
        return;
      }
    }
  }
  std::string fileNameStr{"../saves/"};
  fileNameStr += filename;
  fileNameStr += ".json";
  std::ofstream file{fileNameStr.data()};
  if (!file.is_open()) {
    std::cerr << "Could not open file for saving: " << filename << '\n';
    return;
  }
  json j = m_gameSession.toJson();
  file << j.dump(4);
  file.close();
}

void GameStateManager::loadGame(const std::string &filename) {
  std::string fullFileName{"../saves/" + filename + ".json"};
  std::ifstream file{fullFileName};
  if (!file.is_open()) {
    std::cerr << "Could not open save file: " << fullFileName << '\n';
    return;
  }
  json j;
  file >> j;
  m_gameSession = GameSession::loadFromJson(j);
}

void GameStateManager::newGame(std::string_view name) {
  m_gameSession =
      GameSession(std::make_shared<Player>(Point(2, 1), "level1", 10, name));
  std::unordered_map<std::string, std::shared_ptr<Item>> items{
      DataLoader::getAllItems()};
  std::unordered_map<std::string, std::shared_ptr<NonPlayableCharacter>> npcs{
      DataLoader::getAllNpcs()};
  DataLoader::populateGameSession(items, npcs, m_gameSession);
  m_gameSession.respawnPlayer();
}

std::vector<std::string> GameStateManager::getAvailableSaves() const {
  std::vector<std::string> saveFiles;
  for (const auto &entry : std::filesystem::directory_iterator("../saves/")) {
    if (entry.is_regular_file() && entry.path().extension() == ".json") {
      saveFiles.push_back(entry.path().stem().string());
    }
  }
  return saveFiles;
}

std::string
GameStateManager::getSaveFileName(std::string_view saveOverwrite) const {
  std::string filename;
  auto saveList{getAvailableSaves()};
  while (true) {
    std::cout << "Enter new save name: ";
    std::getline(std::cin, filename);
    if (filename.empty()) {
      std::cout << "Filename cannot be empty.\n";
      continue;
    }
    auto it{std::find_if(filename.begin(), filename.end(), [](char c) {
      return !(std::isalnum(c) || c == '_' || c == '-');
    })};
    if (it != filename.end()) {
      std::cout << "Filename can only contain alphanumeric characters.\n";
      continue;
    }
    auto it2{std::find(saveList.begin(), saveList.end(), filename)};
    if (it2 != saveList.end() && filename != saveOverwrite) {
      std::cout
          << "A save with this name already exists. Choose another name.\n";
      continue;
    }
    return filename;
  }
}

bool GameStateManager::deleteSave(std::string_view filename) const {
  std::string fullFileName{filename};
  fullFileName = "../saves/" + fullFileName + ".json";
  if (std::remove(fullFileName.c_str()) != 0) {
    std::cerr << "Error deleting file: " << fullFileName << '\n';
    return false;
  } else {
    std::cout << "Deleted save file: " << fullFileName << '\n';
    return true;
  }
}
#include "core/SaveManager.h"
#include "Settings.h"
#include "core/GameSession.h"
#include "input/Input.h"
#include <fstream>
#include <iostream>

void SaveManager::saveGame(const GameSession &gameSession) {
  if (gameSession.getPlayer().getName().empty()) {
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
  json j = gameSession.toJson();
  file << j.dump(4);
  file.close();
}

GameSession SaveManager::loadGame(std::string_view filename) {
  std::string fullFileName{"../saves/" + std::string(filename) + ".json"};
  std::ifstream file{fullFileName};
  if (!file.is_open()) {
    throw std::runtime_error("Could not open save file: " + fullFileName);
  }
  json j;
  file >> j;
  return GameSession::loadFromJson(j);
}

GameSession SaveManager::newGame(std::string_view name) {
  GameSession gameSession{
      std::make_shared<Player>(Point(1, 4), "level1", 10, name)};
  std::unordered_map<std::string, std::shared_ptr<Item>> items{
      DataLoader::getAllItems()};
  std::unordered_map<std::string, std::shared_ptr<NonPlayableCharacter>> npcs{
      DataLoader::getAllNpcs()};
  DataLoader::populateGameSession(items, npcs, gameSession);
  gameSession.setCurrentMap("level1");
  gameSession.respawnPlayer();
  return gameSession;
}

std::vector<std::string> SaveManager::getAvailableSaves() {
  std::vector<std::string> saveFiles;
  for (const auto &entry : std::filesystem::directory_iterator("../saves/")) {
    if (entry.is_regular_file() && entry.path().extension() == ".json") {
      saveFiles.push_back(entry.path().stem().string());
    }
  }
  return saveFiles;
}

std::string SaveManager::getSaveFileName(std::string_view saveOverwrite) {
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

bool SaveManager::deleteSave(std::string_view filename) {
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
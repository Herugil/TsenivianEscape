#include "dataLoading/parseJson.h"
#include "core/GameSession.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <gameObjects/items/Item.h>
#include <gameObjects/items/Weapon.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

using json = nlohmann::json;

std::vector<std::string> getLevelJsonFiles(std::string_view dataFolder) {
  std::vector<std::string> levelFiles;
  for (const auto &entry : std::filesystem::directory_iterator(dataFolder)) {
    if (entry.is_regular_file()) {
      auto path = entry.path();
      if (path.extension() == ".json" &&
          path.filename().string().rfind("level", 0) == 0) {
        levelFiles.push_back(path.string());
      }
    }
  }
  return levelFiles;
}

std::unordered_map<std::string, std::shared_ptr<Item>>
DataLoader::getAllItems() {
  std::unordered_map<std::string, std::shared_ptr<Item>> items{};
  std::ifstream f("../data/items.json");
  json data = json::parse(f);
  for (auto &[key, value] : data.items()) {
    if (value["itemType"] == "oneHanded") {
      std::string name{value["name"]};
      int damage{value["damage"]};
      int range{value["range"]};
      items[key] = std::make_shared<Item>(
          Weapon{name, Item::ItemType::oneHanded, damage, range});
    }
  }
  return items;
}

void placeWalls(json &data, Map &map) {
  std::string wallDesc{data["wallDescription"]};
  const auto &arrWalls{data["walls"]};
  assert((arrWalls.size() % 2 == 0) &&
         "Walls list needs to have an even length!");
  for (std::size_t i{0}; i < arrWalls.size(); i += 2) {
    Point p1{arrWalls[i][0], arrWalls[i][1]};
    Point p2{arrWalls[i + 1][0], arrWalls[i + 1][1]};
    map.placeWalls(p1, p2, wallDesc);
  }
}

void placeEnemies(json &data, Map &map) {
  std::string wallDesc{data["wallDescription"]};
  const auto &arrWalls{data["walls"]};
  assert((arrWalls.size() % 2 == 0) &&
         "Walls list needs to have an even length!");
  for (std::size_t i{0}; i < arrWalls.size(); i += 2) {
    Point p1{arrWalls[i][0], arrWalls[i][1]};
    Point p2{arrWalls[i + 1][0], arrWalls[i + 1][1]};
    map.placeWalls(p1, p2, wallDesc);
  }
}

void placeEnemies(json &data, Map &map,
                  [[maybe_unused]] GameSession &gameSession) {
  std::string wallDesc{data["wallDescription"]};
  const auto &arrWalls{data["walls"]};
  assert((arrWalls.size() % 2 == 0) &&
         "Walls list needs to have an even length!");
  for (std::size_t i{0}; i < arrWalls.size(); i += 2) {
    Point p1{arrWalls[i][0], arrWalls[i][1]};
    Point p2{arrWalls[i + 1][0], arrWalls[i + 1][1]};
    map.placeWalls(p1, p2, wallDesc);
  }
}

std::unordered_map<std::string, Map> DataLoader::getAllMaps(
    [[maybe_unused]] std::unordered_map<std::string, std::shared_ptr<Item>>
        &items,
    GameSession &gameSession) {
  std::unordered_map<std::string, Map> allMaps{};
  std::vector<std::string> allLevelNames{getLevelJsonFiles("../data/")};
  for (auto name : allLevelNames) {
    std::ifstream f("../data/" + name);
    json data = json::parse(f);
    std::string textFileName{data["introTextFile"]};
    std::ifstream fIntroText("../data/" + textFileName);
    std::stringstream buffer;
    buffer << fIntroText.rdbuf();
    std::string introText = buffer.str();
    name.erase(0, 8); // this is the "../data/" part
    auto it{name.find(".json")};
    name.erase(it);

    Map map{name, data["mapWidth"], data["mapHeight"], introText};
    placeWalls(data, map);
    placeEnemies(data, map, gameSession);

    allMaps.emplace(name, std::move(map));
  }
  return allMaps;
}
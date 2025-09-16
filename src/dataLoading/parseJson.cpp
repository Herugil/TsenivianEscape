#include "dataLoading/parseJson.h"
#include "core/GameSession.h"
#include "gameObjects/items/Item.h"
#include "gameObjects/items/Weapon.h"
#include "gameObjects/terrain/MapChanger.h"
#include <cassert>
#include <filesystem>
#include <fstream>
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
    if (value["itemType"] == "oneHanded" || value["itemType"] == "twoHanded") {
      std::string weaponType{value["weaponType"]};
      std::string itemType{value["itemType"]};
      std::string name{value["name"]};
      int damage{value["damage"]};
      int range{value["range"]};
      items[key] = std::make_shared<Weapon>(Weapon{
          name, weaponType, Item::getTypeFromStr(itemType), damage, range});
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

void placeEnemies(
    json &data, Map &map, GameSession &gameSession,
    const std::unordered_map<std::string, std::shared_ptr<Item>> &items) {
  const auto &arrEnemies{data["enemies"]};
  for (auto enemy : arrEnemies) {
    char symbol{std::string(enemy[0])[0]}; // json doesnt have char
    Point pos{enemy[1][0], enemy[1][1]};
    int maxHealth{enemy[2]};
    const auto &itemIds{enemy[3]};
    std::vector<std::shared_ptr<Item>> loot{};
    for (auto itemId : itemIds) {
      if (items.contains(itemId))
        loot.emplace_back(items.at(itemId)->clone());
      else
        std::cout << itemId << " not added, cant find it in items.\n";
    }
    std::string name{enemy[4]};
    std::string desc{enemy[5]};
    std::string descDead{enemy[6]};

    auto npc{std::make_shared<NonPlayableCharacter>(
        symbol, pos, map.getName(), maxHealth, loot, name, desc, descDead)};
    gameSession.addNpc(npc);
  }
}

void placeObjects(
    json &data, Map &map,
    const std::unordered_map<std::string, std::shared_ptr<Item>> &items) {
  const auto &arrObjects{data["gameObjects"]};
  for (auto object : arrObjects) {
    char symbol{std::string(object[0])[0]}; // json doesnt have char
    Point pos{object[1][0], object[1][1]};
    const std::string type{object[2]};
    const std::string name{object[3]};
    const std::string desc{object[4]};
    if (type == "container") {
      std::vector<std::shared_ptr<Item>> loot{};
      const auto &itemIds{object[5]};
      for (auto itemId : itemIds) {
        if (items.contains(itemId))
          loot.emplace_back(items.at(itemId)->clone());
        else
          std::cout << itemId << " not added, cant find it in items.\n";
      }
      Container container{loot, pos, name, map.getName(), desc, symbol};
      map.placeFloor(std::make_unique<Container>(std::move(container)), pos);
    } else if (type == "mapChanger") {
      const std::string nextMap{object[5]};
      Point spawningPoint{object[6][0], object[6][1]};
      MapChanger obj{map.getName(), pos,  nextMap, spawningPoint,
                     symbol,        name, desc};
      map.placeFloor(std::make_unique<MapChanger>(std::move(obj)), pos);
    } else {
      GameObject obj{false, false, symbol, map.getName(), pos, name, desc};
      map.placeFloor(std::make_unique<GameObject>(std::move(obj)), pos);
    }
  }
}

void DataLoader::populateGameSession(
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

    gameSession.addMap(
        Map{name, data["mapWidth"], data["mapHeight"], introText});
    gameSession.setCurrentMap(name);

    placeWalls(data, gameSession.getMap());
    placeEnemies(data, gameSession.getMap(), gameSession, items);
    placeObjects(data, gameSession.getMap(), items);
  }
}
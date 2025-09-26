#include "dataLoading/parseJson.h"
#include "core/GameSession.h"
#include "gameObjects/items/Armor.h"
#include "gameObjects/items/InstantUsableItem.h"
#include "gameObjects/items/Item.h"
#include "gameObjects/items/Weapon.h"
#include "gameObjects/terrain/MapChanger.h"
#include "gameObjects/terrain/RestingPlace.h"
#include "scripts/passives/CompositePassiveEffect.h"
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
    std::string name{value["name"]};
    std::string description{value["description"]};
    std::string itemType{};
    if (value.contains("itemType"))
      itemType = value["itemType"];
    if (itemType == "oneHanded" || itemType == "twoHanded") {
      std::string weaponType{value["weaponType"]};
      int damage{value["damage"]};
      int range{value["range"]};
      items[key] = std::make_shared<Weapon>(
          Weapon{name, key, itemType, weaponType, description, damage, range});
    } else if (itemType == "instantUsableItem") {
      int costToUse{value["costToUse"]};
      bool isUnlimitedUse{value["isUnlimitedUse"]};
      int usesLeft{value["usesLeft"]};
      int effectValue{value["effectValue"]};
      InstantUsableItem::Type regenType{
          static_cast<InstantUsableItem::Type>(value["regenType"])};
      items[key] = std::make_shared<InstantUsableItem>(
          InstantUsableItem{name, key, regenType, effectValue, costToUse,
                            isUnlimitedUse, usesLeft, description});
    } else if (itemType == "chestArmor" || itemType == "legArmor" ||
               itemType == "helmet" || itemType == "boots" ||
               itemType == "gloves") {
      std::string name{value["name"]};
      std::string description{value["description"]};
      int armorValue{value["armorValue"]};
      items[key] = std::make_shared<Armor>(
          Armor{name, key, itemType, description, armorValue});
    } else {
      items[key] = std::make_shared<Item>(name, key, description);
    }
  }
  return items;
}

std::unique_ptr<PassiveEffect> parsePassiveEffect(const json &data) {
  std::string typeStr{data["type"]};
  PassiveEffect::Type type{PassiveEffect::typeFromString(typeStr)};
  int duration{data["duration"]};
  bool stackable{data["stackable"]};
  if (data.contains("stackable")) {
    stackable = data["stackable"];
  }
  std::string id{data["id"]};
  std::string name{data["name"]};
  if (type == PassiveEffect::Type::Composite) {
    std::vector<std::unique_ptr<PassiveEffect>> effects{};
    for (const auto &effectData : data["components"]) {
      std::string effectTypeStr{effectData["type"]};
      int effectValue{effectData["value"]};
      effects.emplace_back(std::make_unique<PassiveEffect>(
          PassiveEffect::typeFromString(effectTypeStr), effectValue, 0, id,
          "component", false));
    }
    return std::make_unique<CompositePassiveEffect>(std::move(effects),
                                                    duration, id, name);
  }
  int value{data["value"]};
  return std::make_unique<PassiveEffect>(type, value, duration, id, name,
                                         stackable);
}

std::unordered_map<std::string, std::shared_ptr<NonPlayableCharacter>>
DataLoader::getAllNpcs() {
  std::unordered_map<std::string, std::shared_ptr<NonPlayableCharacter>> npcs{};
  std::ifstream f("../data/npcs.json");
  json data = json::parse(f);
  for (auto &[key, value] : data.items()) {
    int maxHealth{value["maxHealth"]};
    int meleeHitChance{value["meleeHitChance"]};
    int distanceHitChance{value["distanceHitChance"]};
    int meleeDamage{value["meleeDamage"]};
    int distanceDamage{value["distanceDamage"]};
    int evasion{value["evasion"]};
    std::string name{value["name"]};
    std::string desc{value["description"]};
    std::string descDead{value["deadDescription"]};
    std::string aiType{value["type"]};
    int xpValue{value["xpValue"]};
    std::vector<std::unique_ptr<Action>> actions{};
    if (value.contains("actions")) {
      for (auto &action : value["actions"]) {
        if (action["isBasicAttack"]) {
          std::vector<std::unique_ptr<PassiveEffect>> effects{};
          if (action.contains("effects")) {
            for (const auto &effectData : action["effects"]) {
              effects.emplace_back(parsePassiveEffect(effectData));
            }
          }
          std::string name{action["name"]};
          Stat usedStat{Stat::Strength};
          if (action.contains("type")) {
            std::string type{action["type"]};
            if (type == "melee")
              usedStat = Stat::Strength;
            else if (type == "ranged")
              usedStat = Stat::Dexterity;
            else
              usedStat = Stat::Strength; // default to melee
          }
          actions.emplace_back(std::make_unique<BasicAttack>(
              name, usedStat, std::move(effects)));
        } else {
          std::cout << "Unknown action type for NPC " << key << "\n";
        }
      }
    }
    npcs[key] = std::make_shared<NonPlayableCharacter>(
        ' ', Point{0, 0}, "placeholder", maxHealth, name, evasion,
        meleeHitChance, distanceHitChance, meleeDamage, distanceDamage,
        std::move(actions), std::vector<std::shared_ptr<Item>>{}, desc,
        descDead, aiType, xpValue);
  }
  return npcs;
}

void DataLoader::placeWalls(json &data, Map &map) {
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

void DataLoader::placeEnemies(
    json &data, Map &map, GameSession &gameSession,
    const std::unordered_map<std::string, std::shared_ptr<NonPlayableCharacter>>
        &npcs,
    const std::unordered_map<std::string, std::shared_ptr<Item>> &items) {
  const auto &arrEnemies{data["enemies"]};
  for (auto enemy : arrEnemies) {
    char symbol{std::string(enemy["symbol"])[0]}; // json doesnt have char
    Point pos{enemy["position"][0], enemy["position"][1]};
    std::string id{enemy["id"]};
    if (!npcs.contains(id)) {
      std::cout << id << " not added, cant find it in npcs.\n";
      continue;
    }
    auto npc = npcs.at(id)->clone();
    for (auto itemId : enemy["inventory"]) {
      if (items.contains(itemId))
        npc->addItemToInventory(items.at(itemId)->clone());
      else
        std::cout << itemId << " not added, cant find it in items.\n";
    }
    npc->setSymbol(symbol);
    npc->setPosition(pos);
    npc->setCurrentMap(map.getName());
    gameSession.addNpc(npc);
  }
}

void placeObjects(
    json &data, Map &map,
    const std::unordered_map<std::string, std::shared_ptr<Item>> &items) {
  const auto &arrObjects{data["gameObjects"]};
  for (auto object : arrObjects) {
    char symbol{std::string(object["symbol"])[0]}; // json doesnt have char
    Point pos{object["position"][0], object["position"][1]};
    const std::string type{object["type"]};
    const std::string name{object["name"]};
    const std::string desc{object["description"]};
    bool locked{false};
    std::string keyId{};
    if (object.contains("locked")) {
      locked = object["locked"];
      keyId = object["keyId"];
    }
    if (type == "container") {
      std::vector<std::shared_ptr<Item>> loot{};
      const auto &itemIds{object["contents"]};
      for (auto itemId : itemIds) {
        if (items.contains(itemId))
          loot.emplace_back(items.at(itemId)->clone());
        else
          std::cout << itemId << " not added, cant find it in items.\n";
      }
      Container container{
          std::move(loot), pos,    map.getName(), name, desc,
          symbol,          locked, keyId,
      };
      map.placeFloor(std::make_unique<Container>(std::move(container)), pos);
    } else if (type == "mapChanger") {
      const std::string nextMap{object["destLevel"]};
      Point spawningPoint{object["destPosition"][0], object["destPosition"][1]};
      MapChanger obj{map.getName(), pos,  nextMap, spawningPoint, symbol,
                     name,          desc, locked,  keyId};
      map.placeFloor(std::make_unique<MapChanger>(std::move(obj)), pos);
    } else if (type == "restingPlace") {
      RestingPlace obj{map.getName(), pos, symbol, name, desc};
      map.placeFloor(std::make_unique<RestingPlace>(std::move(obj)), pos);
    } else {
      GameObject obj{false, false, symbol, map.getName(), pos,
                     name,  desc,  locked, keyId};
      map.placeFloor(std::make_unique<GameObject>(std::move(obj)), pos);
    }
  }
}

void DataLoader::populateGameSession(
    std::unordered_map<std::string, std::shared_ptr<Item>> &items,
    std::unordered_map<std::string, std::shared_ptr<NonPlayableCharacter>>
        &npcs,
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
    placeEnemies(data, gameSession.getMap(), gameSession, npcs, items);
    placeObjects(data, gameSession.getMap(), items);
  }
}
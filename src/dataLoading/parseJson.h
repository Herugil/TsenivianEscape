#include "gameObjects/creatures/NonPlayableCharacter.h"
#include "gameObjects/items/Item.h"
#include "gameObjects/items/Weapon.h"
#include "map/Map.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>

class Action;
class GameSession;

namespace DataLoader {
using json = nlohmann::json;
void populateGameSession(
    std::unordered_map<std::string, std::shared_ptr<Item>> &items,
    std::unordered_map<std::string, std::shared_ptr<NonPlayableCharacter>>
        &npcs,
    GameSession &gameSession);
std::unordered_map<std::string, std::shared_ptr<Item>> getAllItems();
std::unordered_map<std::string, std::shared_ptr<NonPlayableCharacter>>
getAllNpcs();
void placeEnemies(
    json &data, Map &map, GameSession &gameSession,
    const std::unordered_map<std::string, std::shared_ptr<NonPlayableCharacter>>
        &npcs,
    const std::unordered_map<std::string, std::shared_ptr<Item>> &items);
void placeWalls(json &data, Map &map);
std::unique_ptr<Action> parseAction(const json &j);
} // namespace DataLoader
#include "gameObjects/items/Item.h"
#include "gameObjects/items/Weapon.h"
#include "map/Map.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>

class GameSession;

namespace DataLoader {
using json = nlohmann::json;
void populateGameSession(
    std::unordered_map<std::string, std::shared_ptr<Item>> &items,
    GameSession &gameSession);
std::unordered_map<std::string, std::shared_ptr<Item>> getAllItems();
} // namespace DataLoader
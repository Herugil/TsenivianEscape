#include "core/GameSession.h"
#include "core/GameStateManager.h"
#include "dataLoading/parseJson.h"
#include "gameObjects/creatures/Player.h"
#include <memory>
#include <unordered_map>

int main() {
  GameSession gameSession{
      std::make_shared<Player>(Point(2, 1), "placeHolder", 10)};
  std::unordered_map<std::string, std::shared_ptr<Item>> items{
      DataLoader::getAllItems()};
  std::unordered_map<std::string, std::shared_ptr<NonPlayableCharacter>> npcs{
      DataLoader::getAllNpcs()};
  gameSession.getPlayer().takeItem(items["itemSword"]->clone());
  DataLoader::populateGameSession(items, npcs, gameSession);
  GameStateManager gameStateManager{std::move(gameSession)};
  gameStateManager.mainLoop();
  return 0;
}

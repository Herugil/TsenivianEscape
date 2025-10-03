#include "core/GameSession.h"
#include "core/GameStateManager.h"
#include "dataLoading/parseJson.h"
#include "gameObjects/creatures/Player.h"
#include <memory>

int main() {
  GameSession gameSession{
      std::make_shared<Player>(Point(2, 1), "placeHolder", 10, "")};
  GameStateManager gameStateManager{std::move(gameSession)};
  gameStateManager.mainLoop();
  return 0;
}

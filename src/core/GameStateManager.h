#pragma once
#include "core/GameSession.h"
#include "core/GameState.h"
#include "gameObjects/items/Item.h"
#include <sstream>

class GameObject;
struct InteractionResult;

class GameStateManager {

private:
  std::size_t m_inventoryPage{0};
  GameState m_currentState{GameState::MainMenu};
  GameSession m_gameSession;
  std::ostringstream m_logsToDisplay{};
  std::weak_ptr<Item> m_inspectedItem{};
  InteractionResult m_interactionResult;
  void HandleWorld();
  void handleInventory();
  void handleContainer();
  void handleDisplay();
  void handleDisplayBlocking();
  void handleActions();
  void handleCombatPlayerTurn();
  void handleCombatEnemyTurn();
  void handleItemInspect();
  void handleGameOver();
  void setCombatState();
  void handleRestMenu();
  void handleUnlockMenu();
  void handleLevelUp();
  void handleCharacterSheet();
  void confirmLevelUp(Player &player, Stat stat, Stats playerStats);
  void handleMainMenu();

public:
  GameStateManager(GameSession &&gameSession);
  void mainLoop();
};

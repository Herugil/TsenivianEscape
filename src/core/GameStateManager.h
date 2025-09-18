#pragma once
#include "core/GameSession.h"
#include "core/GameState.h"

class GameObject;
struct InteractionResult;

class GameStateManager {

private:
  std::size_t m_inventoryPage{0};
  GameState m_currentState{GameState::Exploration};
  GameSession m_gameSession;
  InteractionResult m_interactionResult;
  void handleExploration();
  void handleInventory();
  void handleContainer();
  void handleDisplay();
  void handleActions();

public:
  GameStateManager(GameSession &&gameSession);
  void mainLoop();
};

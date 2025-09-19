#pragma once

class GameObject;

enum class GameState {
  Exploration,
  Combat,
  Inventory,
  ActionMenu,
  DisplayBlocking,
  Display,
  Container,
  CombatPlayerTurn,
  CombatEnemyTurn,
  ItemInspect
};

struct InteractionResult {
  GameState nextState;
  GameObject *interactedObject; // can be nullptr if no object was interacted
};

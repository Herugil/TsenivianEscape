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
  ItemInspect,
  CharacterSheet,
  RestMenu,
  UnlockMenu,
  LevelUp,
  GameOver,
};

struct InteractionResult {
  GameState nextState;
  GameObject *interactedObject; // can be nullptr if no object was interacted
};

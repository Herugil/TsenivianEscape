#pragma once

class GameObject;

enum class GameState {
  Exploration,
  Combat,
  Inventory,
  ActionMenu,
  Display,
  Container,
};

struct InteractionResult {
  GameState nextState;
  GameObject *interactedObject; // can be nullptr if no object was interacted
};

#include "gameObjects/terrain/RestingPlace.h"
#include "core/GameState.h"
#include "gameObjects/creatures/Player.h"

RestingPlace::RestingPlace(std::string_view currentMap, Point position,
                           char symbol, std::string_view name,
                           std::string_view description)
    : GameObject{false, true, symbol, currentMap, position, name, description} {
}

InteractionResult RestingPlace::playerInteraction() {
  if (m_hasBeenUsed) {
    return InteractionResult{GameState::DisplayBlocking, this};
  } else
    return InteractionResult{GameState::RestMenu, this};
}

std::string RestingPlace::getDescription() const {
  std::string result{};
  if (m_hasBeenUsed) {
    result = m_description +
             "\n It has already been used. You cannot rest here again"; // todo:
    // inconsistencies in formatting (display blocking expects description
    // without newlines and periods)
  } else {
    result = m_description +
             "\n You can rest here for the night, and refill "
             "your health and skill charges and heal your ailments.\n";
  }
  return result;
}

std::string RestingPlace::rest(Player &player) {
  std::string result{};
  if (m_hasBeenUsed) {
    result = "This resting place has already been used. You cannot rest here "
             "again.\n";
  } else {
    player.addHealthPoints(player.getMaxHealthPoints());
    player.refillSkillCharges();
    player.removePassives();
    m_hasBeenUsed = true;
    result = "You rest for the night. Your health and skill charges are "
             "refilled, and your ailments healed.\n";
  }
  return result;
}
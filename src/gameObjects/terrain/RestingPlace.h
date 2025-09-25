#pragma once

#include "gameObjects/GameObject.h"

class Player;

class RestingPlace : public GameObject {
protected:
  bool m_hasBeenUsed{false};

public:
  RestingPlace(std::string_view currentMap, Point position, char symbol = 'Z',
               std::string_view name = "Resting place",
               std::string_view description = "A place to rest and recover.");
  InteractionResult playerInteraction() override;
  std::string getDescription() const override;
  std::string rest(Player &player);
  virtual ~RestingPlace() = default;
};
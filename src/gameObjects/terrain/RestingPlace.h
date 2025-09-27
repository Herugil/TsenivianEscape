#pragma once

#include "gameObjects/GameObject.h"

using json = nlohmann::json;

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
  void setUsed() override;

  json toJson() const override;

  virtual ~RestingPlace() = default;
};
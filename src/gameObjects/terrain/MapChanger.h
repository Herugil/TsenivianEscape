#include "gameObjects/terrain/WalkOnObject.h"
#include "map/Point.h"
#include <string>
#include <string_view>

class GameSession;
class Creature;

class MapChanger : public WalkOnObject {
protected:
  std::string m_targetMap{};
  Point m_spawningPoint{};

public:
  MapChanger(std::string_view currentMap, Point position,
             std::string_view targetMap, Point spawningPoint, char symbol = ' ',
             std::string_view name = "", std::string_view description = "",
             bool locked = false, std::string_view keyId = "");
  void activateWalkOn(std::shared_ptr<GameObject> gameObject,
                      GameSession &gameSession) override;
};
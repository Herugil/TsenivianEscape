#pragma once
#include <string_view>
#include <vector>

class GameSession;

namespace SaveManager {
void saveGame(const GameSession &gameSession);
GameSession loadGame(std::string_view filename);
GameSession newGame(std::string_view name);
std::vector<std::string> getAvailableSaves();
std::string getSaveFileName(std::string_view saveOverwrite = "");
bool deleteSave(std::string_view filename);
} // namespace SaveManager
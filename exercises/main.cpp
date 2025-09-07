/*
Things to write
point class:
	int x, int y
	getAdjacent(Direction)

game object class:
	bool isMoveable
	bool canBePassedThrough
	char symbol (for display)
	operator << (for map display)
	Point position
	void move(map, direction) moves from 1 case towards direction if adjacent point is available on map
	getters

	(for later)
	str description (for inspection)
	showDescription() (for player inspection)

wall(game object) class
	isMoveable false,
	symbol X,
	description dummy description
	canBePassedThrough false,

map class
	topLeft point,
	bottomLeft point,
	etc
	floorLayer(std vector<std vector<unique ptrs<gameobject>>>) terrain, walls, etc
	topLayer(std vector <std vector<unique ptrs<gameobject>>>) enemies, player, containers, traps, etc so they dont overwrite floor


	placeFloor(gameObject, Point) puts an object on the floor
	placeFloor(gameObject, Point1, Point2) puts copies of object on the floor in a square delimited by points (they form diagonals)
	placeTop(gameObject, Point) same thing
	getters for both layers

	operator << displays top layer if present, bottom layer otherwise, empty char if nullptr
	
player(game object) class
	symbol @
	canBePassedThrough false
	isMoveable true

input class
	handles player input (removes necessity for \n when pressing a key)
	enum commands
	Direction getDirection(commands) -> gets input returns direction
	

main 
	getCommand, getDirection, display map
*/
#ifdef _WIN32
#include <windows.h>
#endif
// Remove the unconditional #include <windows.h>

#include "Map.h"
#include "Layer.h"
#include "GameSession.h"
#include "Player.h"
#include "Point.h"
#include "Directions.h"
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include "Input.h"
#include "Settings.h"

void clearScreen() {
#ifdef _WIN32

	// Windows-specific clear screen
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole != INVALID_HANDLE_VALUE) {
		COORD coordScreen = { 0, 0 };
		DWORD cCharsWritten;
		CONSOLE_SCREEN_BUFFER_INFO csbi;

		if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
			FillConsoleOutputCharacter(hConsole, ' ',
				csbi.dwSize.X * csbi.dwSize.Y, coordScreen, &cCharsWritten);
			SetConsoleCursorPosition(hConsole, coordScreen);
		}
	}
#else
	// macOS/Linux - ANSI escape codes work fine
	std::cout << "\033[2J\033[H";
	std::cout.flush(); // Force immediate output
#endif
}

int main() {
	GameSession gameSession{ 10, 10, std::make_shared<Player>(Point(1,1)) };
	gameSession.getMap().placeWalls(Point(0, 0), Point(0,9));
	gameSession.getMap().placeWalls(Point(0, 0), Point(9, 0));
	gameSession.getMap().placeWalls(Point(9, 0), Point(9, 9));
	gameSession.getMap().placeWalls(Point(0, 9), Point(9, 9));
	gameSession.getMap().placeWalls(Point(5, 0), Point(5, 5));
	gameSession.displayMap();
	while (true) {
		if (Input::hasKeyPressed()) {
			Directions::Direction dir = Input::getDirection();
			if (dir != Directions::nbDirections) {
				gameSession.movePlayer(dir);
				clearScreen();
				gameSession.displayMap();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(Settings::g_timeSleepMS));
	}
}

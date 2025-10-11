# TsenivianEscape

[![C/C++ Multi-Platform CI](https://github.com/Herugil/TsenivianEscape/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/Herugil/TsenivianEscape/actions/workflows/c-cpp.yml)

A text-based dungeon crawler written in C++, in which you play as a deserter, trying to escape from your prison in Tséniva.

## Tech Stack

- **Language**: C++23
- **Build System**: CMake
- **Dependencies**:
  - [nlohmann/json](https://github.com/nlohmann/json) - JSON parsing
  - [Boost.Random](https://www.boost.org/doc/libs/release/libs/random/) - Random number generation
- **Testing**: Google Test
- **CI/CD**: GitHub Actions (Windows, macOS, Linux builds)

## Gameplay

```
Turn-based dungeon crawling with strategic combat:
- Explore multiple interconnected maps
- Battle enemies with varied AI behaviors
- Manage action points and resources
- Level up and customize your character through a skill tree
- Find loot and equipment to enhance your abilities
```

_(Add a screenshot or gameplay GIF here when available)_

### How to Play

- At any time during play, press 'Escape' to access the main menu.
- Press the "Help" number hotkey to view the key bindings and controls.
- All controls are available in the Help menu.

## Features

- Player movement across a grid based map.
- Combat system with turn-based mechanics, and enemy AI.
- Inventory management, with loot, equipment, consumables.
- Leveling system, basic skill tree depending on the chosen stat.
- Data driven design, using JSON to create levels, enemies and items.
- Save and load game functionality.

## Setup

### Prerequisites

- C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 2022+)
- CMake 3.20+
- nlohmann/json
- Boost (Random component)

### Building the Project

#### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake nlohmann-json3-dev libboost-random-dev

# Build
git clone https://github.com/Herugil/TsenivianEscape.git
cd TsenivianEscape
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run
./build/game
```

#### macOS

```bash
# Install dependencies
brew install nlohmann-json boost cmake

# Build
git clone https://github.com/Herugil/TsenivianEscape.git
cd TsenivianEscape
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run
./build/game
```

#### Windows

```powershell
# Install dependencies via vcpkg
vcpkg install nlohmann-json:x64-windows boost-random:x64-windows

# Build
git clone https://github.com/Herugil/TsenivianEscape.git
cd TsenivianEscape
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build

# Run
.\build\game.exe
```

**Note**: The `saves/` directory must exist one level up from the executable for save functionality to work.

### Running Tests

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build
./build/runTests
```

## Project Structure

- `src/`: Contains the source code for the game.
  - `core/`: Core game logic and menus:
    - `GameSession.h/cpp`: Manages the turn order, every npc, map, player
    - `Combat.h/cpp`: Part of the combat logic: handles player and enemy turns.
    - `GameStateManager.h/cpp`: Main game loop and state transitions. Every interaction with the user goes through here and changes the game state if needed.
    - `MenuSystem.h/cpp`: Namespace for most of the UI related functions: main menu, inventory menu, etc.
    - `SaveManager.h/cpp`: Save and load game logic, with writing and reading from JSON serialized files.
    - `Item.h/cpp`: Item representation and inventory management.
    - `Combat.h/cpp`: Combat system implementation.
    - `UserInterface.h/cpp`: Should be moved to MenuSystem, as it handles other menus as well.
  - `dataLoading/`:
    - `ParseJson.h/cpp`: JSON parsing logic for loading game data. Used when creating game session and loading saved games.
  - `gameObjects/`: Most of the classes in this directory inherit from `GameObject` which represents anything that has a position on the map.
    - `Creatures/`: Creature class, from which Player and Npc both inherit. Player handles all player specific logic, Npc handles some enemy AI logic, mostly their state
    - `Items/`: Item class, from which all items inherit. Equipment and Consumable both inherit from Item. Not GameObject, they do not have a position on the map.
    - `terrain/`: Contains child classes of GameObject that represent any object belonging to the terrain: WalkOnObjects activating traps or map changes, Walls, Resting place...
  - `input/`: Input handling logic in a namespace. All keys are bound here.
  - `map/`:
    - `Layer.h`: Template class representing a plane in the map, containing GameObjects.
    - `Map.h/cpp`: Map class, containing a floor layer of map owned GameObjects, and an top layer of GameObjects owned by game session (npcs, player).
      Also includes map related logic: movement, pathfinding, LOS related functions.
  - `scripts/`: Contains actions, passives (every specific action usable by player or npc has a class here), and a namespace for the main NPC AI implementation.
    This directory could probably be broken down as AI logic is mixed with actions and passives.
  - `utils/`: Utility classes and functions. Includes a random number generator, some necessary data structures (priority queue for A* ), and geometry utils:
    Bresenham line algorithm, distance calculations, A* pathfinding algorithm.
  - `AISettings.h`: Namespace containing settings for the NPC AI: fleeing chance, health threshold for fleeing, likelihood to buff, etc.
  - `Settings.h`: Other constants: base hit chance, xp per level, base movement and action points, etc.
  - `main.cpp`: Entry point of the application, starts the game loop.
- `data/`: Contains all game data, except specific actions that are hardcoded. All other data is easily moddable by editing the JSON files. (This folder should be organized better). Needs to be one level up from the executable.
- `saves/`: Directory where save files are stored. Needs to be one level up from the executable.
- `tests/`: Contains unit tests for various components of the game, using Google Test framework.

## Detailed functionality

- **Game Loop and State Management**: The game loop is managed by the `GameStateManager` class, which handles transitions between different game states such as exploration, combat, and menus. It ensures that the game responds appropriately to player actions and game events.
- **Combat System**: The combat system is turn-based, allowing players and NPCs to take actions based on their stats and abilities. `GameSession` manages all turn order related logic.
- **Inventory Management**: Players can manage their inventory through the `MenuSystem`, allowing them to equip items, use consumables, and view item details. They can equip items (armor and weapons), use potions, drop items to the floor.
- **Multiple Maps**: Players navigate through different maps walking on certain tiles. Each map is represented by a `Map` class instance, which contains layers for terrain and game objects.
- **Action and Passive System**: Players and NPCs can perform various actions (like attacking, using abilities) that can have a defined number of uses per rest. Some of them deal damage, allow to shove, heal, buff or debuff. Some of these create passive effects, that can apply damage over time or change stats, or both. Some of these passives have a duration and expire out of combat, while other require a rest to be removed.
- **Enemy AI**: NPCs have different AI types that enable their decision making during combat. _Support_ NPCs will prioritize buffing allies, _AgressiveMelee_ Npcs will always move towards the player and attack in melee range, _AgressiveRanged_ NPCs will try to keep their distance and attack from range. _WaryMelee_ and _WaryRanged_ NPCs will try to flee if their health drops below a certain threshold. _Boss_ NPCs have more complex behavior, using a mix of melee and ranged attacks, and healing and buffing themselves.
- **Leveling System**: Players gain experience points (XP) from defeating enemies and completing objectives. Upon leveling up, players can choose to increase one of their stats (Strength, Dexterity, Constitution, Intelligence, ), which affects their combat effectiveness and abilities and gives them new actions depending on their chosen stat.
- **Locked objects**: Some objects in the game are locked and require a key to be opened. Once opened, they remain open for the rest of the game. These include doors to change maps, chests containing loot, and other interactable objects.
- **Save and Load Functionality**: Every class that needs to be saved has a `toJson` method that serializes its data to JSON format. The `SaveManager` class handles writing this data to files and reading it back when loading a game.
- **Tests**: The project includes a few unit tests using the Google Test framework.

## Future Improvements

- More comprehensive tests covering all major components of the game.
- More content and progression for the player: more levels, actions, enemies, items.
- More enemy types and behaviors.
- Some structural improvements: better organization of the scripts directory, the data directory, breaking down large classes into smaller, more manageable components.
- Dialogue and quest system could be added.
- Graphics and sound could be implemented, using either SFML or 3D graphics with OpenGL.

## About the project

This project was developed as a learning exercise to apply C++ programming skills in game development. This was built from scratch in September 2025, after going through
learncpp.com.

## Main inspirations

- Dungeons and Dragons 5e for introducing me to TTRPGs and some mechanics.
- PF2e for the multiple actions per turn and some mechanics.
- Divinity Original Sin 2, Baldur's Gate 3 for turn based combat and enemy AI, and Divinity Original Sin board game for some of the combat mechanics.

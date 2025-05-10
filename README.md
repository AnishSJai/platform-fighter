# Platform Fighter Game

A simple 2D platform fighter game built with C++ and SDL2, inspired by games like Super Smash Bros and Brawlhalla.

## Features

- Player movement with jumping and platform collision
- Basic attack system with cooldowns
- Enemy AI that patrols platforms
- Health system for enemies
- Platform-based level design

## Requirements

- C++17 or later
- CMake 3.10 or later
- SDL2

## Building the Project

1. Clone the repository:
```bash
git clone https://github.com/yourusername/platform-fighter.git
cd platform-fighter
```

2. Create a build directory and build the project:
```bash
mkdir build
cd build
cmake ..
make
```

3. Run the game:
```bash
./game
```

## Controls

- Left/Right Arrow Keys: Move
- Space: Jump
- Z: Attack

## Project Structure

- `src/`: Source files
  - `main.cpp`: Entry point
  - `Game.cpp`: Main game loop and state management
  - `Player.cpp`: Player character implementation
  - `Enemy.cpp`: Enemy AI implementation
  - `Platform.cpp`: Platform implementation
- `include/`: Header files
  - `Game.h`: Game class declaration
  - `Player.h`: Player class declaration
  - `Enemy.h`: Enemy class declaration
  - `Platform.h`: Platform class declaration

## License

This project is licensed under the MIT License - see the LICENSE file for details.

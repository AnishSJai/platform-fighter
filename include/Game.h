#pragma once
#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <vector>
#include "Player.h"
#include "Platform.h"

class Game {
public:
    Game();
    ~Game();

    // Initialize the game
    bool init(const std::string& title, int width, int height);
    
    // Main game loop
    void run();
    
    // Clean up
    void cleanup();

private:
    // Game window
    SDL_Window* window;
    
    // Game renderer
    SDL_Renderer* renderer;
    
    // Game state
    bool isRunning;
    
    // Player
    std::unique_ptr<Player> player;
    
    // Platforms
    std::vector<Platform> platforms;
    
    // Handle events
    void handleEvents();
    
    // Update game state
    void update();
    
    // Render the game
    void render();
};

#include "Game.h"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr), isRunning(false) {}

Game::~Game() {
    cleanup();
}

bool Game::init(const std::string& title, int width, int height) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create window
    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create platforms
    platforms.push_back(Platform(0, 550, 800, 50));  // Ground
    platforms.push_back(Platform(100, 400, 200, 20)); // Platform 1
    platforms.push_back(Platform(400, 300, 200, 20)); // Platform 2
    platforms.push_back(Platform(200, 200, 200, 20)); // Platform 3

    // Create player
    player = std::make_unique<Player>(width/2, height/2);

    // Create enemy on Platform 1
    enemy = std::make_unique<Enemy>(150, 350, 100, 300);

    // Set initial render color (black)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    isRunning = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
    }
    
    // Get keyboard state
    const Uint8* state = SDL_GetKeyboardState(NULL);
    player->handleInput(state);
}

void Game::update() {
    player->update(platforms);
    if (enemy->isAlive()) {
        enemy->update(platforms);
        
        // Check if player's attack hits enemy
        if (player->isAttacking) {
            if (enemy->isHit(player->getAttackRect())) {
                std::cout << "Enemy hit! Health: " << enemy->isAlive() << std::endl;
            }
        }
    }
}

void Game::render() {
    // Clear screen with black color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw platforms
    for (const auto& platform : platforms) {
        platform.render(renderer);
    }

    // Draw enemy if alive
    if (enemy->isAlive()) {
        enemy->render(renderer);
    }

    // Draw player
    player->render(renderer);

    // Update screen
    SDL_RenderPresent(renderer);
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        update();
        render();
        
        // Cap the frame rate
        SDL_Delay(16); // Approximately 60 FPS
    }
}

void Game::cleanup() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "Platform.h"

class Enemy {
public:
    Enemy(int x, int y);
    void update(const std::vector<Platform>& platforms);
    void render(SDL_Renderer* renderer);
    bool isHit(const SDL_Rect& attackRect);
    bool isAlive() const { return health > 0; }

private:
    static constexpr int WIDTH = 40;
    static constexpr int HEIGHT = 60;
    static constexpr float MOVE_SPEED = 2.0f;
    static constexpr float GRAVITY = 0.8f;

    float x, y;
    float velocityX = MOVE_SPEED;
    float velocityY = 0.0f;
    int health = 3;  // Enemy takes 3 hits to defeat
    bool isGrounded = false;

    // Platform tracking
    const Platform* currentPlatform = nullptr;
    int leftBound = 0;
    int rightBound = 0;

    // Collision detection
    bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) const;
    void resolveCollision(const Platform& platform);
    void updateMovementBounds(const Platform& platform);
    bool isOnPlatform(const Platform& platform) const;
}; 
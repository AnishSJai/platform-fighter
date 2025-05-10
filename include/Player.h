#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "Platform.h"

class Player {
public:
    Player(int x, int y);
    void update(const std::vector<Platform>& platforms);
    void render(SDL_Renderer* renderer);
    void handleInput(const Uint8* state);
    SDL_Rect getRect() const { return {static_cast<int>(x), static_cast<int>(y), WIDTH, HEIGHT}; }
    SDL_Rect getAttackRect() const;

    bool isAttacking = false;

private:
    // Position
    float x, y;
    // Velocity
    float velX, velY;
    // Physics constants
    const float GRAVITY = 0.8f;
    const float JUMP_FORCE = -15.0f;
    const float MOVE_SPEED = 5.0f;
    // Size
    const int WIDTH = 50;
    const int HEIGHT = 50;
    // State
    bool isJumping;
    bool isFacingRight;
    
    // Attack properties
    int attackFrame;
    const int ATTACK_DURATION = 10; // frames
    const int ATTACK_RANGE = 60;    // pixels
    const int ATTACK_COOLDOWN = 20; // frames between attacks
    int attackCooldownTimer = 0;    // current cooldown timer
    bool canAttack = true;          // whether player can attack

    // Collision detection
    bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) const;
    void resolveCollision(const Platform& platform);
}; 
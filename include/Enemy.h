#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "Platform.h"
#include "Player.h"

class Enemy {
public:
    Enemy(int x, int y);
    void update(const std::vector<Platform>& platforms, const Player* player);
    void render(SDL_Renderer* renderer);
    bool isHit(const SDL_Rect& attackRect);
    bool isAlive() const { return health > 0; }
    bool isAttacking = false;
    SDL_Rect getAttackRect() const;

private:
    static constexpr int WIDTH = 40;
    static constexpr int HEIGHT = 60;
    static constexpr float MOVE_SPEED = 2.0f;
    static constexpr float GRAVITY = 0.8f;
    static constexpr float DETECTION_RANGE = 300.0f;  // Range at which enemy notices player
    static constexpr float ATTACK_RANGE = 60.0f;      // Range at which enemy can attack
    static constexpr int ATTACK_COOLDOWN = 60;        // Frames between attacks
    static constexpr int ATTACK_DURATION = 20;        // Frames for attack animation

    float x, y;
    float velocityX = MOVE_SPEED;
    float velocityY = 0.0f;
    int health = 5;  // Enemy takes 5 hits to defeat
    bool isGrounded = false;
    bool isFacingRight = true;

    // AI state
    enum class AIState {
        PATROL,     // Moving back and forth on platform
        CHASE,      // Pursuing the player
        ATTACK,     // Performing an attack
        RETREAT     // Moving away from player when low on health
    };
    AIState currentState = AIState::PATROL;
    int attackCooldownTimer = 0;
    int attackFrame = 0;

    // Platform tracking
    const Platform* currentPlatform = nullptr;
    int leftBound = 0;
    int rightBound = 0;

    // AI methods
    void updateAI(const Player* player);
    void patrol();
    void chase(const Player* player);
    void attack(const Player* player);
    void retreat(const Player* player);
    bool canSeePlayer(const Player* player) const;
    bool isInAttackRange(const Player* player) const;
    void findPathToPlayer(const Player* player, const std::vector<Platform>& platforms);

    // Collision detection
    bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) const;
    void resolveCollision(const Platform& platform);
    void updateMovementBounds(const Platform& platform);
    bool isOnPlatform(const Platform& platform) const;
}; 
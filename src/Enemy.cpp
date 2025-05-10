#include "Enemy.h"
#include <iostream>
#include <cmath>

Enemy::Enemy(int x, int y) : x(x), y(y) {}

void Enemy::update(const std::vector<Platform>& platforms, const Player* player) {
    // Apply gravity
    velocityY += GRAVITY;
    y += velocityY;

    // Store current position for collision checks
    float oldX = x;
    float oldY = y;

    // Update AI state and behavior
    updateAI(player);

    // Move horizontally based on AI decision
    x += velocityX;

    // Check platform collisions
    isGrounded = false;
    SDL_Rect enemyRect = {
        static_cast<int>(x),
        static_cast<int>(y),
        WIDTH,
        HEIGHT
    };

    // Find the platform we're standing on
    const Platform* newPlatform = nullptr;
    for (const auto& platform : platforms) {
        if (checkCollision(enemyRect, platform.getRect())) {
            resolveCollision(platform);
            if (isGrounded) {
                newPlatform = &platform;
            }
        }
    }

    // Update current platform and movement bounds if we're on a new platform
    if (newPlatform != currentPlatform) {
        currentPlatform = newPlatform;
        if (currentPlatform) {
            updateMovementBounds(*currentPlatform);
        }
    }

    // Screen boundary checks
    if (x < 0) {
        x = 0;
        velocityX = MOVE_SPEED;
    } else if (x + WIDTH > 800) {  // Assuming screen width is 800
        x = 800 - WIDTH;
        velocityX = -MOVE_SPEED;
    }

    // Update attack cooldown
    if (attackCooldownTimer > 0) {
        attackCooldownTimer--;
    }

    // Update attack animation
    if (isAttacking) {
        attackFrame++;
        if (attackFrame >= ATTACK_DURATION) {
            isAttacking = false;
            attackFrame = 0;
        }
    }
}

void Enemy::updateAI(const Player* player) {
    if (!player) return;

    // Calculate distance to player
    float dx = player->getRect().x - x;
    float dy = player->getRect().y - y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Update facing direction
    isFacingRight = dx > 0;

    // State machine logic
    switch (currentState) {
        case AIState::PATROL:
            if (canSeePlayer(player)) {
                if (health <= 2) {
                    currentState = AIState::RETREAT;
                } else if (isInAttackRange(player)) {
                    currentState = AIState::ATTACK;
                } else {
                    currentState = AIState::CHASE;
                }
            } else {
                patrol();
            }
            break;

        case AIState::CHASE:
            if (!canSeePlayer(player)) {
                currentState = AIState::PATROL;
            } else if (isInAttackRange(player)) {
                currentState = AIState::ATTACK;
            } else {
                chase(player);
            }
            break;

        case AIState::ATTACK:
            if (!isInAttackRange(player)) {
                currentState = AIState::CHASE;
            } else {
                attack(player);
            }
            break;

        case AIState::RETREAT:
            if (health > 2) {
                currentState = AIState::PATROL;
            } else {
                retreat(player);
            }
            break;
    }
}

void Enemy::patrol() {
    // Basic patrol behavior - move back and forth on platform
    if (currentPlatform) {
        if (x <= leftBound) {
            x = leftBound;
            velocityX = MOVE_SPEED;
        } else if (x + WIDTH >= rightBound) {
            x = rightBound - WIDTH;
            velocityX = -MOVE_SPEED;
        }
    }
}

void Enemy::chase(const Player* player) {
    // Move towards player
    float dx = player->getRect().x - x;
    velocityX = (dx > 0) ? MOVE_SPEED : -MOVE_SPEED;

    // Jump if player is above and we're on the ground
    if (isGrounded && player->getRect().y < y - 50) {
        velocityY = -15.0f;  // Jump force
    }
}

void Enemy::attack(const Player* player) {
    // Stop moving when attacking
    velocityX = 0;

    // Perform attack if cooldown is ready
    if (attackCooldownTimer <= 0 && !isAttacking) {
        isAttacking = true;
        attackFrame = 0;
        attackCooldownTimer = ATTACK_COOLDOWN;
    }
}

void Enemy::retreat(const Player* player) {
    // Move away from player
    float dx = player->getRect().x - x;
    velocityX = (dx > 0) ? -MOVE_SPEED : MOVE_SPEED;

    // Jump to escape if player is close
    if (isGrounded && std::abs(dx) < 100) {
        velocityY = -15.0f;
    }
}

bool Enemy::canSeePlayer(const Player* player) const {
    if (!player) return false;

    float dx = player->getRect().x - x;
    float dy = player->getRect().y - y;
    float distance = std::sqrt(dx * dx + dy * dy);

    return distance <= DETECTION_RANGE;
}

bool Enemy::isInAttackRange(const Player* player) const {
    if (!player) return false;

    float dx = player->getRect().x - x;
    float dy = player->getRect().y - y;
    float distance = std::sqrt(dx * dx + dy * dy);

    return distance <= ATTACK_RANGE;
}

SDL_Rect Enemy::getAttackRect() const {
    if (!isAttacking || attackFrame >= ATTACK_DURATION / 2) {
        return {0, 0, 0, 0};  // Return empty rect if not attacking or in recovery
    }

    int attackX = isFacingRight ? x + WIDTH : x - static_cast<int>(ATTACK_RANGE);
    return {
        attackX,
        static_cast<int>(y),
        static_cast<int>(ATTACK_RANGE),
        HEIGHT
    };
}

void Enemy::render(SDL_Renderer* renderer) {
    // Draw enemy
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red color for enemy
    SDL_Rect rect = {
        static_cast<int>(x),
        static_cast<int>(y),
        WIDTH,
        HEIGHT
    };
    SDL_RenderFillRect(renderer, &rect);

    // Draw attack hitbox if attacking
    if (isAttacking) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Yellow color for attack
        SDL_Rect attackRect = getAttackRect();
        SDL_RenderFillRect(renderer, &attackRect);
    }

    // Draw health bar background
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);  // Gray color for background
    SDL_Rect healthBarBg = {
        static_cast<int>(x),
        static_cast<int>(y - 10),
        WIDTH,
        5
    };
    SDL_RenderFillRect(renderer, &healthBarBg);

    // Draw health bar
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green color for health
    SDL_Rect healthBar = {
        static_cast<int>(x),
        static_cast<int>(y - 10),
        static_cast<int>((WIDTH * health) / 5),  // Scale based on current health
        5
    };
    SDL_RenderFillRect(renderer, &healthBar);
}

bool Enemy::isHit(const SDL_Rect& attackRect) {
    SDL_Rect enemyRect = {
        static_cast<int>(x),
        static_cast<int>(y),
        WIDTH,
        HEIGHT
    };

    // Check for collision between attack and enemy
    if (SDL_HasIntersection(&attackRect, &enemyRect)) {
        std::cout << "Enemy hit! Health before: " << health;
        health--;
        std::cout << ", Health after: " << health << std::endl;
        return true;
    }
    return false;
}

bool Enemy::checkCollision(const SDL_Rect& a, const SDL_Rect& b) const {
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

void Enemy::resolveCollision(const Platform& platform) {
    SDL_Rect enemyRect = {
        static_cast<int>(x),
        static_cast<int>(y),
        WIDTH,
        HEIGHT
    };
    SDL_Rect platformRect = platform.getRect();

    // Calculate overlap on each axis
    int overlapX = std::min(enemyRect.x + enemyRect.w - platformRect.x,
                           platformRect.x + platformRect.w - enemyRect.x);
    int overlapY = std::min(enemyRect.y + enemyRect.h - platformRect.y,
                           platformRect.y + platformRect.h - enemyRect.y);

    // Resolve collision based on smallest overlap
    if (overlapX < overlapY) {
        if (enemyRect.x < platformRect.x) {
            x = platformRect.x - enemyRect.w;
        } else {
            x = platformRect.x + platformRect.w;
        }
        velocityX = -velocityX;  // Reverse direction when hitting platform sides
    } else {
        if (enemyRect.y < platformRect.y) {
            y = platformRect.y - enemyRect.h;
            velocityY = 0;
        } else {
            y = platformRect.y + platformRect.h;
            velocityY = 0;
            isGrounded = true;
        }
    }
}

void Enemy::updateMovementBounds(const Platform& platform) {
    SDL_Rect platformRect = platform.getRect();
    // Set bounds to platform edges with a small margin
    leftBound = platformRect.x + 5;  // Small margin from left edge
    rightBound = platformRect.x + platformRect.w - 5;  // Small margin from right edge
}

bool Enemy::isOnPlatform(const Platform& platform) const {
    SDL_Rect enemyRect = {
        static_cast<int>(x),
        static_cast<int>(y),
        WIDTH,
        HEIGHT
    };
    SDL_Rect platformRect = platform.getRect();
    
    // Check if enemy is standing on top of the platform
    return (enemyRect.y + enemyRect.h <= platformRect.y + 5 &&  // Within 5 pixels of platform top
            enemyRect.y + enemyRect.h >= platformRect.y - 5 &&
            enemyRect.x + enemyRect.w > platformRect.x &&
            enemyRect.x < platformRect.x + platformRect.w);
} 
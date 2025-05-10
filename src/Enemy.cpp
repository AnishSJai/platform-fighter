#include "Enemy.h"

Enemy::Enemy(int x, int y) : x(x), y(y) {}

void Enemy::update(const std::vector<Platform>& platforms) {
    // Apply gravity
    velocityY += GRAVITY;
    y += velocityY;

    // Store current position for collision checks
    float oldX = x;
    float oldY = y;

    // Move horizontally
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

    // Check if we're about to walk off the platform
    if (currentPlatform) {
        SDL_Rect platformRect = currentPlatform->getRect();
        bool willFallOff = false;

        // Check if next step would be off the platform
        SDL_Rect nextStepRect = {
            static_cast<int>(x + velocityX),  // Look ahead in movement direction
            static_cast<int>(y + 1),          // Check just below feet
            1,                                // Just check a single pixel
            1
        };

        // Check if the next step would be over empty space
        if (!checkCollision(nextStepRect, platformRect)) {
            willFallOff = true;
        }

        // If about to fall off, change direction
        if (willFallOff) {
            velocityX = -velocityX;
            x = oldX;  // Revert to previous position
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

    // Change direction when reaching bounds
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

void Enemy::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red color for enemy
    SDL_Rect rect = {
        static_cast<int>(x),
        static_cast<int>(y),
        WIDTH,
        HEIGHT
    };
    SDL_RenderFillRect(renderer, &rect);
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
        health--;
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
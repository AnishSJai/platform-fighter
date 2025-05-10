#include "Enemy.h"

Enemy::Enemy(int x, int y, int leftBound, int rightBound)
    : x(x), y(y), leftBound(leftBound), rightBound(rightBound) {}

void Enemy::update(const std::vector<Platform>& platforms) {
    // Apply gravity
    velocityY += GRAVITY;
    y += velocityY;

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

    for (const auto& platform : platforms) {
        if (checkCollision(enemyRect, platform.getRect())) {
            resolveCollision(platform);
        }
    }

    // Change direction when reaching bounds
    if (x <= leftBound) {
        x = leftBound;
        velocityX = MOVE_SPEED;
    } else if (x + WIDTH >= rightBound) {
        x = rightBound - WIDTH;
        velocityX = -MOVE_SPEED;
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
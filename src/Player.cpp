#include "Player.h"
#include <iostream>

Player::Player(int x, int y) 
    : x(x), y(y), velX(0), velY(0), isJumping(false), 
      isFacingRight(true), isAttacking(false), attackFrame(0) {}

void Player::update(const std::vector<Platform>& platforms) {
    // Store old position
    float oldX = x;
    float oldY = y;
    
    // Apply gravity
    velY += GRAVITY;
    
    // Update position
    x += velX;
    y += velY;
    
    // Update attack state
    if (isAttacking) {
        attackFrame++;
        if (attackFrame >= ATTACK_DURATION) {
            isAttacking = false;
            attackFrame = 0;
        }
    }
    
    // Get player rect
    SDL_Rect playerRect = getRect();
    
    // Check collisions with platforms
    isJumping = true; // Assume we're jumping until we hit a platform
    
    // Continuous collision detection
    for (const auto& platform : platforms) {
        SDL_Rect platformRect = platform.getRect();
        
        // Check if we're moving down and might collide with platform
        if (velY > 0) {
            // Create a rectangle representing the path of movement
            SDL_Rect pathRect = {
                static_cast<int>(oldX),
                static_cast<int>(oldY),
                WIDTH,
                static_cast<int>(y - oldY + HEIGHT)
            };
            
            // If the path intersects with the platform
            if (checkCollision(pathRect, platformRect)) {
                // We're above the platform
                y = platformRect.y - HEIGHT;
                velY = 0;
                isJumping = false;
            }
        }
        // Check if we're moving up and might collide with platform
        else if (velY < 0) {
            SDL_Rect pathRect = {
                static_cast<int>(oldX),
                static_cast<int>(y),
                WIDTH,
                static_cast<int>(oldY - y + HEIGHT)
            };
            
            if (checkCollision(pathRect, platformRect)) {
                y = platformRect.y + platformRect.h;
                velY = 0;
            }
        }
        
        // Horizontal collision checks
        if (velX != 0) {
            SDL_Rect pathRect = {
                static_cast<int>(velX > 0 ? oldX : x),
                static_cast<int>(y),
                static_cast<int>(std::abs(x - oldX) + WIDTH),
                HEIGHT
            };
            
            if (checkCollision(pathRect, platformRect)) {
                if (velX > 0) {
                    x = platformRect.x - WIDTH;
                } else {
                    x = platformRect.x + platformRect.w;
                }
                velX = 0;
            }
        }
    }
    
    // Screen boundaries
    if (x < 0) x = 0;
    if (x > 750) x = 750; // 800 - WIDTH
}

void Player::render(SDL_Renderer* renderer) {
    // Draw player
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), WIDTH, HEIGHT};
    SDL_RenderFillRect(renderer, &rect);
    
    // Draw attack hitbox if attacking
    if (isAttacking) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow for attack
        SDL_Rect attackRect = getAttackRect();
        SDL_RenderFillRect(renderer, &attackRect);
    }
}

void Player::handleInput(const Uint8* state) {
    // Reset horizontal velocity
    velX = 0;
    
    // Left/Right movement
    if (state[SDL_SCANCODE_LEFT]) {
        velX = -MOVE_SPEED;
        isFacingRight = false;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        velX = MOVE_SPEED;
        isFacingRight = true;
    }
    
    // Jump
    if (state[SDL_SCANCODE_SPACE] && !isJumping) {
        velY = JUMP_FORCE;
        isJumping = true;
    }
    
    // Attack
    if (state[SDL_SCANCODE_A] && !isAttacking) {
        isAttacking = true;
        attackFrame = 0;
    }
}

SDL_Rect Player::getAttackRect() const {
    int attackX = isFacingRight ? x + WIDTH : x - ATTACK_RANGE;
    return {
        attackX,
        static_cast<int>(y + HEIGHT/4), // Attack from middle of player
        ATTACK_RANGE,
        HEIGHT/2
    };
}

bool Player::checkCollision(const SDL_Rect& a, const SDL_Rect& b) const {
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

void Player::resolveCollision(const Platform& platform) {
    // This function is no longer used, but kept for reference
} 
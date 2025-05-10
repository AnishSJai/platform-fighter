#include "Player.h"
#include <iostream>

Player::Player(int x, int y) 
    : x(x), y(y), velX(0), velY(0), isJumping(false), 
      isFacingRight(true), attackFrame(0) {}

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

    // Update cooldown
    if (!canAttack) {
        attackCooldownTimer--;
        if (attackCooldownTimer <= 0) {
            canAttack = true;
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
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green color for player
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), WIDTH, HEIGHT};
    SDL_RenderFillRect(renderer, &rect);
    
    // Draw attack hitbox if attacking
    if (isAttacking) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Yellow color for attack
        SDL_Rect attackRect = getAttackRect();
        SDL_RenderFillRect(renderer, &attackRect);
    }

    // Draw cooldown indicator
    if (!canAttack) {
        // Calculate cooldown progress (0.0 to 1.0)
        float cooldownProgress = static_cast<float>(attackCooldownTimer) / ATTACK_COOLDOWN;
        
        // Draw a small bar above the player
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red color for cooldown
        SDL_Rect cooldownBar = {
            static_cast<int>(x),
            static_cast<int>(y - 10),
            static_cast<int>(WIDTH * cooldownProgress),
            3
        };
        SDL_RenderFillRect(renderer, &cooldownBar);
    }
}

void Player::handleInput(const Uint8* state) {
    // Movement
    if (state[SDL_SCANCODE_LEFT]) {
        velX = -MOVE_SPEED;
        isFacingRight = false;
    } else if (state[SDL_SCANCODE_RIGHT]) {
        velX = MOVE_SPEED;
        isFacingRight = true;
    } else {
        velX = 0;
    }

    // Jumping
    if (state[SDL_SCANCODE_SPACE] && !isJumping) {
        velY = JUMP_FORCE;
        isJumping = true;
    }

    // Attacking
    if (state[SDL_SCANCODE_Z] && canAttack && !isAttacking) {
        isAttacking = true;
        attackFrame = 0;
        canAttack = false;
        attackCooldownTimer = ATTACK_COOLDOWN;
    }
}

SDL_Rect Player::getAttackRect() const {
    // Only return attack rect during the first half of the attack animation
    if (attackFrame >= ATTACK_DURATION / 2) {
        return {0, 0, 0, 0};  // Return empty rect if attack is in recovery
    }

    int attackX = isFacingRight ? x + WIDTH : x - ATTACK_RANGE;
    return {
        attackX,
        static_cast<int>(y),
        ATTACK_RANGE,
        HEIGHT
    };
}

bool Player::checkCollision(const SDL_Rect& a, const SDL_Rect& b) const {
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

void Player::resolveCollision(const Platform& platform) {
    SDL_Rect playerRect = getRect();
    SDL_Rect platformRect = platform.getRect();

    // Calculate overlap on each axis
    int overlapX = std::min(playerRect.x + playerRect.w - platformRect.x,
                           platformRect.x + platformRect.w - playerRect.x);
    int overlapY = std::min(playerRect.y + playerRect.h - platformRect.y,
                           platformRect.y + platformRect.h - playerRect.y);

    // Resolve collision based on smallest overlap
    if (overlapX < overlapY) {
        if (playerRect.x < platformRect.x) {
            x = platformRect.x - playerRect.w;
        } else {
            x = platformRect.x + platformRect.w;
        }
        velX = 0;
    } else {
        if (playerRect.y < platformRect.y) {
            y = platformRect.y - playerRect.h;
            velY = 0;
        } else {
            y = platformRect.y + platformRect.h;
            velY = 0;
            isJumping = false;
        }
    }
} 
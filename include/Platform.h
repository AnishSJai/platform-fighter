#pragma once
#include <SDL2/SDL.h>

class Platform {
public:
    Platform(int x, int y, int width, int height);
    void render(SDL_Renderer* renderer) const;
    SDL_Rect getRect() const { return rect; }

private:
    SDL_Rect rect;
}; 
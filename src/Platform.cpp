#include "Platform.h"

Platform::Platform(int x, int y, int width, int height) {
    rect = {x, y, width, height};
}

void Platform::render(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &rect);
} 
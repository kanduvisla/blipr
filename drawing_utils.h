#ifndef DRAWING_UTILS_H
#define DRAWING_UTILS_H

#include <SDL.h>

SDL_Color adjustColorBrightness(SDL_Color color, float percent);
void setColor(SDL_Color color);

#endif
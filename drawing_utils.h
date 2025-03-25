#ifndef DRAWING_UTILS_H
#define DRAWING_UTILS_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

SDL_Color adjustColorBrightness(SDL_Color color, float percent);
void setColor(SDL_Color color);

#ifdef __cplusplus
}
#endif

#endif
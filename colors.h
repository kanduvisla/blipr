#ifndef COLORS_H
#define COLORS_H

#include <SDL.h>

extern SDL_Color iconColors[8];

extern SDL_Color COLOR_RED;
extern SDL_Color COLOR_DARK_RED;
extern SDL_Color COLOR_GRAY;
extern SDL_Color COLOR_LIGHT_GRAY;
extern SDL_Color COLOR_WHITE;
extern SDL_Color COLOR_BLACK;
extern SDL_Color COLOR_YELLOW;
extern SDL_Color COLOR_ORANGE;

#ifdef __cplusplus
extern "C" {
#endif

SDL_Color mixColors(SDL_Color color1, SDL_Color color2, float weight);

#ifdef __cplusplus
}
#endif

#endif
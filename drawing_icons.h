#ifndef DRAWING_ICON_H
#define DRAWING_ICON_H

#include <SDL.h>

typedef enum {
    BLIPR_ICON_UKNOWN = 0,
    BLIPR_ICON_CONFIG = 1
} Blipr_Icon;

void drawIcon(int startX, int startY, Blipr_Icon icon, SDL_Color color);

#endif
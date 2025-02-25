#ifndef DRAWING_ICON_H
#define DRAWING_ICON_H

#include <SDL.h>

typedef enum {
    BLIPR_ICON_EMPTY = 0,
    BLIPR_ICON_UKNOWN = 1,
    BLIPR_ICON_CONFIG = 2,
    BLIPR_ICON_MIDI = 3,
    BLIPR_ICON_SEQUENCER = 4,
    BLIPR_ICON_FOOT_DOWN = 5,
    BLIPR_ICON_FOOT_UP = 6,
    BLIPR_ICON_CROSS = 7,
} Blipr_Icon;

void drawIcon(
    int startX, 
    int startY, 
    Blipr_Icon icon
);

void drawIconOnIndex(
    int index, 
    Blipr_Icon icon
);

#endif
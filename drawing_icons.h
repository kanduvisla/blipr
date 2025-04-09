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
    BLIPR_ICON_KICK = 8,
    BLIPR_ICON_CHAT_1 = 9,
    BLIPR_ICON_CHAT_2 = 10,
    BLIPR_ICON_OHAT = 11,
    BLIPR_ICON_RIDE = 12,
    BLIPR_ICON_CRASH = 13,
    BLIPR_ICON_PLAY = 14,
    BLIPR_ICON_STOP = 15
} Blipr_Icon;

#ifdef __cplusplus
extern "C" {
#endif

void drawIcon(
    int startX, 
    int startY, 
    Blipr_Icon icon
);

void drawIconOnIndex(
    int index, 
    Blipr_Icon icon
);

#ifdef __cplusplus
}
#endif

#endif
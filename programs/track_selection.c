#include <SDL.h>
#include <stdbool.h>
#include "../project.h"
#include "../constants.h"
#include "../colors.h"
#include "../drawing.h"
#include "../drawing_components.h"

void updateTrackSelection(int *selectedTrack, SDL_Scancode key) {
    switch(key) {
        case BLIPR_KEY_1:
           *selectedTrack = 0;
            break;
        case BLIPR_KEY_2:
            *selectedTrack = 1;
            break;
        case BLIPR_KEY_3:
            *selectedTrack = 2;
            break;
        case BLIPR_KEY_4:
            *selectedTrack = 3;
            break;
        case BLIPR_KEY_5:
            *selectedTrack = 4;
            break;
        case BLIPR_KEY_6:
            *selectedTrack = 5;
            break;
        case BLIPR_KEY_7:
            *selectedTrack = 6;
            break;
        case BLIPR_KEY_8:
            *selectedTrack = 7;
            break;
        case BLIPR_KEY_9:
           *selectedTrack = 8;
            break;
        case BLIPR_KEY_10:
            *selectedTrack = 9;
            break;
        case BLIPR_KEY_11:
            *selectedTrack = 10;
            break;
        case BLIPR_KEY_12:
            *selectedTrack = 11;
            break;
        case BLIPR_KEY_13:
            *selectedTrack = 12;
            break;
        case BLIPR_KEY_14:
            *selectedTrack = 13;
            break;
        case BLIPR_KEY_15:
            *selectedTrack = 14;
            break;
        case BLIPR_KEY_16:
            *selectedTrack = 15;
            break;
    }
}

void drawTrackSelection(int *selectedTrack) {
    drawHighlightedGridTile(*selectedTrack);
}
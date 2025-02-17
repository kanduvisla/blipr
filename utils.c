#include <SDL.h>
#include "utils.h"
#include "constants.h"

/**
 * Convert a scancode to a step index
 */
int scancodeToStep(SDL_Scancode key) {
    switch(key) {
        case BLIPR_KEY_1:
            return 0;
        case BLIPR_KEY_2:
            return 1;
        case BLIPR_KEY_3:
            return 2;
        case BLIPR_KEY_4:
            return 3;
        case BLIPR_KEY_5:
            return 4;
        case BLIPR_KEY_6:
            return 5;
        case BLIPR_KEY_7:
            return 6;
        case BLIPR_KEY_8:
            return 7;
        case BLIPR_KEY_9:
            return 8;
        case BLIPR_KEY_10:
            return 9;
        case BLIPR_KEY_11:
            return 10;
        case BLIPR_KEY_12:
            return 11;
        case BLIPR_KEY_13:
            return 12;
        case BLIPR_KEY_14:
            return 13;
        case BLIPR_KEY_15:
            return 14;
        case BLIPR_KEY_16:
            return 15;
        default:
            return -1;
    }
};

#include <SDL.h>
#include "drawing_icons.h"
#include "drawing.h"

#define ICON_WIDTH 9
#define ICON_HEIGHT 9

const int icons[][ICON_WIDTH][ICON_HEIGHT] = {
    // A
    {{1,1,1,1,1,1,1,1,1},
     {1,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,0,1},
     {1,1,1,1,1,1,1,1,1}},
};

void drawIcon(int startX, int startY, Blipr_Icon icon, SDL_Color color) {
    // Draw the character
    for (int y = 0; y < ICON_WIDTH; y++) {
        for (int x = 0; x < ICON_HEIGHT; x++) {
            if (icons[icon][y][x] == 1) {
                drawPixel(startX + x, startY + y, color);
            }
        }
    }
}
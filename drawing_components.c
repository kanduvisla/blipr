#include <SDL.h>
#include "globals.h"
#include "drawing_utils.h"
#include "drawing.h"
#include "colors.h"

/**
 * Draw a simple button
 */
void drawButton(int x, int y, int width, int height) {
    drawBeveledRect(x, y, width, height, COLOR_LIGHT_GRAY);
}

/**
 * Draw sequencer button
 */
void drawSequencerButton(int x, int y, int width, int height, bool isActive) {
    drawBeveledRect(x, y, width, height, COLOR_LIGHT_GRAY);
    drawBeveledRectOutline(x + (width / 2) - 3, y + height - 5, 6, 3, COLOR_LIGHT_GRAY, true);
    SDL_Color indicatorColor = adjustColorBrightness(COLOR_RED, isActive ? 0 : -0.75f);
    drawLine(
        x + (width / 2) - 2, 
        y + height - 4,
        x + (width / 2) + 1, 
        y + height - 4,
        indicatorColor
    );
}

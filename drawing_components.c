#include <SDL.h>
#include "globals.h"
#include "drawing_utils.h"
#include "drawing.h"
#include "colors.h"
#include "constants.h"

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

/**
 * Draw the BPM Blinker
 */
void drawBPMBlinker(int noteCounter, int ppqnCounter) {
    float p = (float)((noteCounter % 4) * PPQN + ppqnCounter) / (float)(PPQN * 4);
    int r = 255 * (1-p);

    SDL_Color colorBlinker = {r, 0, 0, 255};
    drawRectOutline(0, 0, WIDTH, HEIGHT, GRID_UNIT, colorBlinker);
}

/**
 * Draw the Note Counter
 */
void drawNoteCounter(int noteCounter) {
    // 16 dots on the bottom for the steps:
    for (int i = 0; i < PATTERN_LENGTH; ++i) {
        drawPixel(
            2 + (i * 2),
            HEIGHT - (GRID_UNIT * 3),
            noteCounter == i ? COLOR_RED : COLOR_GRAY
        );
    }            
}

/**
 * Draw the Page Counter
 */
void drawPageCounter(int pageCounter) {
    // 4 dots on the bottom right to indicate page:
    for (int i = 0; i < 4; ++i) {
        drawPixel(
            WIDTH - 9 + (i * 2),
            HEIGHT - (GRID_UNIT * 3),
            pageCounter == i ? COLOR_RED : COLOR_GRAY
        );
    }
}

/**
 * Draw a basic 4x6 grid
 */
void drawBasicGrid() {
    int size = 10;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++) {
            drawRect(
                2 + (i * size),
                2 + (j * size),
                size,
                GRID_UNIT,
                COLOR_GRAY
            );
        }
    }
}
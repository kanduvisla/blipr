#include <SDL.h>
#include "globals.h"
#include "drawing_utils.h"
#include "drawing_text.h"
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
void drawBasicGrid(bool keyStates[SDL_NUM_SCANCODES]) {
    int width = HEIGHT / 6;

    // 16-Pad
    for (int j = 0; j < 4; j++) {
        int height = width;
        for (int i = 0; i < 4; i++) {
            drawSingleLineRectOutline(
                2 + i + (i * width),
                2 + j + (j * height),
                width,
                height,
                COLOR_GRAY
            );
        }
    }

    char characters[] = {'A', 'B', 'C', 'D'};
    char bottomButtonsDescriptions[4][3] = {"^1", "^2", "^3", "FN"};

    // Bottom Pad
    for (int j = 0; j < 2; j++) {
        int height = width / 2;
        for (int i = 0; i < 4; i++) {
            // Draw from bottom up
            int x = 2 + i + (i * width);
            int y = HEIGHT - 2 - j - ((j + 1) * height);

            if (
                (j == 0 && i == 0 && keyStates[BLIPR_KEY_SHIFT_1]) ||
                (j == 0 && i == 1 && keyStates[BLIPR_KEY_SHIFT_2]) ||
                (j == 0 && i == 2 && keyStates[BLIPR_KEY_SHIFT_3]) ||
                (j == 0 && i == 3 && keyStates[BLIPR_KEY_FUNC]) ||
                (j == 1 && i == 0 && keyStates[BLIPR_KEY_A]) ||
                (j == 1 && i == 1 && keyStates[BLIPR_KEY_B]) ||
                (j == 1 && i == 2 && keyStates[BLIPR_KEY_C]) ||
                (j == 1 && i == 3 && keyStates[BLIPR_KEY_D])
            ) {
                drawRect(x, y, width, height, COLOR_GRAY);
                drawSingleLineRectOutline(x, y, width, height, COLOR_LIGHT_GRAY);
            } else {
                drawSingleLineRectOutline(x, y, width, height, COLOR_GRAY);
            }

            if (j == 0) {
                drawText(x + 10, y + 5, bottomButtonsDescriptions[i], width, COLOR_WHITE);
            }

            if (j == 1) {
                drawCharacter(x + 13, y + 5, characters[i], COLOR_WHITE);
            }
        }
    }
}

void drawHighlightedGridTile(int tileIndex) {
    int width = HEIGHT / 6;
    int height = width;

    int x = tileIndex % 4;
    int y = tileIndex / 4;

    if (tileIndex < 16) {
        drawSingleLineRectOutline(
            2 + x + (x * width),
            2 + y + (y * height),
            width,
            height,
            COLOR_WHITE
        );
    } else {
        // Bottom buttons:
        height = width / 2;
        drawSingleLineRectOutline(
            2 + x + (x * width),
            (HEIGHT - 7 - (height * 2)) + y + ((y-4) * height),
            width,
            height,
            COLOR_WHITE
        );
    }
}

void drawBasicNumbers() {
    // Draw numbers 1-16:
    int width = HEIGHT / 6;

    // 16-Pad
    for (int j = 0; j < 4; j++) {
        int height = width;
        for (int i = 0; i < 4; i++) {
            char str[3];
            int result = (j * 4) + i;
            sprintf(str, "%d", result);

            drawText(
                (result < 10 ? 15 : 11) + i + (i * width),
                14 + j + (j * height),
                str,
                11,
                COLOR_WHITE
            );
        }
    }
}

void drawSingleNumber(int index) {
    int width = HEIGHT / 6;
    int height = width;

    int x = index % 4;
    int y = index / 4;

    char str[3];
    int result = (y * 4) + x;
    sprintf(str, "%d", result);

    drawText(
        (result < 10 ? 15 : 11) + x + (x * width),
        14 + y + (y * height),
        str,
        11,
        COLOR_WHITE
    );
}
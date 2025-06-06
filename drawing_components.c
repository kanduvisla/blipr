#include <SDL.h>
#include "globals.h"
#include "drawing_utils.h"
#include "drawing_text.h"
#include "drawing.h"
#include "colors.h"
#include "constants.h"
#include "print.h"

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
void drawBPMBlinker(uint64_t *ppqnCounter) {
    float p = (float)(*ppqnCounter % (PPQN_MULTIPLIED)) / (float)(PPQN_MULTIPLIED);
    int r = 255 * (1-p);

    SDL_Color colorBlinker = {r, 0, 0, 255};
    drawRectOutline(0, 0, WIDTH, HEIGHT, GRID_UNIT, colorBlinker);
}

/**
 * Draw the sidebar
 */
void drawSideBar() {
    drawRect(SIDEBAR_OFFSET, 2, SIDEBAR_WIDTH, SIDEBAR_HEIGHT, COLOR_GRAY);
}

/**
 * Draw the current track indicator
 */
void drawCurrentTrackIndicator(int sequenceNr, int patternNr, int trackNr) {
    drawRect(SIDEBAR_OFFSET + 1, 3, SIDEBAR_WIDTH - 2, CHAR_HEIGHT + 2, COLOR_BLACK);
    drawText(SIDEBAR_OFFSET + 2, 4, "S", 11, COLOR_RED);
    int offset = 7;
    char text[3];
    sprintf(text, "%02d", sequenceNr);
    drawText(offset + SIDEBAR_OFFSET + 2, 4, text, 11, COLOR_ORANGE);
    drawText(offset + SIDEBAR_OFFSET + 12, 4, ":", 11, COLOR_RED);
    sprintf(text, "%02d", patternNr);
    drawText(offset + SIDEBAR_OFFSET + 17, 4, text, 11, COLOR_ORANGE);
    drawText(offset + SIDEBAR_OFFSET + 27, 4, ":", 11, COLOR_RED);
    sprintf(text, "%02d", trackNr);
    drawText(offset + SIDEBAR_OFFSET + 32, 4, text, 11, COLOR_ORANGE);
}

/**
 * Sidebar template
 */
void drawSidebarTemplate(int y, const char *text) {
    drawRect(SIDEBAR_OFFSET + 1, y, SIDEBAR_WIDTH - 2, CHAR_HEIGHT + 2, COLOR_BLACK);
    drawText(SIDEBAR_OFFSET + 2, y + 1, text, 20, COLOR_RED);
    drawText(SIDEBAR_OFFSET + 19, y + 1, ":", 11, COLOR_RED);
}

/**
 * Draw the BPM indicator
 */
void drawBPMIndiciator(int bpm) {
    drawSidebarTemplate(11, "BPM");
    char text[4];
    sprintf(text, "%d", bpm);
    drawText(SIDEBAR_OFFSET + 24, 12, text, 20, COLOR_ORANGE);
}

/**
 * Draw pattern length indicator
 */
void drawPatternLengthIndicator(int current, int total) {
    drawSidebarTemplate(19, "LEN");
    char text[8];
    sprintf(text, "%d/%d", current + 1, total + 1);
    float p = (float)current / (float)total;
    drawRect(SIDEBAR_OFFSET + 24, 20, (SIDEBAR_WIDTH - 26) * p, 5, COLOR_RED);
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
                (j == 0 && i == 3 && keyStates[BLIPR_KEY_FUNC])/* ||
                (j == 1 && i == 0 && keyStates[BLIPR_KEY_A]) ||
                (j == 1 && i == 1 && keyStates[BLIPR_KEY_B]) ||
                (j == 1 && i == 2 && keyStates[BLIPR_KEY_C]) ||
                (j == 1 && i == 3 && keyStates[BLIPR_KEY_D])*/
            ) {
                drawRect(x, y, width, height, COLOR_GRAY);
                drawSingleLineRectOutline(x, y, width, height, COLOR_LIGHT_GRAY);
            } else {
                drawSingleLineRectOutline(x, y, width, height, COLOR_GRAY);
            }

            if (j == 0) {
                drawText(x + 10, y + 5, bottomButtonsDescriptions[i], width, COLOR_WHITE);
            }

            /*
            if (j == 1) {
                drawCharacter(x + 13, y + 5, characters[i], COLOR_WHITE);
            }
            */
        }
    }
}

void drawABCDButtons(const char descriptions[4][4]) {
    int width = WIDTH / 6;
    int height = width / 2;

    for (int i = 0; i < 4; i++) {
        int x = 3 + i + (i * width);
        int y = HEIGHT - 3 - (2 * height);
        drawCenteredLine(x, y + 5, descriptions[i], width, COLOR_WHITE);
    }
}

void drawABCDButtonsInColor(const char descriptions[4][4], SDL_Color color) {
    int width = WIDTH / 6;
    int height = width / 2;

    for (int i = 0; i < 4; i++) {
        int x = 3 + i + (i * width);
        int y = HEIGHT - 3 - (2 * height);
        drawCenteredLine(x, y + 5, descriptions[i], width, color);
    }
}

void drawHighlightedGridTileInColor(int tileIndex, SDL_Color color) {
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
            color
        );
    } else {
        // Bottom buttons:
        height = width / 2;
        drawSingleLineRectOutline(
            2 + x + (x * width),
            (HEIGHT - 7 - (height * 2)) + y + ((y-4) * height),
            width,
            height,
            color
        );
    }
}

void drawHighlightedGridTile(int tileIndex) {
    drawHighlightedGridTileInColor(tileIndex, COLOR_WHITE);
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

void drawTextOnButton(int index, const char* text) {
    int width = HEIGHT / 6;
    int height = width;

    int x = index % 4;
    int y = index / 4;

    drawText(
        14 + x + (x * width) - ((strlen(text) / 2) * 5),
        14 + y + (y * height),
        text,
        11,
        COLOR_WHITE
    );
}

/**
 * Draw default increase- and decrease buttons
 */
void drawIncreaseAndDecreaseButtons(
    int firstButtonIndex,
    const char *header,
    const char *text
) {
    int y = (firstButtonIndex / 4);
    drawCenteredLine(
        2 + (BUTTON_WIDTH * (firstButtonIndex % 4)) + (firstButtonIndex % 4), 
        5 + (BUTTON_HEIGHT * (firstButtonIndex / 4)) + (firstButtonIndex / 4), 
        header, 
        BUTTON_WIDTH * 2, 
        COLOR_WHITE
    );
    drawTextOnButton(firstButtonIndex, "<");
    drawTextOnButton(firstButtonIndex + 1, ">");
    drawCenteredLine(
        2 + (BUTTON_WIDTH * (firstButtonIndex % 4)) + (firstButtonIndex % 4), 
        14 + (BUTTON_HEIGHT * (firstButtonIndex / 4)) + (firstButtonIndex / 4),
        text, 
        BUTTON_WIDTH * 2, 
        COLOR_WHITE
    );
}

/**
 * Draw a rotating button (a button that updates it's value every time it's pressed)
 */
void drawRotatingButton(
    int index,
    const char *header,
    const char *value
) {
    drawCenteredLine(
        2 + (index % 4) + (BUTTON_WIDTH * (index % 4)), 
        BUTTON_HEIGHT + 7 + (index / 4), 
        header, 
        BUTTON_WIDTH, 
        COLOR_WHITE
    );
    drawTextOnButton(index, value);
}

/**
 * Draw a dimmed, pixelated overlay
 */
void drawDimmedOverlay(int x, int y, int width, int height) {
    for (int yPos=0; yPos<height; yPos++) {
        for (int xPos=0; xPos<width; xPos+=2) {
            drawPixel(x + xPos + (yPos % 2), y + yPos, COLOR_BLACK);
        }
    }
}

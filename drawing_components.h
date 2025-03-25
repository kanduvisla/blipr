#ifndef DRAWING_COMPONENTS_H
#define DRAWING_COMPONENTS_H

#include <stdbool.h>
#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

void drawButton(int x, int y, int width, int height);
void drawSequencerButton(int x, int y, int width, int height, bool isActive);
void drawBPMBlinker(uint64_t *ppqnCounter);

/**
 * Draw the sidebar
 */
void drawSideBar();

/**
 * Sidebar template
 */
void drawSidebarTemplate(int y, const char *text);

/**
 * Draw the current track indicator
 */
void drawCurrentTrackIndicator(int sequenceNr, int patternNr, int trackNr);

/**
 * Draw the BPM indicator
 */
void drawBPMIndiciator(int bpm);

/**
 * Draw pattern indicator
 */
void drawPatternLengthIndicator(int current, int total);

/**
 * Draw a basic 4x6 grid
 */
void drawBasicGrid(bool keyStates[SDL_NUM_SCANCODES]);

/**
 * Draw a hightlighted grid tile in a given color (zero-based index)
 */
void drawHighlightedGridTileInColor(int tileIndex, SDL_Color color);

/**
 * Draw a hightlighted grid tile (zero-based index)
 */
void drawHighlightedGridTile(int tileIndex);

/**
 * Draw the numbers 1-16
 */
void drawBasicNumbers();

/**
 * Draw a single number
 */
void drawSingleNumber(int index);

/**
 * Draw text on a button
 */
void drawTextOnButton(int index, const char* text);

/**
 * Draw ABCD buttons
 */
void drawABCDButtons(const char descriptions[4][4]);

/**
 * Draw ABCD buttons in color
 */
void drawABCDButtonsInColor(const char descriptions[4][4], SDL_Color color);

/**
 * Draw increase & decrease buttons
 */
void drawIncreaseAndDecreaseButtons(int firstButtonIndex, const char *header, const char *text);

/**
 * Draw a rotating button (a button that updates it's value every time it's pressed)
 */
void drawRotatingButton(int index, const char *header, const char *value);

/**
 * Draw a dimmed overlay
 */
void drawDimmedOverlay(int x, int y, int width, int height);

#ifdef __cplusplus
}
#endif

#endif
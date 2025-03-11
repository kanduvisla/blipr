#ifndef DRAWING_COMPONENTS_H
#define DRAWING_COMPONENTS_H

#include <stdbool.h>
#include <SDL.h>

void drawButton(int x, int y, int width, int height);
void drawSequencerButton(int x, int y, int width, int height, bool isActive);
void drawBPMBlinker(uint64_t *ppqnCounter);

/**
 * Draw the sidebar
 */
void drawSideBar();

/**
 * Draw the current track indicator
 */
void drawCurrentTrackIndicator(int sequenceNr, int patternNr, int trackNr);

/**
 * Draw the BPM indicator
 */
void drawBPMIndiciator(int bpm);

/**
 * Draw a basic 4x6 grid
 */
void drawBasicGrid(bool keyStates[SDL_NUM_SCANCODES]);

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
void drawTextOnButton(int index, char* text);

/**
 * Draw ABCD buttons
 */
void drawABCDButtons(char descriptions[4][3]);
void drawABCDButtonsInColor(char descriptions[4][4], SDL_Color color);

/**
 * Draw increase & decrease buttons
 */
void drawIncreaseAndDecreaseButtons(int firstButtonIndex, const char *header, const char *text);

#endif
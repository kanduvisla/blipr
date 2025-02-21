#ifndef DRAWING_COMPONENTS_H
#define DRAWING_COMPONENTS_H

#include <stdbool.h>
#include <SDL.h>

void drawButton(int x, int y, int width, int height);
void drawSequencerButton(int x, int y, int width, int height, bool isActive);
void drawBPMBlinker(int *ppqnCounter);
void drawNoteCounter(int noteCounter);
void drawPageCounter(int pageCounter);

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

#endif
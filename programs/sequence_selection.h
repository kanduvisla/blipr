#ifndef SEQUENCE_SELECTION_H
#define SEQUENCE_SELECTION_H

#include <SDL.h>

/**
 * Update the sequence selection according to user input
 */
void updateSequenceSelection(int *selectedSequence, SDL_Scancode key);

/**
 * Draw the sequence selection
 */
void drawSequenceSelection(int *selectedSequence);

#endif
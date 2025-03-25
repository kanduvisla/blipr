#ifndef SEQUENCE_SELECTION_H
#define SEQUENCE_SELECTION_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Update the sequence selection according to user input
 */
void updateSequenceSelection(int *selectedSequence, SDL_Scancode key);

/**
 * Draw the sequence selection
 */
void drawSequenceSelection(const int *selectedSequence);

#ifdef __cplusplus
}
#endif

#endif
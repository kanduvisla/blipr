#ifndef PATTERN_SELECTION_H
#define PATTERN_SELECTION_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Update the pattern selection according to user input
 */
void updatePatternSelection(int *selectedPattern, SDL_Scancode key);

/**
 * Draw the pattern selection
 */
void drawPatternSelection(const int *selectedPattern, const int *queuedPattern);

#ifdef __cplusplus
}
#endif

#endif
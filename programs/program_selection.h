#ifndef PROGRAM_SELECTION_H
#define PROGRAM_SELECTION_H

#include <SDL.h>
#include "../project.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Draw the program selection
 */
void drawProgramSelection(struct Track *track);

/**
 * Process a key during program selection
 */
void updateProgram(struct Track *track, SDL_Scancode key);

#ifdef __cplusplus
}
#endif

#endif
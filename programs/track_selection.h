#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <SDL.h>
#include <stdbool.h>
#include "../project.h"

/**
 * Update the track selection according to user input
 */
void updateTrackSelection(int *selectedTrack, SDL_Scancode key);

/**
 * Draw the track selection
 */
void drawTrackSelection(int *selectedTrack);

#endif
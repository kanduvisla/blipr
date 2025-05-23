#ifndef TRACK_SELECTION_H
#define TRACK_SELECTION_H

#include <SDL.h>

/**
 * Update the track selection according to user input
 */
void updateTrackSelection(int *selectedTrack, SDL_Scancode key);

/**
 * Draw the track selection
 */
void drawTrackSelection(int *selectedTrack);

#endif
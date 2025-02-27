#ifndef TRACK_OPTIONS_H
#define TRACK_OPTIONS_H

#include <SDL.h>
#include "../project.h"

void drawTrackOptions(struct Track* track);
void updateTrackOptions(struct Track* track, SDL_Scancode key);

/**
 * Check if we need to do some key repeat actions
 */
void checkTrackOptionsForKeyRepeats(
    struct Track *selectedTrack,
    bool keyStates[SDL_NUM_SCANCODES]
);

#endif
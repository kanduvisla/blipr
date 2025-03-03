#ifndef FOUR_ON_THE_FLOOR_H
#define FOUR_ON_THE_FLOOR_H

#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include "../project.h"

/**
 * Update FOTF according to user input
 */
// void updateFourOnTheFloor(
//     struct Project *project, 
//     bool keyStates[SDL_NUM_SCANCODES], 
//     SDL_Scancode key, 
//     int selectedSequence, 
//     int selectedPatern, 
//     int selectedTrack
// );

/**
 * Run FOTF
 */
void runFourOnTheFloor(
    PmStream *outputStream,
    uint64_t *ppqnCounter, 
    struct Track *selectedTrack
);

/**
 * Draw FOTF
 */
void drawFourOnTheFloor(
    uint64_t *ppqnCounter, 
    struct Track *track
);

#endif
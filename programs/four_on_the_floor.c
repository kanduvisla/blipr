#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include "../constants.h"
#include "../project.h"

/**
 * Run FOTF
 */
void runFourOnTheFloor(
    PmStream *outputStream,
    struct Project *project,
    int *ppqnCounter, 
    int selectedSequence, 
    int selectedPatern,
    struct Track *selectedTrack
) {
    // Very basic program, just send a note every beat (whole note)
    if (*ppqnCounter % (PPQN_MULTIPLIED * 4) == 0) {

    }
}

/**
 * Draw FOTF
 */
void drawFourOnTheFloor(
    int *ppqnCounter, 
    struct Track *track
) {
    // TODO, draw foot, stomping on the floor
    if (*ppqnCounter % (PPQN_MULTIPLIED * 4) == 0) {
        
    }
}

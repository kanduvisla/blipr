#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include "../constants.h"
#include "../drawing_icons.h"
#include "../project.h"
#include "../midi.h"

static bool isNotePlaying = false;

/**
 * Run FOTF
 */
void runFourOnTheFloor(
    PmStream *outputStream,
    int *ppqnCounter, 
    struct Track *selectedTrack
) {
    // Very basic program, just send a note every beat (whole note)
    if (*ppqnCounter % (PPQN_MULTIPLIED) == 0) {
        sendMidiNoteOn(outputStream, selectedTrack->midiChannel, 60, 100);
        isNotePlaying = true;
    } else if(isNotePlaying) {
        sendMidiNoteOff(outputStream, selectedTrack->midiChannel, 60);
        isNotePlaying = false;
    }
}

/**
 * Draw FOTF
 */
void drawFourOnTheFloor(
    int *ppqnCounter, 
    struct Track *track
) {
    // Draw foot, stomping on the floor
    drawIcon(
        55, 
        55, 
        *ppqnCounter % (PPQN_MULTIPLIED) == 0 ? BLIPR_ICON_FOOT_DOWN : BLIPR_ICON_FOOT_UP
    );
}

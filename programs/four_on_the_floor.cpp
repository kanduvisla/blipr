#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include "../constants.h"
#include "../drawing_icons.h"
#include "../project.h"
#include "../midi.h"
#include "four_on_the_floor.hpp"

// static bool isNotePlaying = false;

// Constructor implementation
FourOnTheFloor::FourOnTheFloor() {
    // Initialization code if needed
}

// Destructor implementation
FourOnTheFloor::~FourOnTheFloor() {
    // Cleanup code if needed
}

/**
 * Update according to user input
 */
void FourOnTheFloor::update(struct Track *selectedTrack, bool keyStates[SDL_NUM_SCANCODES], SDL_Scancode key) {
    // TODO
}

// void FourOnTheFloor::reset() {
//     // TODO
// }

/**
 * Run FOTF
 */
void FourOnTheFloor::run(PmStream *outputStream, const uint64_t *ppqnCounter, struct Track *selectedTrack) {
    // Very basic program, just send a note every beat (whole note)
    if (*ppqnCounter % (PPQN_MULTIPLIED) == 0) {
        // Set proper stream + track:
        tmpStream = outputStream;
        tmpTrack = selectedTrack;

        // Send a note:
        struct Note note;
        note.note = 40;
        note.velocity = 100;
        note.length = PP16N;
        playNote(&note);
    }
}

/**
 * Draw FOTF
 */
void FourOnTheFloor::draw(uint64_t *ppqnCounter, bool keyStates[SDL_NUM_SCANCODES], struct Track *track) {
    // Draw foot, stomping on the floor
    drawIcon(
        55, 
        55, 
        *ppqnCounter % (PPQN_MULTIPLIED) == 0 ? BLIPR_ICON_FOOT_DOWN : BLIPR_ICON_FOOT_UP
    );
}

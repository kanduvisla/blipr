#include "drumkit_sequencer.hpp"
#include "../project.h"

/**
 * Constructor 
 */
DrumkitSequencer::DrumkitSequencer() {}

/** 
 * Destructor
 */
DrumkitSequencer::~DrumkitSequencer() {}

/**
 * Toggle a step
 */
void DrumkitSequencer::toggleStep(struct Step *step, int noteIndex) {
    if (step->notes[noteIndex].enabled) {
        // Decrease velocity:
        if (step->notes[noteIndex].velocity > 100) {
            step->notes[noteIndex].velocity = 100;
        } else if (step->notes[noteIndex].velocity > 50) {
            step->notes[noteIndex].velocity = 50;
        } else if (step->notes[noteIndex].velocity > 25) {
            step->notes[noteIndex].velocity = 25;
        } else {
            step->notes[noteIndex].enabled = false;    
        }
    } else {
        step->notes[noteIndex].enabled = true;
        // Get proper note according to drumkit configuration
        // Use template note
        
        // copyNote(&templateNote, &step->notes[noteIndex]);
        // Reset enabled state, because template note might be a disabled note :-/
        step->notes[noteIndex].enabled = true;
    }
}

/**
 * Handle key input when shift 2 is down
 */
void handleKeyWithShift2Down(struct Track *track, int index) {
    // Also set selected note
    int polyCount = getPolyCount(track);
    templateNote.velocity = 100;
    switch(index) {
        case 0:
            // Kick
            selectedNote = 0;
            templateNote.note = 36;
            break;
        case 1:
            // Snare
            selectedNote = 1;
            templateNote.note = 40;
            break;
        case 2:
            // Clap
            selectedNote = polyCount == 8 ? 2 : 1;
            templateNote.note = 39;
            break;
        case 3:
            // Rimshot
            selectedNote = 1;
            templateNote.note = 37;
            break;
        case 4:
            // C.Hat 1
            selectedNote = polyCount == 8 ? 3 : (polyCount == 4 ? 2 : 1);
            templateNote.note = 42;
            break;
        case 5:
            // C.Hat 2
            selectedNote = polyCount == 8 ? 3 : (polyCount == 4 ? 2 : 1);
            templateNote.note = 44;
            break;
        case 6:
            // O.Hat
            selectedNote = polyCount == 8 ? 4 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 46;
            break;
        case 7:
            // Ride
            selectedNote = polyCount == 8 ? 4 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 51;
            break;
        case 8:
            // Crash
            selectedNote = polyCount == 8 ? 4 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 55;
            break;
        case 9:
            // L.Tom
            selectedNote = polyCount == 8 ? 5 : 1;
            templateNote.note = 45;
            break;
        case 10:
            // M.Tom
            selectedNote = polyCount == 8 ? 5 : 1;
            templateNote.note = 48;
            break;
        case 11:
            // H.Tom
            selectedNote = polyCount == 8 ? 5 : 1;
            templateNote.note = 50;
            break;
        case 12:
            // Cowbell
            selectedNote = polyCount == 8 ? 6 : 1;
            templateNote.note = 56;
            break;
        case 13:
            // Extra 1
            selectedNote = polyCount == 8 ? 6 : 0;
            templateNote.note = 71;
            break;
        case 14:
            // Extra 2
            selectedNote = polyCount == 8 ? 7 : (polyCount == 4 ? 2 : 1);
            templateNote.note = 73;
            break;
        case 15:
            // Extra 3
            selectedNote = polyCount == 8 ? 7 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 75;
            break;
        default:
            // Nothing
            break;
    }

    if (polyCount == 1) {
        selectedNote = 0;
    }   
}
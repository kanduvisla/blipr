#include <portmidi.h>
#include "../midi.h"
#include "abstract_program.hpp"

Program::Program() {
    // Constructor implementation
}

Program::~Program() {
    // Destructor implementation
}

/**
 * Play a single note
 */
void Program::playNote(const struct Note *note) {
    // Send CC:
    if (note->cc1Value > 0) {
        sendMidiMessage(tmpStream, tmpTrack->midiChannel | 0xB0, tmpTrack->cc1Assignment, note->cc1Value - 1);
    }
    if (note->cc2Value > 0) {
        sendMidiMessage(tmpStream, tmpTrack->midiChannel | 0xB0, tmpTrack->cc2Assignment, note->cc2Value - 1);
    }

    sendMidiNoteOn(tmpStream, tmpTrack->midiChannel, note->note, note->velocity);
    addNoteToTracker(tmpStream, tmpTrack->midiChannel, note);
}
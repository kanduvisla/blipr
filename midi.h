#ifndef MIDI_H
#define MIDI_H

#include <portmidi.h>
#include <porttime.h>

/**
 * Print a list of MIDI devices to the shell
 */
void listMidiDevices();

void handleMidiError(PmError error) {
    if (error != pmNoError) {
        printf("PortMidi error: %s\n", Pm_GetErrorText(error));
        exit(1);
    }
}

#endif
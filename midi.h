#ifndef MIDI_H
#define MIDI_H

#include <portmidi.h>
#include <porttime.h>


void handleMidiError(PmError error);

/**
 * Print a list of MIDI devices to the shell
 */
void listMidiDevices();

void openMidiInput(int device_id);

void openMidiOutput(int device_id);

void sendMidiMessage(int status, int data1, int data2);

void processMidiInput();

#endif
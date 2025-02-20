#ifndef MIDI_H
#define MIDI_H

#include <portmidi.h>
#include <porttime.h>

void handleMidiError(PmError error);

/**
 * Print a list of MIDI devices to the shell
 */
void listMidiDevices();

/**
 * Open device for midi input
 */
void openMidiInput(int device_id);

/**
 * Open device for midi output
 */
void openMidiOutput(int device_id);

/**
 * Send midi message
 */
void sendMidiMessage(int status, int data1, int data2);

/**
 * Process midi input
 */
void processMidiInput();

#endif
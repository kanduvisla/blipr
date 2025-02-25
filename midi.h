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
void openMidiInput(int deviceId, PmStream *inputStream);

/**
 * Open device for midi output
 */
void openMidiOutput(int deviceId, PmStream *outputStream);

/**
 * Send midi message
 */
void sendMidiMessage(PmStream *outputStream, int status, int data1, int data2);

/**
 * Send Midi Note On
 */
void sendMidiNoteOn(PmStream *outputStream, int channel, int noteNumber, int velocity);

/**
 * Send Midi Note Off
 */
void sendMidiNoteOff(PmStream *outputStream, int channel, int noteNumber);

/**
 * Process midi input
 */
void processMidiInput(PmStream *inputStream);

/**
 * Returns -1 if not device is found with the given name
 */
int getOutputDeviceIdByDeviceName(char* deviceName);

/**
 * Send Midi Clock
 */
void sendMidiClock(PortMidiStream *stream);

#endif
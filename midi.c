#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <portmidi.h>
#include <porttime.h>
#include "project.h"
#include "constants.h"

#define INPUT_BUFFER_SIZE 100
#define OUTPUT_BUFFER_SIZE 100
#define MIDI_CLOCK 0xF8
#define MAX_NOTES 512

void handleMidiError(PmError error) {
    if (error != pmNoError) {
        printf("PortMidi error: %s\n", Pm_GetErrorText(error));
        exit(1);
    }
}

void listMidiDevices() {
    int num_devices = Pm_CountDevices();
    printf("Available MIDI devices:\n");
    for (int i = 0; i < num_devices; i++) {
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
        printf("%d: %s, %s\n", i, info->name, info->input ? "Input" : "Output");
    }
}

void sendMidiClock(PmStream *outputStream) {
    PmEvent event;
    event.message = Pm_Message(MIDI_CLOCK, 0, 0);
    event.timestamp = 0; // Send immediately
    PmError error = Pm_Write(outputStream, &event, 1);
    handleMidiError(error);
}

void openMidiInput(int deviceId, PmStream **inputStream) {
    PmError error = Pm_OpenInput(inputStream, deviceId, NULL, INPUT_BUFFER_SIZE, NULL, NULL);
    handleMidiError(error);
    printf("Opened input device %d\n", deviceId);
}

void openMidiOutput(int deviceId, PmStream **outputStream) {
    PmError error = Pm_OpenOutput(outputStream, deviceId, NULL, OUTPUT_BUFFER_SIZE, NULL, NULL, 0);
    handleMidiError(error);
    printf("Opened output device %d\n", deviceId);
}

void sendMidiMessage(PmStream *outputStream, int status, int data1, int data2) {
    PmEvent event = {0};
    event.message = Pm_Message(status, data1, data2);
    PmError error = Pm_Write(outputStream, &event, 1);
    handleMidiError(error);
}

void sendMidiNoteOn(PmStream *outputStream, int channel, int noteNumber, int velocity) {
    sendMidiMessage(outputStream, channel | 0x90, noteNumber, velocity);
}

void sendMidiNoteOff(PmStream *outputStream, int channel, int noteNumber) {
    sendMidiMessage(outputStream, channel | 0x80, noteNumber, 0);
}

void processMidiInput(PmStream *inputStream) {
    PmEvent buffer[32];
    int num_events = Pm_Read(inputStream, buffer, 32);
    
    for (int i = 0; i < num_events; i++) {
        int status = Pm_MessageStatus(buffer[i].message);
        int data1 = Pm_MessageData1(buffer[i].message);
        int data2 = Pm_MessageData2(buffer[i].message);
        
        printf("Received MIDI message: status = %02X, data1 = %02X, data2 = %02X\n", status, data1, data2);
        
        // Check for MIDI clock message
        if (status == 0xF8) {
            printf("Received MIDI Clock\n");
        }
    }
}

int getOutputDeviceIdByDeviceName(char* deviceName) {
    int num_devices = Pm_CountDevices();
    for (int i = 0; i < num_devices; i++) {
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
        if (info->output) {
            if (strcmp(deviceName, info->name) == 0) {
                return i;
            }
        }
    }
    return -1;
}

/**
 * Tracker to keep track of note off calls
 */
typedef struct {
    const struct Note* note;  // Pointer to the original note
    PmStream* outputStream;   // The MIDI output stream
    int midiChannel;          // The MIDI channel
    int counter;              // Counter for pulses
    bool active;              // Whether this slot is in use
} NoteTracker;

NoteTracker activeNotes[MAX_NOTES];

void initializeNoteTracker() {
    for (int i = 0; i < MAX_NOTES; i++) {
        activeNotes[i].note = NULL;
        activeNotes[i].outputStream = NULL;
        activeNotes[i].midiChannel = 0;
        activeNotes[i].counter = 0;
        activeNotes[i].active = false;
    }
}

void addNoteToTracker(PmStream* outputStream, int midiChannel, const struct Note* note) {
    for (int i = 0; i < MAX_NOTES; i++) {
        if (!activeNotes[i].active) {
            activeNotes[i].note = note;
            activeNotes[i].outputStream = outputStream;
            activeNotes[i].midiChannel = midiChannel;
            activeNotes[i].counter = MAX(1, note->length) * 6;  // TODO: Make a proper calculation for length here.
            activeNotes[i].active = true;
            // printf("set counter: %d\n", activeNotes[i].counter);
            break;
        }
    }
}

void updateNotesAndSendOffs() {
    for (int i = 0; i < MAX_NOTES; i++) {
        if (activeNotes[i].active) {
            activeNotes[i].counter--;
            // printf("counter: %d\n", activeNotes[i].counter);
            if (activeNotes[i].counter <= 0) {
                // printf("Send note off on channel %d\n", activeNotes[i].midiChannel);
                sendMidiNoteOff(activeNotes[i].outputStream, 
                                activeNotes[i].midiChannel, 
                                activeNotes[i].note->note);
                activeNotes[i].active = false;
                activeNotes[i].note = NULL;
                activeNotes[i].outputStream = NULL;
            }
        }
    }
}

char* getMidiNoteName(unsigned char midiNote) {
    static char noteName[5];  // Static array to hold the result

    if (midiNote > 127) {
        return "Invalid";
    }

    const char* noteNames[] = {"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"};
    int octave = (midiNote / 12) - 1;
    int note = midiNote % 12;

    snprintf(noteName, sizeof(noteName), "%s%d", noteNames[note], octave);

    return noteName;
}
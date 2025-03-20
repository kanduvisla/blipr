#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <portmidi.h>
#include <porttime.h>
#include "project.h"
#include "constants.h"
#include "print.h"
#include "globals.h"

#define INPUT_BUFFER_SIZE 100
#define OUTPUT_BUFFER_SIZE 100
#define MIDI_CLOCK 0xF8
#define MAX_NOTES 512

void handleMidiError(PmError error) {
    if (error != pmNoError) {
        printError("PortMidi error: %s", Pm_GetErrorText(error));
        // exit(1);
    }
}

void listMidiDevices() {
    int num_devices = Pm_CountDevices();
    printLog("Available MIDI devices:");
    for (int i = 0; i < num_devices; i++) {
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
        printLog("%d: %s, %s", i, info->name, info->input ? "Input" : "Output");
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
    printLog("Opened input device %d", deviceId);
}

void openMidiOutput(int deviceId, PmStream **outputStream) {
    PmError error = Pm_OpenOutput(outputStream, deviceId, NULL, OUTPUT_BUFFER_SIZE, NULL, NULL, 0);
    handleMidiError(error);
    printLog("Opened output device %d", deviceId);
}

void sendMidiMessage(PmStream *outputStream, int status, int data1, int data2) {
    printLog("MIDI: 0x%X 0x%X 0x%X", status, data1, data2);
    if (outputStream == NULL) {
        // Failsafe to prevent crashing
        return;
    } else if (isMidiDataLogged) {
        // Print debug info instead, ignoring clock
        printLog("MIDI: 0x%X 0x%X 0x%X", status, data1, data2);
    }

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
    struct Note note;  // Pointer to the original note
    PmStream* outputStream;   // The MIDI output stream
    int midiChannel;          // The MIDI channel
    int counter;              // Counter for pulses
    bool active;              // Whether this slot is in use
} NoteTracker;

NoteTracker activeNotes[MAX_NOTES];

void initializeNoteTracker() {
    for (int i = 0; i < MAX_NOTES; i++) {
        activeNotes[i].outputStream = NULL;
        activeNotes[i].midiChannel = 0;
        activeNotes[i].counter = 0;
        activeNotes[i].active = false;
    }
}

void addNoteToTracker(PmStream* outputStream, int midiChannel, const struct Note* note) {
    // Create a copy of the struct, because when using a pointer the note off can be missed if the MIDI note byte is changed:
    for (int i = 0; i < MAX_NOTES; i++) {
        if (!activeNotes[i].active) {
            activeNotes[i].note = *note;
            activeNotes[i].outputStream = outputStream;
            activeNotes[i].midiChannel = midiChannel;
            activeNotes[i].counter = MAX(1, note->length);  // TODO: Make a proper calculation for length here.
            activeNotes[i].active = true;
            break;
        }
    }
}

void updateNotesAndSendOffs() {
    for (int i = 0; i < MAX_NOTES; i++) {
        if (activeNotes[i].active) {
            activeNotes[i].counter--;
            if (activeNotes[i].counter <= 0) {
                sendMidiNoteOff(activeNotes[i].outputStream, 
                                activeNotes[i].midiChannel, 
                                activeNotes[i].note.note);
                activeNotes[i].active = false;
                activeNotes[i].outputStream = NULL;
            }
        }
    }
}

char* getMidiNoteName(unsigned char midiNote) {
    static char noteName[5];  // Static array to hold the result

    if (midiNote > 127) {
        return "---";
    }

    const char* noteNames[] = {"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"};
    int octave = (midiNote / 12) - 1;
    int note = midiNote % 12;

    snprintf(noteName, sizeof(noteName), "%s%d", noteNames[note], octave);

    return noteName;
}

void sendProgramChange(PortMidiStream *stream, int channel, int program) {
    // Ensure channel is in valid range (0-15)
    channel = channel & 0x0F;
    
    // Ensure program is in valid range (0-127)
    program = program & 0x7F;
    
    // Calculate status byte: 0xC0 + channel
    int status = 0xC0 | channel;
    
    // For Program Change, the second data byte is ignored
    // but we'll set it to 0 for clarity
    sendMidiMessage(stream, status, program, 0);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <portmidi.h>
#include <porttime.h>

#define INPUT_BUFFER_SIZE 100
#define OUTPUT_BUFFER_SIZE 100
#define MIDI_CLOCK 0xF8

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

void sendMidiMessage(PmStream *outputStream, int channel, int status, int data1, int data2) {
    PmEvent event = {0};
    event.message = Pm_Message(status, data1, data2);
    PmError error = Pm_Write(outputStream, &event, 1);
    handleMidiError(error);
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

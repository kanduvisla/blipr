#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <portmidi.h>
#include <porttime.h>

#define INPUT_BUFFER_SIZE 100
#define OUTPUT_BUFFER_SIZE 100

PmStream *input_stream;
PmStream *output_stream;

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

void openMidiInput(int device_id) {
    PmError error = Pm_OpenInput(&input_stream, device_id, NULL, INPUT_BUFFER_SIZE, NULL, NULL);
    handleMidiError(error);
    printf("Opened input device %d\n", device_id);
}

void openMidiOutput(int device_id) {
    PmError error = Pm_OpenOutput(&output_stream, device_id, NULL, OUTPUT_BUFFER_SIZE, NULL, NULL, 0);
    handleMidiError(error);
    printf("Opened output device %d\n", device_id);
}

void sendMidiMessage(int status, int data1, int data2) {
    PmEvent event = {0};
    event.message = Pm_Message(status, data1, data2);
    PmError error = Pm_Write(output_stream, &event, 1);
    handleMidiError(error);
}

void processMidiInput() {
    PmEvent buffer[32];
    int num_events = Pm_Read(input_stream, buffer, 32);
    
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

/*
int main() {
    PmError error = Pm_Initialize();
    handle_error(error);

    list_devices();

    int input_device, output_device;
    printf("Enter input device number: ");
    scanf("%d", &input_device);
    printf("Enter output device number: ");
    scanf("%d", &output_device);

    open_input(input_device);
    open_output(output_device);

    printf("Listening for MIDI messages. Press Enter to send a test message, or 'q' to quit.\n");

    char input[10];
    while (1) {
        if (Pm_Poll(input_stream) == TRUE) {
            process_midi_input();
        }

        if (fgets(input, sizeof(input), stdin) != NULL) {
            if (input[0] == 'q') {
                break;
            }
            // Send a test Note On message (middle C, velocity 64)
            send_midi_message(0x90, 60, 64);
            printf("Sent test MIDI message\n");
        }
    }

    Pm_Close(input_stream);
    Pm_Close(output_stream);
    Pm_Terminate();

    return 0;
}
*/
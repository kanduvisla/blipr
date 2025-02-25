#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include "../constants.h"
#include "../drawing_icons.h"
#include "../project.h"
#include "../midi.h"

/**
 * Run FOTF
 */
void runFourOnTheFloor(
    PmStream *outputStream,
    int *ppqnCounter, 
    struct Track *selectedTrack
) {
    // Very basic program, just send a note every beat (whole note)
    if (*ppqnCounter % (PPQN_MULTIPLIED) == 0) {

        // Send midi note to stream:
        PmEvent event;
        // printf("Midi channel: %d\n", selectedTrack->midiChannel);
        int channel = selectedTrack->midiChannel & 0x0F;
        int status = 0x90; // note on
        // Combine status and channel
        int statusByte = (status & 0xF0) | channel;

        printf("Send note on - channel 0x%X\n", channel);
        printf("Status byte 0x%X\n", statusByte);

        // Note On message
        event.message = Pm_Message(statusByte, 60, 100);  // 0x90 is the status byte for Note On, 60 is C5, velocity is 0-127
        event.timestamp = 0;  // Send immediately
        Pm_Write(outputStream, &event, 1);
    }
}

/**
 * Draw FOTF
 */
void drawFourOnTheFloor(
    int *ppqnCounter, 
    struct Track *track
) {
    // Draw foot, stomping on the floor
    drawIcon(
        55, 
        55, 
        *ppqnCounter % (PPQN_MULTIPLIED) == 0 ? BLIPR_ICON_FOOT_DOWN : BLIPR_ICON_FOOT_UP
    );
}

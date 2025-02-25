#include <string.h>
#include "../project.h"
#include "../drawing_components.h"
#include "../utils.h"
#include "../constants.h"
#include "../drawing_text.h"
#include "../colors.h"

char midiDeviceToCharacter(int midiDevice) {
    switch (midiDevice) {
        case BLIPR_MIDI_DEVICE_A:
           return 'A';
        case BLIPR_MIDI_DEVICE_B:
           return 'B';
        case BLIPR_MIDI_DEVICE_C:
           return 'C';
        case BLIPR_MIDI_DEVICE_D:
           return 'D';
    }

    return ' ';
}

void drawTrackOptions(struct Track* track) {
    // Title:
    drawCenteredLine(2, 133, "TRACK OPTIONS", TITLE_WIDTH, COLOR_WHITE);

    // Track length:
    char line[32];
    snprintf(line, sizeof(line), "TRACK LENGTH:%d", track->trackLength + 1);
    drawCenteredLine(2, 22, line, TITLE_WIDTH, COLOR_WHITE);
    drawTextOnButton(0, "<<");
    drawTextOnButton(1, "<");
    drawTextOnButton(2, ">");
    drawTextOnButton(3, ">>");

    // Midi Device: 
    char deviceChar[2] = {
        midiDeviceToCharacter(track->midiDevice),
        '\0'
    };
    drawCenteredLine(2, 37, "MD", BUTTON_WIDTH, COLOR_WHITE);
    drawTextOnButton(4, deviceChar);
    
    // Midi Channel:
    char channelChar[3];
    snprintf(channelChar, sizeof(channelChar), "%d", track->midiChannel + 1);
    drawCenteredLine(32, 37, "MC", BUTTON_WIDTH, COLOR_WHITE);
    drawTextOnButton(5, channelChar);
}

void updateTrackOptions(struct Track* track, SDL_Scancode key) {
    track->polyCount = 8;

    int maxLength = (9 - track->polyCount) * 64;

    switch (key) {
        case BLIPR_KEY_1:
            track->trackLength = MAX(0, track->trackLength - 16);
            break;
        case BLIPR_KEY_2:
            track->trackLength = MAX(0, track->trackLength - 1);
            break;
        case BLIPR_KEY_3:
            track->trackLength = MIN(maxLength - 1, track->trackLength + 1);
            break;
        case BLIPR_KEY_4:
            track->trackLength = MIN(maxLength - 1, track->trackLength + 16);
            break;
        case BLIPR_KEY_5:
            track->midiDevice = track->midiDevice + 1;
            if (track->midiDevice > BLIPR_MIDI_DEVICE_D) {
                track->midiDevice = BLIPR_MIDI_DEVICE_A;
            }
            break;
        case BLIPR_KEY_6:
            track->midiChannel = track->midiChannel + 1;
            if (track->midiChannel >= 16) {
                track->midiChannel = 0;
            }
            break;
    }
}
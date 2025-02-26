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

    // Track Length:
    char trackLengthTitle[32];
    snprintf(trackLengthTitle, sizeof(trackLengthTitle), "TRACK LENGTH:%d", track->trackLength + 1);
    drawCenteredLine(2, 22, trackLengthTitle, TITLE_WIDTH, COLOR_WHITE);
    drawTextOnButton(0, "<<");
    drawTextOnButton(1, "<");
    drawTextOnButton(2, ">");
    drawTextOnButton(3, ">>");

    // Track Speed (TODO):
    drawCenteredLine(2, 37, "TS", BUTTON_WIDTH * 2, COLOR_WHITE);
    drawTextOnButton(4, "<");
    drawTextOnButton(5, ">");

    // Page Length:
    char pageLengthTitle[32];
    snprintf(pageLengthTitle, sizeof(pageLengthTitle), "PL:%d", track->pageLength + 1);
    drawCenteredLine(62, 37, pageLengthTitle, BUTTON_WIDTH * 2, COLOR_WHITE);
    drawTextOnButton(6, "<");
    drawTextOnButton(7, ">");

    // Midi Device: 
    char deviceChar[2] = {
        midiDeviceToCharacter(track->midiDevice),
        '\0'
    };
    drawCenteredLine(2, 67, "MD", BUTTON_WIDTH, COLOR_WHITE);
    drawTextOnButton(8, deviceChar);
    
    // Midi Channel:
    char channelChar[3];
    snprintf(channelChar, sizeof(channelChar), "%d", track->midiChannel + 1);
    drawCenteredLine(32, 67, "MC", BUTTON_WIDTH, COLOR_WHITE);
    drawTextOnButton(9, channelChar);

    // Page play mode:
    drawCenteredLine(62, 67, "PPM", BUTTON_WIDTH, COLOR_WHITE);
    drawTextOnButton(10, track->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS ? "C" : "R");
}

void updateTrackOptions(struct Track* track, SDL_Scancode key) {
    int maxLength = (8 - track->polyCount) * 64;

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
            track->speed = MAX(TRACK_SPEED_EIGHT, track->speed - 1);
            break;
        case BLIPR_KEY_6:
            track->speed = MIN(TRACK_SPEED_TIMES_EIGHT, track->speed + 1);
            break;
        case BLIPR_KEY_7:
            track->pageLength = MAX(0, track->pageLength - 1);
            break;
        case BLIPR_KEY_8:
            track->pageLength = MIN(15, track->pageLength + 1);
            break;
        case BLIPR_KEY_9:
            track->midiDevice = track->midiDevice + 1;
            if (track->midiDevice > BLIPR_MIDI_DEVICE_D) {
                track->midiDevice = BLIPR_MIDI_DEVICE_A;
            }
            break;
        case BLIPR_KEY_10:
            track->midiChannel = track->midiChannel + 1;
            if (track->midiChannel >= 16) {
                track->midiChannel = 0;
            }
            break;
        case BLIPR_KEY_11:
            track->pagePlayMode = track->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS ? PAGE_PLAY_MODE_REPEAT : PAGE_PLAY_MODE_CONTINUOUS;
    }
}
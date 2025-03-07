#include <string.h>
#include <stdbool.h>
#include "../project.h"
#include "../drawing_components.h"
#include "../utils.h"
#include "../constants.h"
#include "../drawing_text.h"
#include "../colors.h"
#include "../print.h"

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

    // Page play mode:
    drawCenteredLine(2, 5, "PPM", BUTTON_WIDTH, COLOR_WHITE);
    drawTextOnButton(0, track->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS ? "C" : "R");

    // Polyphony:
    drawCenteredLine(33, 5, "POLY", BUTTON_WIDTH, COLOR_WHITE);
    char polyChar[2];
    snprintf(polyChar, sizeof(polyChar), "%d", getPolyCount(track));
    drawTextOnButton(1, polyChar);

    // Track Length:
    char trackLengthTitle[32];
    if (track->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        snprintf(trackLengthTitle, sizeof(trackLengthTitle), "T.LEN:%d", track->trackLength + 1);
    } else {
        snprintf(trackLengthTitle, sizeof(trackLengthTitle), "P.LEN:%d", track->pageLength + 1);
    }
    drawCenteredLine(64, 5, trackLengthTitle, BUTTON_WIDTH * 2, COLOR_WHITE);
    drawTextOnButton(2, "<");
    drawTextOnButton(3, ">");

    // Track Speed:
    drawCenteredLine(2, 37, "SPEED", BUTTON_WIDTH * 2, COLOR_WHITE);
    drawTextOnButton(4, "<");
    drawTextOnButton(5, ">");
    char trackSpeed[4];
    switch(track->speed) {
        case TRACK_SPEED_DIV_EIGHT:
            sprintf(trackSpeed, "1/8");
            break;
        case TRACK_SPEED_DIV_FOUR:
            sprintf(trackSpeed, "1/4");
            break;
        case TRACK_SPEED_DIV_TWO:
            sprintf(trackSpeed, "1/2");
            break;
        case TRACK_SPEED_TIMES_EIGHT:
            sprintf(trackSpeed, "8");
            break;
        case TRACK_SPEED_TIMES_FOUR:
            sprintf(trackSpeed, "4");
            break;
        case TRACK_SPEED_TIMES_TWO:
            sprintf(trackSpeed, "2");
            break;
        default:
            sprintf(trackSpeed, "1");
            break;
    }
    drawCenteredLine(2, 47, trackSpeed, BUTTON_WIDTH * 2, COLOR_WHITE);

    // Shuffle:
    drawCenteredLine(64, 37, "SHUFFLE", BUTTON_WIDTH * 2, COLOR_WHITE);
    drawTextOnButton(6, "<");
    drawTextOnButton(7, ">");
    char shuffleAmount[4];
    sprintf(shuffleAmount, "%d", track->shuffle - PP16N);
    drawCenteredLine(64, 47, shuffleAmount, BUTTON_WIDTH * 2, COLOR_WHITE);

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

    // ABCD Buttons:
    char descriptions[4][4] = {"TRK", "PRG", "PAT", "UTI"};
    drawABCDButtons(descriptions);
    drawHighlightedGridTile(16);
}

void handleKey(
    struct Track *track,
    SDL_Scancode key
) {
    int polyCount = getPolyCount(track);
    int maxLength = (9 - polyCount) * 64;

    switch (key) {
        case BLIPR_KEY_3:
            if (track->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
                track->trackLength = MAX(0, track->trackLength - 1);
            } else {
                track->pageLength = MAX(0, track->pageLength - 1);
            }
            break;
        case BLIPR_KEY_4:
            if (track->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
                track->trackLength = MIN(maxLength - 1, track->trackLength + 1);
            } else {
                track->pageLength = MIN(15, track->pageLength + 1);
            }
            break;
    }
} 

/**
 * Check if we need to do some key repeat actions
 */
void checkTrackOptionsForKeyRepeats(
    struct Track *selectedTrack,
    bool keyStates[SDL_NUM_SCANCODES]
) {
    if (keyStates[BLIPR_KEY_3]) { handleKey(selectedTrack, BLIPR_KEY_3); } else 
    if (keyStates[BLIPR_KEY_4]) { handleKey(selectedTrack, BLIPR_KEY_4); }
}

/**
 * Update track options according to key input
 */
void updateTrackOptions(struct Track* track, SDL_Scancode key) {
    switch (key) {
        case BLIPR_KEY_1:
            track->pagePlayMode = track->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS ? PAGE_PLAY_MODE_REPEAT : PAGE_PLAY_MODE_CONTINUOUS;
            break;
        case BLIPR_KEY_2:
            if (track->polyCount + 1 >= 4) {
                track->polyCount = 0;
            } else {
                track->polyCount += 1;
            }
            break;
        case BLIPR_KEY_3:
        case BLIPR_KEY_4:
            handleKey(track, key);
            break;
        case BLIPR_KEY_5:
            track->speed = MAX(TRACK_SPEED_DIV_EIGHT, track->speed - 1);
            break;
        case BLIPR_KEY_6:
            track->speed = MIN(TRACK_SPEED_TIMES_EIGHT, track->speed + 1);
            break;
        case BLIPR_KEY_7:
            track->shuffle = MAX(0, track->shuffle - 1);
            break;
        case BLIPR_KEY_8:
            track->shuffle = MIN(PP16N * 2, track->shuffle + 1);
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
    }
}
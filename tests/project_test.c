#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../project.h"

bool assertByte(unsigned char value, unsigned char expected) {
    if (value == expected) {
        printf(".");
        return true;
    } else {
        printf("F\n");
        printf("Expected %d\n", expected);
        printf("Actual %d\n", value);
        return false;
    }
}

bool assert(bool value) {
    if (value) {
        printf(".");
        return true;
    } else {
        printf("F\n");
        return false;
    }
}

/// --- Step tests

void testStepToByteArray() {
    struct Step step;
    step.note = 78;
    step.velocity = 110;
    step.nudge = 0;

    unsigned char arr[STEP_BYTE_SIZE];
    stepToByteArray(&step, arr);
    
    assertByte(arr[0], 0x4E);
    assertByte(arr[1], 0x6E);
    assertByte(arr[2], 0x3F);
}

void testByteArrayToStep() {
    unsigned char arr[STEP_BYTE_SIZE] = {0x4E, 0x6E, 0x3F};
    struct Step step = byteArrayToStep(arr);

    assert(step.note == 78);
    assert(step.velocity == 110);
    assert(step.nudge == 0);
}

/// --- Track tests

void testTrackToByteArray() {
    struct Track track;
    snprintf(track.name, sizeof(track.name), "Track %d", 3);
    track.midiDevice = 4;
    track.midiChannel = 2;
    track.program = 1;
    track.pageLength = 16;
    for (int s = 0; s < 64; s++) {
        struct Step step;
        step.note = 0;
        step.velocity = 0;
        step.nudge = 0;
        track.steps[s] = step;
    }
    track.steps[42].note = 78;

    unsigned char arr[TRACK_BYTE_SIZE];
    trackToByteArray(&track, arr);

    struct Track track2 = byteArrayToTrack(arr);
    assert(strcmp(track2.name, "Track 3") == 0);
    assert(track2.midiDevice == 4);
    assert(track2.midiChannel == 2);
    assert(track2.program == 1);
    assert(track2.pageLength == 16);
    assert(track2.steps[42].note == 78);
}

/// --- Pattern tests

void testPatternToByteArray() {
    // TODO
}

/// --- Entry point

void testProjectFile() {
    testStepToByteArray();
    testByteArrayToStep();
    testTrackToByteArray();
}

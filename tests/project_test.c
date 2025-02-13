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

    unsigned char arr[16];
    stepToByteArray(&step, arr);
    
    assertByte(arr[0], 0x4E);
    assertByte(arr[1], 0x6E);
    assertByte(arr[2], 0x3F);
}

void testByteArrayToStep() {
    unsigned char arr[16] = {0x4E, 0x6E, 0x3F};
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

    unsigned char arr[1088];
    trackToByteArray(&track, arr);
}

void testProjectFile() {
    testStepToByteArray();
    testByteArrayToStep();
    testTrackToByteArray();
}

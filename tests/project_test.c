#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
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

void testProjectFile() {
    testStepToByteArray();
    testByteArrayToStep();
}

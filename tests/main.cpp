#include <stdio.h>
#include <stdbool.h>

// --- Assertion methods:

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

#include "pattern_player_test.cpp"

int main(void) {
    testPatternPlayer();

    return 0;
}
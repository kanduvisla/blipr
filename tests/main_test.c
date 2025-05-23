#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

// Globals that are required for compiling (but not used in the tests):
SDL_Renderer *renderer = NULL;
bool isMidiDataLogged = false;

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

#include "project_test.c"
#include "sequencer_test.c"

/**
 * Entry point
 */
int main(void)
{
    printf("blipr test suite\n");

    testProjectFile();
    testSequencer();

    printf("\n");
}
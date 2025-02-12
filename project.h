#ifndef PROJECT_H
#define PROJECT_H

#include <stdint.h>

/**
 * A step contains a note
 */
struct Step {
    char note[3];       // C-5, D#3, etc.
    uint8_t velocity;   // 0-127
    uint8_t nudge;      // -63 - 63
};

/**
 * A pattern contains of 64 steps
 */
struct Pattern {
    uint8_t midiDevice;
    uint8_t midiChannel;
    uint8_t program;
    struct Step steps[64];
};

/**
 * A sequence contains of 16 patterns
 */
struct Sequence {
    struct Pattern patterns[16];
};

/**
 * A Project contains of 16 sequences
 */
struct Project {
    struct Sequence sequences[16];
};

/**
 * Initialize an empty project
 */
struct Project initializeProject();

#endif
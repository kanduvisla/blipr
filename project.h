#ifndef PROJECT_H
#define PROJECT_H

#include <stdint.h>

/**
 * A step contains a note
 */
struct Step {
    unsigned char note;       // byte representation of note C-5, D#3, etc.
    unsigned char velocity;   // 0-127
    char nudge;      // -63 - 63
};

void stepToByteArray(const struct Step *step, unsigned char bytes[16]);
struct Step byteArrayToStep(const unsigned char bytes[16]);

/**
 * A tracks contains 64 steps and some metadata
 */
struct Track {
    char name[32]; // "Track 0" - "Track 15"
    uint8_t midiDevice;
    uint8_t midiChannel;
    uint8_t program;
    uint8_t pageLength;
    // Steps are only used for the "Sequencer"-program
    struct Step steps[64];
};

void trackToByteArray(const struct Track *track, uint8_t *bytes);
struct Track byteArrayToTrack(const uint8_t *bytes);

/**
 * A pattern contains 16 tracks
 */
struct Pattern {
    char name[32];
    struct Track tracks[16];
};

/**
 * A sequence contains 16 patterns
 */
struct Sequence {
    char name[32];
    struct Pattern patterns[16];
};

/**
 * A Project contains 16 sequences
 */
struct Project {
    char name[32];
    struct Sequence sequences[16];
};

/**
 * Initialize an empty project
 */
struct Project initializeProject();

#endif
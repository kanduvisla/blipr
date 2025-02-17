#ifndef PROJECT_H
#define PROJECT_H

#include <stdint.h>
#include <stdbool.h>

#define NOTE_BYTE_SIZE 8
#define STEP_BYTE_SIZE 16
#define TRACK_BYTE_SIZE (64 + (64 * STEP_BYTE_SIZE))        // 64 bytes header + 64 steps
#define PATTERN_BYTE_SIZE (64 + (16 * TRACK_BYTE_SIZE))     // 64 bytes header + 16 tracks
#define SEQUENCE_BYTE_SIZE (64 + (16 * PATTERN_BYTE_SIZE))  // 64 bytes header + 16 patterns
#define PROJECT_BYTE_SIZE (256 + (16 * SEQUENCE_BYTE_SIZE)) // 256 bytes header + 16 sequences

/**
 * A Note
 */
struct Note {
    bool enabled;               // If this note is enabled
    unsigned char note;         // byte representation of note C-5, D#3, etc.
    unsigned char velocity;     // 0-127
    char nudge;                 // -63 - 63
    unsigned char trigg;        // Trigg condition
    unsigned char length;       // Length
    unsigned char cc1Value;     // CC1 Value
    unsigned char cc2Value;     // CC2 Value
};

/**
 * A step can contains 8 notes
 */
struct Step {
    unsigned char note;         // byte representation of note C-5, D#3, etc.
    unsigned char velocity;     // 0-127
    char nudge;                 // -63 - 63
};

void stepToByteArray(const struct Step *step, unsigned char bytes[STEP_BYTE_SIZE]);
struct Step byteArrayToStep(const unsigned char bytes[STEP_BYTE_SIZE]);

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

void trackToByteArray(const struct Track *track, unsigned char bytes[TRACK_BYTE_SIZE]);
struct Track byteArrayToTrack(const unsigned char bytes[TRACK_BYTE_SIZE]);

/**
 * A pattern contains 16 tracks
 */
struct Pattern {
    char name[32];
    struct Track tracks[16];
};

void patternToByteArray(const struct Pattern *pattern, unsigned char bytes[PATTERN_BYTE_SIZE]);
struct Pattern byteArrayToPattern(const unsigned char bytes[PATTERN_BYTE_SIZE]);

/**
 * A sequence contains 16 patterns
 */
struct Sequence {
    char name[32];
    struct Pattern patterns[16];
};

void sequenceToByteArray(const struct Sequence *sequence, unsigned char bytes[SEQUENCE_BYTE_SIZE]);
struct Sequence byteArrayToSequence(const unsigned char bytes[SEQUENCE_BYTE_SIZE]);

/**
 * A Project contains 16 sequences
 */
struct Project {
    char name[32];
    struct Sequence sequences[16];
};

void projectToByteArray(const struct Project *project, unsigned char bytes[PROJECT_BYTE_SIZE]);
struct Project byteArrayToProject(const unsigned char bytes[PROJECT_BYTE_SIZE]);

/**
 * Initialize an empty project
 */
struct Project initializeProject();

#endif
#ifndef PROJECT_H
#define PROJECT_H

#include <stdint.h>
#include <stdbool.h>

#define NOTE_BYTE_SIZE 8
#define NOTES_IN_STEP 8
#define SMALL_HEADER_BYTE_SIZE 64
#define LARGE_HEADER_BYTE_SIZE 256
#define STEP_BYTE_SIZE (NOTES_IN_STEP * NOTE_BYTE_SIZE)     // n notes in a step
#define TRACK_BYTE_SIZE (SMALL_HEADER_BYTE_SIZE + (64 * STEP_BYTE_SIZE))        // header + 64 steps
#define PATTERN_BYTE_SIZE (SMALL_HEADER_BYTE_SIZE + (16 * TRACK_BYTE_SIZE))     // header + 16 tracks
#define SEQUENCE_BYTE_SIZE (SMALL_HEADER_BYTE_SIZE + (16 * PATTERN_BYTE_SIZE))  // header + 16 patterns
#define PROJECT_BYTE_SIZE (LARGE_HEADER_BYTE_SIZE + (16 * SEQUENCE_BYTE_SIZE))  // header + 16 sequences

#define PAGE_PLAY_MODE_CONTINUOUS 0 // Play the track pages after each other
#define PAGE_PLAY_MODE_REPEAT 1     // Loop the currently selected track, individually
#define PAGE_PLAY_MODE_SYNCED 1     // Loop the currently selected track, synced globally with other tracks
#define TRACK_SPEED_NORMAL 4
#define TRACK_SPEED_TIMES_TWO 5
#define TRACK_SPEED_TIMES_FOUR 6
#define TRACK_SPEED_TIMES_EIGHT 7
#define TRACK_SPEED_DIV_TWO 3
#define TRACK_SPEED_DIV_FOUR 2 
#define TRACK_SPEED_DIV_EIGHT 1

/**
 * A Note
 */
struct Note {
    bool enabled;               // If this note is enabled
    unsigned char note;         // byte representation of note C-5, D#3, etc.
    unsigned char velocity;     // 0-127          
    unsigned char nudge;        // 0 - (PP16N * 2) (where 1*PP16N equals 0 nudge)
    unsigned char trigg;        // Trigg condition
    unsigned char length;       // Length
    unsigned char cc1Value;     // CC1 Value
    unsigned char cc2Value;     // CC2 Value
};

/**
 * Structure to hold a matching note and its information
 */
typedef struct {
    struct Note *note;          // Pointer to the matching note
    uint16_t stepIndex;  // The extended step index (0-511)
    uint8_t noteIndex;   // The note index within the step (0-7)
} MatchingNote;

void noteToByteArray(const struct Note *note, unsigned char bytes[NOTE_BYTE_SIZE]);
struct Note byteArrayToNote(const unsigned char bytes[NOTE_BYTE_SIZE]);

/**
 * A step can contains 8 notes
 */
struct Step {
    struct Note notes[NOTES_IN_STEP];
};

void stepToByteArray(const struct Step *step, unsigned char bytes[STEP_BYTE_SIZE]);
struct Step byteArrayToStep(const unsigned char bytes[STEP_BYTE_SIZE]);

/**
 * A tracks contains 64 steps and some metadata
 */
struct Track {
    char name[32]; // "Track 0" - "Track 15"
    unsigned char midiDevice;
    unsigned char midiChannel;
    unsigned char program;
    unsigned char pageLength;
    short int trackLength;   // 2 bytes, since track length can be up to 512
    unsigned char cc1Assignment;
    unsigned char cc2Assignment;
    unsigned char polyCount; // 0=8, 1=4, 2=2, 3=1
    unsigned char pagePlayMode;
    unsigned char speed;
    unsigned char shuffle;
    unsigned char transitionRepeats;    // How many repeats before a transition kicks in?
    // Not saved, used internally:
    unsigned char selectedPage;
    unsigned char playingPageBank;
    unsigned char queuedPage;
    unsigned int repeatCount;
    bool isFirstPulse;
    
    // Steps are only used for the "Sequencer"-program
    struct Step steps[64];
};

void trackToByteArray(const struct Track *track, unsigned char bytes[TRACK_BYTE_SIZE]);
struct Track* byteArrayToTrack(const unsigned char bytes[TRACK_BYTE_SIZE]);

/**
 * Reset internal, locally track data (like counters, etc)
 */
void resetTrack(struct Track *track);

/**
 * A pattern contains 16 tracks
 */
struct Pattern {
    char name[32];
    unsigned char bpm;      // 0-255 accounts for 45-300 BPM (BPM = byte value + 45)
    unsigned char programA; // Midi Program for device A (0x00 - 0xFF (low = bank, high = pattern))
    unsigned char programB;
    unsigned char programC;
    unsigned char programD;
    unsigned char length;   // How many steps before a transition to take effect? (take into account page play mode)
    struct Track tracks[16];
};

void patternToByteArray(const struct Pattern *pattern, unsigned char bytes[PATTERN_BYTE_SIZE]);
struct Pattern* byteArrayToPattern(const unsigned char bytes[PATTERN_BYTE_SIZE]);

/**
 * A sequence contains 16 patterns
 */
struct Sequence {
    char name[32];
    struct Pattern patterns[16];
};

void sequenceToByteArray(const struct Sequence *sequence, unsigned char bytes[SEQUENCE_BYTE_SIZE]);
struct Sequence* byteArrayToSequence(const unsigned char bytes[SEQUENCE_BYTE_SIZE]);

/**
 * A Project contains 16 sequences
 */
struct Project {
    char name[32];
    char midiDeviceAName[32];
    char midiDeviceBName[32];
    char midiDeviceCName[32];
    char midiDeviceDName[32];
    unsigned char midiDevicePcChannelA;
    unsigned char midiDevicePcChannelB;
    unsigned char midiDevicePcChannelC;
    unsigned char midiDevicePcChannelD;
    struct Sequence sequences[16];
};

void projectToByteArray(const struct Project *project, unsigned char bytes[PROJECT_BYTE_SIZE]);
struct Project* byteArrayToProject(const unsigned char bytes[PROJECT_BYTE_SIZE]);

/**
 * Initialize an empty project
 */
void initializeProject(struct Project* project);

/**
 * Get polyphony count for a given track
 */
int getPolyCount(const struct Track* track);

#endif
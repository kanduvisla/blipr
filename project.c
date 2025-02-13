#include <stdio.h>
#include <string.h>
#include "project.h"

/**
 * Convert Step to Byte Array
 * Byte 1       : Note
 * Byte 2       : Velocity
 * Byte 3       : Nudge
 * Byte 4-...   : Spare
 */
void stepToByteArray(const struct Step *step, unsigned char bytes[STEP_BYTE_SIZE]) {
    bytes[0] = step->note;
    bytes[1] = step->velocity;
    bytes[2] = (unsigned char)(step->nudge + 63);
    memset(bytes + 3, 0, STEP_BYTE_SIZE - 3);
}

/**
 * Convert Byte Array to Step
 */
struct Step byteArrayToStep(const unsigned char bytes[STEP_BYTE_SIZE]) {
    struct Step step;
    step.note = bytes[0];
    step.velocity = bytes[1];
    step.nudge = (char)bytes[2] - 63;
    return step;
}

/**
 * Covert Track to Byte Array
 * byte 1-32    : Name
 * byte 33      : midi device
 * byte 34      : midi channel
 * byte 35      : program 
 * byte 36      : page length
 * byte 37-64   : Spare
 * byte 65-...  : Steps data (64 steps)
 */
void trackToByteArray(const struct Track *track, unsigned char bytes[TRACK_BYTE_SIZE]) {
    memcpy(bytes, track->name, 32);
    bytes[32] = track->midiDevice;
    bytes[33] = track->midiChannel;
    bytes[34] = track->program;
    bytes[35] = track->pageLength;
    memset(bytes + 36, 0, TRACK_BYTE_SIZE - 36);
    for (int i = 0; i < 64; i++) {
        stepToByteArray(&track->steps[i], bytes + 64 + (i * STEP_BYTE_SIZE));
    }
}

/**
 * Convert Byte Array to Track
 */
struct Track byteArrayToTrack(const unsigned char bytes[TRACK_BYTE_SIZE]) {
    struct Track track;
    memcpy(track.name, bytes, 32);
    track.midiDevice = bytes[32];
    track.midiChannel = bytes[33];
    track.program = bytes[34];
    track.pageLength = bytes[35];
    for (int i = 0; i < 64; i++) {
        unsigned char arr[16];
        // Copy part of byte array into arr
        memcpy(arr, bytes + 64 + (i * STEP_BYTE_SIZE), STEP_BYTE_SIZE);
        track.steps[i] = byteArrayToStep(arr);
    }
    return track;
}

/**
 * Convert Pattern to Byte Array
 * byte 1-32    : Name
 * byte 33-64   : Spare
 * byte 65-...  : Track Data
 */
void patternToByteArray(const struct Pattern *pattern, unsigned char bytes[PATTERN_BYTE_SIZE]) {
    memcpy(bytes, pattern->name, 32);
    memset(bytes + 32, 0, 64 - 32);
    for (int i = 0; i < 16; i++) {
        trackToByteArray(&pattern->tracks[i], bytes + 64 + (i * TRACK_BYTE_SIZE));
    }
}

/**
 * Convert byte array to pattern
 */
struct Pattern byteArrayToPattern(const unsigned char bytes[PATTERN_BYTE_SIZE]) {
    struct Pattern pattern;
    memcpy(pattern.name, bytes, 32);
    for (int i = 0; i < 16; i++) {
        pattern.tracks[i] = byteArrayToTrack(bytes + 64 + (i * TRACK_BYTE_SIZE));
    }
    return pattern;
}

/**
 * Convert Sequence to Byte Array
 * byte 1-32    : Name
 * byte 33-64   : Spare
 * byte 65-...  : Pattern Data
 */
void sequenceToByteArray(const struct Sequence *sequence, unsigned char bytes[SEQUENCE_BYTE_SIZE]) {
    memcpy(bytes, sequence->name, 32);
    memset(bytes + 32, 0, 64 - 32);
    for (int i = 0; i < 16; i++) {
        patternToByteArray(&sequence->patterns[i], bytes + 64 + (i * PATTERN_BYTE_SIZE));
    }
}

/**
 * Convert Byte Array to Sequence
 */
struct Sequence byteArrayToSequence(const unsigned char bytes[SEQUENCE_BYTE_SIZE]) {
    struct Sequence sequence;
    memcpy(sequence.name, bytes, 32);
    for (int i = 0; i < 16; i++) {
        sequence.patterns[i] = byteArrayToPattern(bytes + 32 + (i * PATTERN_BYTE_SIZE));
    }
    return sequence;
}

/**
 * Convert Project to Byte Array
 * byte 1-32    : Name
 * byte 33-256  : Spare
 * byte 256-... : Pattern Data
 */
void projectToByteArray(const struct Project *project, unsigned char bytes[PROJECT_BYTE_SIZE]) {
    memcpy(bytes, project->name, 32);
    memset(bytes + 32, 0, 256 - 32);
    for (int i = 0; i < 16; i++) {
        sequenceToByteArray(&project->sequences[i], bytes + 256 + (i * SEQUENCE_BYTE_SIZE));
    }
}

/**
 * Convert Byte Array to Project
 */
struct Project byteArrayToProject(const unsigned char bytes[PROJECT_BYTE_SIZE]) {
    struct Project project;
    memcpy(project.name, bytes, 32);
    for (int i = 0; i < 16; i++) {
        project.sequences[i] = byteArrayToSequence(bytes + 256  + (i  * SEQUENCE_BYTE_SIZE));
    }
    return project;
}

/**
 * Initialize a new project
 */
struct Project initializeProject() {
    struct Project project;
    strcpy(project.name, "New Project");
    for (int i = 0; i < 16; i++) {
        struct Sequence sequence;
        snprintf(sequence.name, sizeof(sequence.name), "Sequence %d", i);
        for (int j = 0; j < 16; j++) {
            struct Pattern pattern;
            snprintf(pattern.name, sizeof(pattern.name), "Pattern %d", j);
            for (int k = 0; k < 16; k++) {
                struct Track track;
                snprintf(track.name, sizeof(track.name), "Track %d", k);
                track.midiDevice = 0;
                track.midiChannel = 0;
                track.program = 0;
                track.pageLength = 16;
                for (int s = 0; s < 64; s++) {
                    struct Step step;
                    step.note = 0;
                    step.velocity = 0;
                    step.nudge = 0;
                    track.steps[s] = step;
                }
                pattern.tracks[k] = track;
            }
            sequence.patterns[j] = pattern;
        }
        project.sequences[i] = sequence;
    }

    return project;
}

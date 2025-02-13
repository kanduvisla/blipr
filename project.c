#include <stdio.h>
#include <string.h>
#include "project.h"

#define STEP_BYTE_SIZE 16

/**
 * Convert Step to Byte Array
 * Byte 1       : Note
 * Byte 2       : Velocity
 * Byte 3       : Nudge
 * Bytes 4-12   : Spare
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
 * byte 65-1088 : Steps data (64 steps, 16 bytes each)
 */
void trackToByteArray(const struct Track *track, unsigned char bytes[1088]) {
    memcpy(bytes, track->name, 32);
    bytes[32] = track->midiDevice;
    bytes[33] = track->midiChannel;
    bytes[34] = track->program;
    bytes[35] = track->pageLength;
    memset(bytes + 36, 0, 64 - 36);
    for (int i = 0; i < 64; ++i) {
        unsigned char arr[STEP_BYTE_SIZE];
        stepToByteArray(&track->steps[i], arr);
        memcpy(bytes + 64 + (i * STEP_BYTE_SIZE), arr, STEP_BYTE_SIZE);
    }
}

/**
 * Convert Byte Array to Track
 */
struct Track byteArrayToTrack(const unsigned char bytes[1088]) {
    struct Track track;
    memcpy(track.name, bytes, 32);
    track.midiDevice = bytes[32];
    track.midiChannel = bytes[33];
    track.program = bytes[34];
    track.pageLength = bytes[35];
    return track;
}

struct Project initializeProject() {
    struct Project project;

    strcpy(project.name, "New Project");
    for (int i = 0; i < 16; i++) {
        struct Sequence sequence;
        // strcpy(sequence.name, sprintf("Sequence %d", i));
        snprintf(sequence.name, sizeof(sequence.name), "Sequence %d", i);
        for (int j = 0; j < 16; j++) {
            struct Pattern pattern;
            // strcpy(pattern.name, sprintf("Pattern %d", j));
            snprintf(pattern.name, sizeof(pattern.name), "Pattern %d", j);

            for (int k = 0; k < 16; k++) {
                struct Track track;
                // strcpy(track.name, sprintf("Track %d", k));
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

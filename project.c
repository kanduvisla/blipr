#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"
#include "constants.h"

/**
 * Convert Note to Byte Array
 * Byte 1       : Enabled
 * Byte 2       : Note
 * Byte 3       : Velocity
 * Byte 4       : Nudge
 * Byte 5       : Trigg
 * Byte 6       : Length
 * Byte 7       : CC1 Value
 * Byte 8       : CC2 Value
 */
void noteToByteArray(const struct Note *note, unsigned char bytes[NOTE_BYTE_SIZE]) {
    bytes[0] = note->enabled;
    bytes[1] = note->note;
    bytes[2] = note->velocity;
    bytes[3] = (unsigned char)(note->nudge + 63);
    bytes[4] = note->trigg;
    bytes[5] = note->length;
    bytes[6] = note->cc1Value;
    bytes[7] = note->cc2Value;
}

/**
 * Convert Byte Array to Note
 */
struct Note byteArrayToNote(const unsigned char bytes[NOTE_BYTE_SIZE]) {
    struct Note note;
    note.enabled = bytes[0];
    note.note = bytes[1];
    note.velocity = bytes[2];
    note.nudge = (char)bytes[3] - 63;
    note.trigg = bytes[4];
    note.length = bytes[5];
    note.cc1Value = bytes[6];
    note.cc2Value = bytes[7];
    return note;
}

/**
 * Convert Step to Byte Array
 * Byte 1       : Note
 * Byte 2       : Velocity
 * Byte 3       : Nudge
 * Byte 4-...   : Spare
 */
void stepToByteArray(const struct Step *step, unsigned char bytes[STEP_BYTE_SIZE]) {
    for (int i = 0; i < NOTES_IN_STEP; i++) {
        noteToByteArray(&step->notes[i], bytes + (i * NOTE_BYTE_SIZE));
    }
}

/**
 * Convert Byte Array to Step
 */
struct Step byteArrayToStep(const unsigned char bytes[STEP_BYTE_SIZE]) {
    struct Step step;
    for (int i = 0; i < NOTES_IN_STEP; i++) {
        step.notes[i] = byteArrayToNote(bytes + (i * NOTE_BYTE_SIZE));
    }
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
    memcpy(bytes + 36, &(track->trackLength), 2);
    bytes[38] = track->cc1Assignment;
    bytes[39] = track->cc2Assignment;
    memset(bytes + 40, 0, TRACK_BYTE_SIZE - 40);
    for (int i = 0; i < 64; i++) {
        stepToByteArray(&track->steps[i], bytes + 64 + (i * STEP_BYTE_SIZE));
    }
}

/**
 * Convert Byte Array to Track
 */
struct Track* byteArrayToTrack(const unsigned char bytes[TRACK_BYTE_SIZE]) {
    struct Track* track = malloc(TRACK_BYTE_SIZE);
    if (track == NULL) {
        printf("Error: cannot allocate memory for track\n");
        exit(1);
    }

    memcpy(track->name, bytes, 32);
    track->midiDevice = bytes[32];
    track->midiChannel = bytes[33];
    track->program = bytes[34];
    track->pageLength = bytes[35];
    memcpy(&(track->trackLength), bytes + 36, 2);
    track->cc1Assignment = bytes[38];
    track->cc2Assignment = bytes[39];
    for (int i = 0; i < 64; i++) {
        unsigned char arr[STEP_BYTE_SIZE];
        // Copy part of byte array into arr
        memcpy(arr, bytes + 64 + (i * STEP_BYTE_SIZE), STEP_BYTE_SIZE);
        track->steps[i] = byteArrayToStep(arr);
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
struct Pattern* byteArrayToPattern(const unsigned char bytes[PATTERN_BYTE_SIZE]) {
    struct Pattern* pattern = malloc(PATTERN_BYTE_SIZE);
    if (pattern == NULL) {
        printf("Error: cannot allocate memory for pattern\n");
        exit(1);
    }

    memcpy(pattern->name, bytes, 32);
    for (int i = 0; i < 16; i++) {
        pattern->tracks[i] = *byteArrayToTrack(bytes + 64 + (i * TRACK_BYTE_SIZE));
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
struct Sequence* byteArrayToSequence(const unsigned char bytes[SEQUENCE_BYTE_SIZE]) {
    struct Sequence* sequence = malloc(SEQUENCE_BYTE_SIZE);
    if (sequence == NULL) {
        printf("Error: cannot allocate memory for sequence\n");
        exit(1);
    }

    memcpy(sequence->name, bytes, 32);
    for (int i = 0; i < 16; i++) {
        sequence->patterns[i] = *byteArrayToPattern(bytes + 64 + (i * PATTERN_BYTE_SIZE));
    }
    return sequence;
}

/**
 * Convert Project to Byte Array
 * byte 1-32    : Name
 * byte 33-64   : Midi Device #1 name
 * byte 65-96   : Midi Device #2 name
 * byte 97-128  : Midi Device #3 name
 * byte 129-160 : Midi Device #4 name
 * byte 33-256  : Spare 
 * byte 257-... : Sequence Data
 */
void projectToByteArray(const struct Project *project, unsigned char bytes[PROJECT_BYTE_SIZE]) {
    memcpy(bytes, project->name, 32);
    memcpy(bytes + 32, project->midiDeviceAName, 32);
    memcpy(bytes + 64, project->midiDeviceBName, 32);
    memcpy(bytes + 96, project->midiDeviceCName, 32);
    memcpy(bytes + 128, project->midiDeviceDName, 32);
    memset(bytes + 160, 0, 256 - 160);
    for (int i = 0; i < 16; i++) {
        sequenceToByteArray(&project->sequences[i], bytes + 256 + (i * SEQUENCE_BYTE_SIZE));
    }
}

/**
 * Convert Byte Array to Project
 */
struct Project* byteArrayToProject(const unsigned char bytes[PROJECT_BYTE_SIZE]) {
    struct Project* project = malloc(PROJECT_BYTE_SIZE);
    if (project == NULL) {
        printf("Error: cannot allocate memory for project\n");
        exit(1);
    }

    memcpy(project->name, bytes, 32);
    memcpy(project->midiDeviceAName, bytes + 32, 32);
    memcpy(project->midiDeviceBName, bytes + 64, 32);
    memcpy(project->midiDeviceCName, bytes + 96, 32);
    memcpy(project->midiDeviceDName, bytes + 128, 32);
    for (int i = 0; i < 16; i++) {
        project->sequences[i] = *byteArrayToSequence(bytes + 256  + (i  * SEQUENCE_BYTE_SIZE));
    }
    return project;
}

/**
 * Initialize a new project
 */
void initializeProject(struct Project* project) {
    strcpy(project->name, "New Project");
    for (int i = 0; i < 16; i++) {
        struct Sequence sequence;
        snprintf(sequence.name, sizeof(sequence.name), "Sequence %d", i + 1);
        for (int j = 0; j < 16; j++) {
            struct Pattern pattern;
            snprintf(pattern.name, sizeof(pattern.name), "Pattern %d", j + 1);
            for (int k = 0; k < 16; k++) {
                struct Track track;
                snprintf(track.name, sizeof(track.name), "Track %d", k + 1);
                track.midiDevice = 0;
                track.midiChannel = 0;
                track.program = BLIPR_PROGRAM_NONE;
                track.pageLength = 16;
                track.trackLength = 64;
                track.polyCount = 8;
                for (int s = 0; s < 64; s++) {
                    struct Step step;
                    for (int n = 0; n < NOTES_IN_STEP; n++) {
                        struct Note note;
                        note.note = 0;
                        note.velocity = 0;
                        step.notes[n] = note;
                    }
                    track.steps[s] = step;
                }
                pattern.tracks[k] = track;
            }
            sequence.patterns[j] = pattern;
        }
        project->sequences[i] = sequence;
    }
}

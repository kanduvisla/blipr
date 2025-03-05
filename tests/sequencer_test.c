#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../programs/sequencer.h"
#include "../utils.h"
#include "../constants.h"

void testCalculateTrackStepIndex() {
    // Test cases, for manual testing:
    /*
    printf("Page size 12, total length 64:\n");
    for (int i = 0; i < 170; i++) {
        printf("PPQN Step: %d, Track Step Index: %d\n", i, calculateTrackStepIndex(i, 12, 64));
    }

    printf("\nPage size 4, total length 64:\n");
    for (int i = 0; i < 120; i++) {
        printf("PPQN Step: %d, Track Step Index: %d\n", i, calculateTrackStepIndex(i, 4, 64));
    }

    printf("\nPage size 12, total length 44:\n");
    for (int i = 0; i < 150; i++) {
        printf("PPQN Step: %d, Track Step Index: %d\n", i, calculateTrackStepIndex(i, 12, 44));
    }

    printf("\nPage size 16, total length 44:\n");
    for (int i = 0; i < 150; i++) {
        printf("PPQN Step: %d, Track Step Index: %d\n", i, calculateTrackStepIndex(i, 16, 44));
    }
    */

    // In continuous mode, page length is ignored
    assert(calculateTrackStepIndex(0, 12, 64, PAGE_PLAY_MODE_CONTINUOUS) == 0);
    assert(calculateTrackStepIndex(12, 12, 64, PAGE_PLAY_MODE_CONTINUOUS) == 12);
    assert(calculateTrackStepIndex(24, 12, 64, PAGE_PLAY_MODE_CONTINUOUS) == 24);
    assert(calculateTrackStepIndex(36, 12, 32, PAGE_PLAY_MODE_CONTINUOUS) == 4);
    assert(calculateTrackStepIndex(48, 12, 32, PAGE_PLAY_MODE_CONTINUOUS) == 16);
    assert(calculateTrackStepIndex(64, 12, 64, PAGE_PLAY_MODE_CONTINUOUS) == 0);
    assert(calculateTrackStepIndex(65, 12, 64, PAGE_PLAY_MODE_CONTINUOUS) == 1);

    // In page repeat mode, track length is ignored
    assert(calculateTrackStepIndex(0, 12, 64, PAGE_PLAY_MODE_REPEAT) == 0);
    assert(calculateTrackStepIndex(12, 12, 64, PAGE_PLAY_MODE_REPEAT) == 16);
    assert(calculateTrackStepIndex(13, 12, 64, PAGE_PLAY_MODE_REPEAT) == 17);
    assert(calculateTrackStepIndex(13, 14, 64, PAGE_PLAY_MODE_REPEAT) == 13);
    assert(calculateTrackStepIndex(24, 12, 64, PAGE_PLAY_MODE_REPEAT) == 32);
    assert(calculateTrackStepIndex(24, 16, 64, PAGE_PLAY_MODE_REPEAT) == 24);
    assert(calculateTrackStepIndex(36, 12, 32, PAGE_PLAY_MODE_REPEAT) == 48);
    assert(calculateTrackStepIndex(48, 12, 32, PAGE_PLAY_MODE_REPEAT) == 64);
    assert(calculateTrackStepIndex(64, 12, 64, PAGE_PLAY_MODE_REPEAT) == 84);
    assert(calculateTrackStepIndex(65, 12, 64, PAGE_PLAY_MODE_REPEAT) == 85);
}

void testTrigConditions() {
    // What is required for a trig condition?
    // Total steps counter
    // Track length (so we can calculate how many times the step has been played)
    struct Note note;
    note.trigg = create2FByte(true, false, TRIG_FIRST); // This note is trigged only the first time it is played
    assert(isNoteTrigged(note.trigg, 0) == true);
    assert(isNoteTrigged(note.trigg, 1) == false);
    assert(isNoteTrigged(note.trigg, 2) == false);
    note.trigg = create2FByte(true, true, TRIG_FIRST);  // This note is trigged not the first time it is played (inversed flag)
    assert(isNoteTrigged(note.trigg, 0) == false);
    assert(isNoteTrigged(note.trigg, 1) == true);
    assert(isNoteTrigged(note.trigg, 2) == true);
    note.trigg = create2FByte(true, false, TRIG_1_2);   // This note is trigged every 1 out of 2 repeats
    assert(isNoteTrigged(note.trigg, 0) == true);
    assert(isNoteTrigged(note.trigg, 1) == false);
    assert(isNoteTrigged(note.trigg, 2) == true);
    assert(isNoteTrigged(note.trigg, 3) == false);
    note.trigg = create2FByte(true, false, TRIG_2_3);   // This note is trigged every 2 out of 3 repeats
    assert(isNoteTrigged(note.trigg, 0) == false);
    assert(isNoteTrigged(note.trigg, 1) == true);
    assert(isNoteTrigged(note.trigg, 2) == false);
    assert(isNoteTrigged(note.trigg, 3) == false);
    note.trigg = create2FByte(true, true, TRIG_2_3);   // This note is not trigged every 2 out of 3 repeats (inversed flag)
    assert(isNoteTrigged(note.trigg, 0) == true);
    assert(isNoteTrigged(note.trigg, 1) == false);
    assert(isNoteTrigged(note.trigg, 2) == true);
    assert(isNoteTrigged(note.trigg, 3) == true);
}

void testFindMatchingNotes() {
    // Find matching notes:
    struct Track *track = malloc(TRACK_BYTE_SIZE);
    for (int s = 0; s < 64; s++) {
        struct Step step;
        for (int n = 0; n < NOTES_IN_STEP; n++) {
            struct Note note;
            step.notes[n] = note;
        }
        track->steps[s] = step;
    }
    // Polycount: 0=8, 1=4, 2=2, 3=1
    track->polyCount = 0;   // 8 polyphony
    track->steps[0].notes[0].enabled = true;
    track->steps[0].notes[0].note = 60;
    track->steps[0].notes[0].nudge = 0;

    // Run tests:
    MatchingNote matches[8] = {NULL};
    int numMatches = findMatchingNotes(track, 0, matches);  // Check for PPQN 0
    assert(numMatches == 1);

    matches[] = NULL;
    numMatches = findMatchingNotes(track, PPQN * 1, matches);  // Check for PPQN 0
    assert(numMatches == 0);
}

void testSequencer() {
    testCalculateTrackStepIndex();
    testTrigConditions();
    testFindMatchingNotes();
}
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../programs/sequencer.h"
#include "../utils.h"
#include "../constants.h"
#include "../print.h"

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

void testGetTrackStepIndexForContinuousPlay() {
    struct Track *track = malloc(TRACK_BYTE_SIZE);
    track->pagePlayMode = PAGE_PLAY_MODE_CONTINUOUS;
    track->trackLength = 43; // =0-based
    u_int64_t ppqnCounter = 0;

    assert(getTrackStepIndex(&ppqnCounter, track) == 0);
    // For each pulse in this 16th note, the track step should still remain the same:
    for (int i=0; i<PP16N - 1; i++) {
        ppqnCounter++;
        assert(getTrackStepIndex(&ppqnCounter, track) == 0);
    }
    // Next pulse should increase the step:
    ppqnCounter++;
    assert(getTrackStepIndex(&ppqnCounter, track) == 1);
    // Add 15 steps - 1 pulse:
    ppqnCounter += (PP16N * 15) -1;
    assert(getTrackStepIndex(&ppqnCounter, track) == 15);
    // Add 1 pulse:
    ppqnCounter++;
    assert(getTrackStepIndex(&ppqnCounter, track) == 16);
    // Add steps up to the track length, -1 pulse:
    ppqnCounter += (PP16N * (44 - 16)) - 1;
    assert(getTrackStepIndex(&ppqnCounter, track) == 43);
    // Add 1 pulse:
    ppqnCounter++;
    assert(getTrackStepIndex(&ppqnCounter, track) == 0);
}

void testGetTrackStepIndexForRepeatPlay() {
    struct Track *track = malloc(TRACK_BYTE_SIZE);
    track->pagePlayMode = PAGE_PLAY_MODE_REPEAT;
    track->pageLength = 15; // =0-based
    track->selectedPage = 0;
    track->selectedPageBank = 0;
    u_int64_t ppqnCounter = 0;

    assert(getTrackStepIndex(&ppqnCounter, track) == 0);
    // For each pulse in this 16th note, the track step should still remain the same:
    for (int i=0; i<PP16N - 1; i++) {
        ppqnCounter++;
        assert(getTrackStepIndex(&ppqnCounter, track) == 0);
    }
    // Next pulse should increase the step:
    ppqnCounter++;
    assert(getTrackStepIndex(&ppqnCounter, track) == 1);
    // Add 15 steps - 1 pulse:
    ppqnCounter += (PP16N * 15) -1;
    assert(getTrackStepIndex(&ppqnCounter, track) == 15);
    // Add 1 pulse:
    ppqnCounter++;
    assert(getTrackStepIndex(&ppqnCounter, track) == 0);

    // Test different page:
    track->selectedPage = 1;
    assert(getTrackStepIndex(&ppqnCounter, track) == 16);
    track->selectedPage = 2;
    assert(getTrackStepIndex(&ppqnCounter, track) == 32);
    track->selectedPage = 3;
    assert(getTrackStepIndex(&ppqnCounter, track) == 48);
    track->selectedPage = 4; // overflow
    assert(getTrackStepIndex(&ppqnCounter, track) == 0);

    // Test different page bank (page bank doesn't affect step, but note (in conjunction with poly)):
    track->selectedPage = 0;
    track->selectedPageBank = 0;
    assert(getTrackStepIndex(&ppqnCounter, track) == 0);
    track->selectedPage = 1;
    assert(getTrackStepIndex(&ppqnCounter, track) == 16);
    track->selectedPage = 2;
    assert(getTrackStepIndex(&ppqnCounter, track) == 32);
    track->selectedPage = 3;
    assert(getTrackStepIndex(&ppqnCounter, track) == 48);
    track->selectedPage = 4; // overflow
    assert(getTrackStepIndex(&ppqnCounter, track) == 0);

    // Test different page lengths:
    track->selectedPage = 0;
    track->pageLength = 11; // 0-based
    ppqnCounter = 0;
    assert(getTrackStepIndex(&ppqnCounter, track) == 0);
    ppqnCounter += (PP16N * 12) - 1;
    assert(getTrackStepIndex(&ppqnCounter, track) == 11);
    ppqnCounter += 1;
    assert(getTrackStepIndex(&ppqnCounter, track) == 0);
}

void assertEnabledNotesCount(const struct Note **notes, int expectedCount) {
    int enabledCount = 0;
    for (int i  = 0; i < NOTES_IN_STEP; i++) {
        if (notes[i] != NULL && notes[i]->enabled) {
            enabledCount++;
        }
    }
    assert(enabledCount == expectedCount);
    if (enabledCount != expectedCount) {
        printWarning("expected %d enabled notes, got: %d", expectedCount, enabledCount);
    }
}

void testGetNotesAtTrackStepIndex() {
    // Setup:
    struct Track *track = malloc(TRACK_BYTE_SIZE);
    track->pagePlayMode = PAGE_PLAY_MODE_CONTINUOUS;
    track->trackLength = 63; // =0-based
    track->polyCount = 0;   // 8 voice polyphony
    for (int s = 0; s < 64; s++) {
        struct Step step;
        for (int n = 0; n < NOTES_IN_STEP; n++) {
            struct Note note;
            note.enabled = false;
            step.notes[n] = note;
        }
        track->steps[s] = step;
    }
    // Set 1 note at step 0:
    track->steps[0].notes[0].enabled = true;
    // Set 2 notes at step 1:
    track->steps[1].notes[0].enabled = true;
    track->steps[1].notes[1].enabled = true;
    // Set 1 notes at step 3:
    track->steps[3].notes[0].enabled = true;
    // Set 4 notes at step 5:
    track->steps[5].notes[0].enabled = true;
    track->steps[5].notes[0].note = 60;
    track->steps[5].notes[2].enabled = true;
    track->steps[5].notes[2].note = 62;
    track->steps[5].notes[3].enabled = true;
    track->steps[5].notes[3].note = 63;
    track->steps[5].notes[4].enabled = true;
    track->steps[5].notes[4].note = 64;

    // Exercise & Verify:
    struct Note *notes[NOTES_IN_STEP];
    getNotesAtTrackStepIndex(0, track, notes);
    assertEnabledNotesCount(notes, 1);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    getNotesAtTrackStepIndex(1, track, notes);
    assertEnabledNotesCount(notes, 2);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    getNotesAtTrackStepIndex(2, track, notes);
    assertEnabledNotesCount(notes, 0);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    getNotesAtTrackStepIndex(3, track, notes);
    assertEnabledNotesCount(notes, 1);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    getNotesAtTrackStepIndex(4, track, notes);
    assertEnabledNotesCount(notes, 0);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 4);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    // Test 4 voice polyphony
    track->polyCount = 1;   // 4 voice polyphony
    track->selectedPageBank = 0;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 3);  // Only note 0-3
    assert(notes[0]->note == 60);
    assert(notes[2]->note == 62);
    assert(notes[3]->note == 63);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    track->selectedPageBank = 1;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 1);  // Only note 4-7
    assert(notes[0]->note == 64);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    // Test 2 voice polyphony
    track->polyCount = 2;   // 2 voice polyphony
    track->selectedPageBank = 0;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 1);  // Only note 0-1
    assert(notes[0]->note == 60);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    track->selectedPageBank = 1;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 2);  // Only note 2-3
    assert(notes[0]->note == 62);
    assert(notes[1]->note == 63);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    track->selectedPageBank = 2;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 1);  // Only note 4-5
    assert(notes[0]->note == 64);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    track->selectedPageBank = 3;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 0);  // Only note 6-7
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    // Test 1 voice polyphony (only 0,2,3 and 4 have notes)
    track->polyCount = 3;   // 2 voice polyphony
    track->selectedPageBank = 0;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 1);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    track->selectedPageBank = 1;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 0);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    track->selectedPageBank = 2;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 1);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    track->selectedPageBank = 3;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 1);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    track->selectedPageBank = 4;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 1);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    track->selectedPageBank = 5;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 0);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    track->selectedPageBank = 6;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 0);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);

    track->selectedPageBank = 7;
    getNotesAtTrackStepIndex(5, track, notes);
    assertEnabledNotesCount(notes, 0);
    memset(notes, 0, NOTE_BYTE_SIZE * 8);
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
    /*
    matches[] = NULL;
    numMatches = findMatchingNotes(track, PPQN * 1, matches);  // Check for PPQN 0
    assert(numMatches == 0);
    */
}

void testSequencer() {
    testCalculateTrackStepIndex();
    testTrigConditions();
    testGetTrackStepIndexForContinuousPlay();
    testGetTrackStepIndexForRepeatPlay();
    testGetNotesAtTrackStepIndex();
    // testFindMatchingNotes();
}
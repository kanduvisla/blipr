#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../programs/sequencer.h"
#include "../utils.h"
#include "../constants.h"
#include "../print.h"

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

static bool testIsFirstPulse = false;

static void testProcessPulseCallback() {
    testIsFirstPulse = true;
}

void testGetTrackStepIndexForContinuousPlay() {
    struct Track *track = malloc(TRACK_BYTE_SIZE);
    track->pagePlayMode = PAGE_PLAY_MODE_CONTINUOUS;
    track->trackLength = 43; // =0-based
    track->shuffle = PP16N;
    track->speed = TRACK_SPEED_NORMAL;
    u_int64_t ppqnCounter = 0;

    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 0);
    assert(testIsFirstPulse == true);
    testIsFirstPulse = false;
    // For each pulse in this 16th note, the track step should still remain the same:
    for (int i=0; i<PP16N - 1; i++) {
        ppqnCounter++;
        testIsFirstPulse = false;
        assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 0);
        assert(testIsFirstPulse == false);
    }
    
    // Next pulse should increase the step:
    ppqnCounter++;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 1);
    assert(testIsFirstPulse == false);
    // Add 15 steps - 1 pulse:
    ppqnCounter += (PP16N * 15) -1;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 15);
    assert(testIsFirstPulse == false);
    // Add 1 pulse:
    ppqnCounter++;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 16);
    assert(testIsFirstPulse == false);
    // Add steps up to the track length, -1 pulse:
    ppqnCounter += (PP16N * (44 - 16)) - 1;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 43);
    assert(testIsFirstPulse == false);
    // Add 1 pulse:
    ppqnCounter++;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 0);
    assert(testIsFirstPulse == true);
}

void testGetTrackStepIndexForRepeatPlay() {
    struct Track *track = malloc(TRACK_BYTE_SIZE);
    track->pagePlayMode = PAGE_PLAY_MODE_REPEAT;
    track->pageLength = 15; // =0-based
    track->shuffle = PP16N;
    track->selectedPage = 0;
    track->selectedPageBank = 0;
    track->speed = TRACK_SPEED_NORMAL;
    u_int64_t ppqnCounter = 0;
    
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 0);
    assert(testIsFirstPulse == true);
    // For each pulse in this 16th note, the track step should still remain the same:
    for (int i=0; i<PP16N - 1; i++) {
        ppqnCounter++;
        testIsFirstPulse = false;
        assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 0);
        assert(testIsFirstPulse == false);
    }
    // Next pulse should increase the step:
    ppqnCounter++;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 1);
    assert(testIsFirstPulse == false);
    // Add 15 steps - 1 pulse:
    ppqnCounter += (PP16N * 15) -1;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 15);
    assert(testIsFirstPulse == false);
    // Add 1 pulse:
    ppqnCounter++;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 0);
    assert(testIsFirstPulse == true);

    // Test different page:
    track->selectedPage = 1;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 16);
    assert(testIsFirstPulse == true);
    track->selectedPage = 2;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 32);
    assert(testIsFirstPulse == true);
    track->selectedPage = 3;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 48);
    assert(testIsFirstPulse == true);
    track->selectedPage = 4; // overflow
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 0);
    assert(testIsFirstPulse == true);

    // Test different page bank (page bank doesn't affect step, but note (in conjunction with poly)):
    track->selectedPage = 0;
    track->selectedPageBank = 0;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 0);
    assert(testIsFirstPulse == true);
    track->selectedPage = 1;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 16);
    assert(testIsFirstPulse == true);
    track->selectedPage = 2;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 32);
    assert(testIsFirstPulse == true);
    track->selectedPage = 3;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 48);
    assert(testIsFirstPulse == true);
    track->selectedPage = 4; // overflow
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 0);
    assert(testIsFirstPulse == true);

    // Test different page lengths:
    track->selectedPage = 0;
    track->pageLength = 11; // 0-based
    ppqnCounter = 0;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 0);
    assert(testIsFirstPulse == true);
    ppqnCounter += (PP16N * 12) - 1;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 11);
    assert(testIsFirstPulse == false);
    ppqnCounter += 1;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 0);
    assert(testIsFirstPulse == true);
    track->selectedPage = 1;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 16);
    assert(testIsFirstPulse == true);
    ppqnCounter += (PP16N * 12) - 1;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 27);
    assert(testIsFirstPulse == false);
    ppqnCounter += 1;
    testIsFirstPulse = false;
    assert(getTrackStepIndex(&ppqnCounter, track, testProcessPulseCallback) == 16);
    assert(testIsFirstPulse == true);
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
    track->shuffle = PP16N;
    track->polyCount = 0;   // 8 voice polyphony
    track->speed = TRACK_SPEED_NORMAL;
    for (int s = 0; s < 64; s++) {
        struct Step step;
        for (int n = 0; n < NOTES_IN_STEP; n++) {
            struct Note note;
            note.enabled = false;
            note.nudge = PP16N; // no nudge
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

// Reference to the last played note, for testing
struct Note playedNotes[NOTES_IN_STEP];
int playedNoteCount;
void testPlayNoteCallback(const struct Note *note) {
    playedNotes[playedNoteCount] = *note;
    playedNoteCount++;
}

void testProcessPulseNudge() {
    // Setup:
    struct Track *track = malloc(TRACK_BYTE_SIZE);
    track->pagePlayMode = PAGE_PLAY_MODE_CONTINUOUS;
    track->trackLength = 63; // =0-based
    track->shuffle = PP16N;
    track->polyCount = 0;   // 8 voice polyphony
    track->speed = TRACK_SPEED_NORMAL;
    for (int s = 0; s < 64; s++) {
        struct Step step;
        for (int n = 0; n < NOTES_IN_STEP; n++) {
            struct Note note;
            note.enabled = false;
            note.nudge = PP16N; // no nudge
            note.trigg = create2FByte(false, false, TRIG_DISABLED); // no trigg condition
            step.notes[n] = note;
        }
        track->steps[s] = step;
    }
    
    track->steps[0].notes[0].enabled = true; // Note 0, no nudge
    track->steps[0].notes[0].note = 60;
    track->steps[0].notes[1].enabled = true; // Note 1, nudge +1
    track->steps[0].notes[1].nudge = PP16N + 1;
    track->steps[0].notes[1].note = 61;
    track->steps[0].notes[2].enabled = true; // Note 2, no nudge
    track->steps[0].notes[2].note = 62;
    track->steps[0].notes[3].enabled = true; // Note 1, nudge +2
    track->steps[0].notes[3].nudge = PP16N + 2;
    track->steps[0].notes[3].note = 63;
    track->steps[1].notes[0].enabled = true; // Next step has negative nudge, so it should be played earlier
    track->steps[1].notes[0].note = 64;
    track->steps[1].notes[0].nudge = PP16N - 2;
    track->steps[0].notes[4].enabled = true; // Note with negative nudge on step 0. So this needs to be triggered on step 63 + (PP16N - 2)
    track->steps[0].notes[4].note = 65;
    track->steps[0].notes[4].nudge = PP16N - 2;

    // Nudge test:
    int ppqnCounter = 0;
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 2);
    assert(playedNotes[0].note == 60);
    assert(playedNotes[1].note == 62);
    
    playedNoteCount = 0;
    ppqnCounter = 1;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);
    assert(playedNotes[0].note == 61);
    
    playedNoteCount = 0;
    ppqnCounter = 2;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);
    assert(playedNotes[0].note == 63);

    // Negative nudge:
    playedNoteCount = 0;
    ppqnCounter = PP16N - 2;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);
    assert(playedNotes[0].note == 64);

    playedNoteCount = 0;
    ppqnCounter = (PP16N * 64) - 2;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);
    assert(playedNotes[0].note == 65);
}

void testProcessPulseShuffle() {
    // Setup:
    struct Track *track = malloc(TRACK_BYTE_SIZE);
    track->pagePlayMode = PAGE_PLAY_MODE_CONTINUOUS;
    track->trackLength = 63; // =0-based
    track->shuffle = PP16N + 2; // shuffle of 2
    track->polyCount = 0;   // 8 voice polyphony
    track->speed = TRACK_SPEED_NORMAL;
    for (int s = 0; s < 64; s++) {
        struct Step step;
        for (int n = 0; n < NOTES_IN_STEP; n++) {
            struct Note note;
            note.enabled = false;
            note.nudge = PP16N; // no nudge
            note.trigg = create2FByte(false, false, TRIG_DISABLED); // no trigg condition
            step.notes[n] = note;
        }
        // Enable all first notes:
        step.notes[0].enabled = true;
        track->steps[s] = step;
    }

    // Special case: step 5 also has note nudge:
    track->steps[5].notes[0].nudge = PP16N + 2;
    // Special case #2: step 7 has a negative nudge:
    track->steps[7].notes[0].nudge = PP16N - 2;
    // Special case #3: step 9 has a more negative nudge:
    track->steps[9].notes[0].nudge = PP16N - 4;
    // Special case $4: step 11 has a more positive nudge:
    track->steps[11].notes[0].nudge = PP16N + 4;

    int ppqnCounter = 0;    // step 0
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = PP16N;    // step 1
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = PP16N + 2;
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = PP16N * 2;    // step 2
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = PP16N * 3;    // step 3
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = (PP16N * 3) + 2;
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = PP16N * 4;    // step 4
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    // Note with additional nudge:
    ppqnCounter = PP16N * 5;    // step 5
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = (PP16N * 5) + 2;    // step 5 + 2 pulses (does not trigger because of additional note nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = (PP16N * 5) + 4;    // step 5 + 4 pulses (does trigger because of additional note nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = PP16N * 7;    // step 7 (note has -2 negative nudge, so this should cancel the shuffle out)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = PP16N * 9;    // step 9 (note has -4 negative nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = (PP16N * 9) - 2;    // step 9 (note has -4 negative nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = (PP16N * 11);    // step 11 (note has +4 nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = (PP16N * 11) + 6;    // step 11 (note has +4 nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    // Negative shuffle tests:
    track->shuffle = PP16N - 2; // shuffle of 2

    ppqnCounter = 0;    // step 0
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = PP16N;    // step 1
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = PP16N - 2;
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = PP16N * 2;    // step 2
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = PP16N * 3;    // step 3
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = (PP16N * 3) - 2;
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = PP16N * 4;    // step 4
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    // Note with additional nudge:
    ppqnCounter = PP16N * 5;    // step 5 will trigger because of additional +2 nudge
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = (PP16N * 5) - 2;    // step 5 - 2 pulses (does not trigger because of additional note nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = (PP16N * 5) + 2;    // step 5 + 2 pulses (does not trigger because of additional note nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = PP16N * 7;    // step 7 (note has -2 negative nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = (PP16N * 7) - 2;    // step 7 (note has -2 negative nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = (PP16N * 7) - 4;    // step 7 (note has -2 negative nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = PP16N * 9;    // step 9 (note has -4 negative nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = (PP16N * 9) - 6;    // step 9 (note has -4 negative nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);

    ppqnCounter = (PP16N * 11);    // step 11 (note has +4 nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 0);

    ppqnCounter = (PP16N * 11) + 2;    // step 11 (note has +4 nudge)
    playedNoteCount = 0;
    processPulse(&ppqnCounter, track, testProcessPulseCallback, testPlayNoteCallback);
    assert(playedNoteCount == 1);
}

void testSequencer() {
    testTrigConditions();
    testGetTrackStepIndexForContinuousPlay();
    testGetTrackStepIndexForRepeatPlay();
    testGetNotesAtTrackStepIndex();
    testProcessPulseNudge();
    testProcessPulseShuffle();
}
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

void testGetTrackStepIndexForContinuousPlay() {
    struct Track *track = malloc(TRACK_BYTE_SIZE);
    track->pagePlayMode = PAGE_PLAY_MODE_CONTINUOUS;
    track->trackLength = 43; // =0-based
    u_int64_t ppqnCounter = 0;
    bool isFirstPulse;

    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 0);
    assert(isFirstPulse == true);
    // For each pulse in this 16th note, the track step should still remain the same:
    for (int i=0; i<PP16N - 1; i++) {
        ppqnCounter++;
        assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 0);
        assert(isFirstPulse == false);
    }
    // Next pulse should increase the step:
    ppqnCounter++;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 1);
    assert(isFirstPulse == false);
    // Add 15 steps - 1 pulse:
    ppqnCounter += (PP16N * 15) -1;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 15);
    assert(isFirstPulse == false);
    // Add 1 pulse:
    ppqnCounter++;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 16);
    assert(isFirstPulse == false);
    // Add steps up to the track length, -1 pulse:
    ppqnCounter += (PP16N * (44 - 16)) - 1;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 43);
    assert(isFirstPulse == false);
    // Add 1 pulse:
    ppqnCounter++;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 0);
    assert(isFirstPulse == true);
}

void testGetTrackStepIndexForRepeatPlay() {
    struct Track *track = malloc(TRACK_BYTE_SIZE);
    track->pagePlayMode = PAGE_PLAY_MODE_REPEAT;
    track->pageLength = 15; // =0-based
    track->selectedPage = 0;
    track->selectedPageBank = 0;
    u_int64_t ppqnCounter = 0;
    bool isFirstPulse;

    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 0);
    assert(isFirstPulse == true);
    // For each pulse in this 16th note, the track step should still remain the same:
    for (int i=0; i<PP16N - 1; i++) {
        ppqnCounter++;
        assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 0);
        assert(isFirstPulse == false);
    }
    // Next pulse should increase the step:
    ppqnCounter++;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 1);
    assert(isFirstPulse == false);
    // Add 15 steps - 1 pulse:
    ppqnCounter += (PP16N * 15) -1;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 15);
    assert(isFirstPulse == false);
    // Add 1 pulse:
    ppqnCounter++;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 0);
    assert(isFirstPulse == true);

    // Test different page:
    track->selectedPage = 1;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 16);
    assert(isFirstPulse == true);
    track->selectedPage = 2;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 32);
    assert(isFirstPulse == true);
    track->selectedPage = 3;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 48);
    assert(isFirstPulse == true);
    track->selectedPage = 4; // overflow
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 0);
    assert(isFirstPulse == true);

    // Test different page bank (page bank doesn't affect step, but note (in conjunction with poly)):
    track->selectedPage = 0;
    track->selectedPageBank = 0;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 0);
    assert(isFirstPulse == true);
    track->selectedPage = 1;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 16);
    assert(isFirstPulse == true);
    track->selectedPage = 2;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 32);
    assert(isFirstPulse == true);
    track->selectedPage = 3;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 48);
    assert(isFirstPulse == true);
    track->selectedPage = 4; // overflow
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 0);
    assert(isFirstPulse == true);

    // Test different page lengths:
    track->selectedPage = 0;
    track->pageLength = 11; // 0-based
    ppqnCounter = 0;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 0);
    assert(isFirstPulse == true);
    ppqnCounter += (PP16N * 12) - 1;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 11);
    assert(isFirstPulse == false);
    ppqnCounter += 1;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 0);
    assert(isFirstPulse == true);
    track->selectedPage = 1;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 16);
    assert(isFirstPulse == true);
    ppqnCounter += (PP16N * 12) - 1;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 27);
    assert(isFirstPulse == false);
    ppqnCounter += 1;
    assert(getTrackStepIndex(&ppqnCounter, track, &isFirstPulse) == 16);
    assert(isFirstPulse == true);
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

void testSequencer() {
    testTrigConditions();
    testGetTrackStepIndexForContinuousPlay();
    testGetTrackStepIndexForRepeatPlay();
    testGetNotesAtTrackStepIndex();
}
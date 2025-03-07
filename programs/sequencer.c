#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include <string.h>
#include "sequencer.h"
#include "../project.h"
#include "../constants.h"
#include "../colors.h"
#include "../drawing.h"
#include "../drawing_components.h"
#include "../drawing_text.h"
#include "../utils.h"
#include "../midi.h"
#include "../print.h"

// TODO: These should be the global things I guess?

// Pages can be global, or per track configured
int selectedNote = 0;
int defaultNote = 60;       // C-4
int defaultVelocity = 100;
int halfVelocity = 50;
int selectedStep = -1;

/**
 * Reset the selected step
 */
void resetSequencerSelectedStep() {
    selectedStep = -1;
}

/**
 * Toggle a step
 */
void toggleStep(struct Step *step) {
    step->notes[selectedNote].enabled = !step->notes[selectedNote].enabled;
    // Set default values:
    if (step->notes[selectedNote].enabled) {
        step->notes[selectedNote].velocity = defaultVelocity;
        step->notes[selectedNote].note = defaultNote;
    }
}

/**
 * Toggle velocity
 */
void toggleVelocity(struct Step *step) {
    if (step->notes[selectedNote].enabled) {
        if (step->notes[selectedNote].velocity == defaultVelocity) {
            step->notes[selectedNote].velocity = halfVelocity;
        } else {
            step->notes[selectedNote].velocity = defaultVelocity;
        }
    } else {
        // If no note is set, create a new one on half velocity:
        toggleStep(step);
        step->notes[selectedNote].velocity = halfVelocity;
    }
}

/**
 * Set selected page
 */
void setSelectedPage(
    struct Track *selectedTrack,
    int index
) {
    if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        selectedTrack->selectedPage = index;
    } else {
        selectedTrack->queuedPage = index;
    }
}

/**
 * Transpose an amount of steps, clamped
 */
unsigned char transposeMidiNote(unsigned char midiNote, int steps) {
    // Ensure the input is a valid MIDI note
    if (midiNote > 127) {
        return 0;  // Return 0 (lowest note) for invalid input
    }

    // Perform the transposition
    int transposed = (int)midiNote + steps;

    // Clamp the result to the valid MIDI note range (0-127)
    if (transposed < 0) {
        transposed = 0;
    } else if (transposed > 127) {
        transposed = 127;
    } 

    // Set default note to this:
    defaultNote = transposed;

    return (unsigned char)transposed;   
}

static void handleKey(
    struct Track *selectedTrack,
    SDL_Scancode key
) {
    struct Step *step = &selectedTrack->steps[selectedStep];
    struct Note *note = &selectedTrack->steps[selectedStep].notes[selectedNote];
    // We're in the step editor, handle keys:
    if (key == BLIPR_KEY_1) { note->note = transposeMidiNote(note->note, -12); } else
    if (key == BLIPR_KEY_2) { note->note = transposeMidiNote(note->note, -1); } else 
    if (key == BLIPR_KEY_3) { note->note = transposeMidiNote(note->note, 1); } else 
    if (key == BLIPR_KEY_4) { note->note = transposeMidiNote(note->note, 12); } else 
    if (key == BLIPR_KEY_5) { note->velocity = MAX(0, note->velocity - 1); } else 
    if (key == BLIPR_KEY_6) { note->velocity = MIN(127, note->velocity + 1); } else 
    if (key == BLIPR_KEY_7) { note->length = MAX(0, note->length - 1); } else 
    if (key == BLIPR_KEY_8) { note->length = MIN(127, note->length + 1); } else 
    if (key == BLIPR_KEY_9) { 
        note->nudge = MAX(0, note->nudge - 1); 
    } else if (key == BLIPR_KEY_10) { 
        note->nudge = MIN(PP16N * 2, note->nudge + 1); 
    } else if (key == BLIPR_KEY_11) { 
        int value = get2FByteValue(note->trigg);
        bool isInversed = get2FByteFlag2(note->trigg);
        if (isInversed) {
            value += TRIG_HIGHEST_VALUE;
        }
        if (value > 0) {
            value--;
            isInversed = value > TRIG_HIGHEST_VALUE;
            note->trigg = create2FByte(
                value > 0,
                isInversed,
                isInversed ? (value - TRIG_HIGHEST_VALUE) : value
            );
        }
    } else if (key == BLIPR_KEY_12) { 
        int value = get2FByteValue(note->trigg);
        bool isInversed = get2FByteFlag2(note->trigg);
        if (isInversed) {
            value += TRIG_HIGHEST_VALUE;
        }
        if (value < TRIG_HIGHEST_VALUE * 2) {
            value++;
            isInversed = value > TRIG_HIGHEST_VALUE;
            note->trigg = create2FByte(
                value > 0,
                isInversed,
                isInversed ? (value - TRIG_HIGHEST_VALUE) : value
            );
        }
    } else 
    if (key == BLIPR_KEY_13) { note->cc1Value = MAX(0, note->cc1Value - 1); } else 
    if (key == BLIPR_KEY_14) { note->cc1Value = MIN(127, note->cc1Value + 1); } else 
    if (key == BLIPR_KEY_15) { note->cc2Value = MAX(0, note->cc2Value - 1); } else 
    if (key == BLIPR_KEY_16) { note->cc2Value = MIN(127, note->cc2Value + 1); } else 
    if (key == BLIPR_KEY_C) {
        // TODO: Copy (once=copy note, twice=copy step)
    } else if (key == BLIPR_KEY_D) {
        // TODO: Paste
    } 
}

/**
 * Update the sequencer according to user input
 */
void updateSequencer(
    struct Track *selectedTrack,
    bool keyStates[SDL_NUM_SCANCODES], 
    SDL_Scancode key
) {
    int index = scancodeToStep(key) + (selectedTrack->selectedPage * 16);
    if (keyStates[BLIPR_KEY_SHIFT_1]) {
        if (index >= 0) {
            // Toggle velocity
            toggleVelocity(&selectedTrack->steps[index]);
        } else {
            // this is one of the bottom buttons, change page bank, for poly 2 to 8, this is:
            // A=page 1,2,3,4       (bank 0)
            // B=page 5,6,7,8       (bank 1)
            // C=page 9,10,11,12    (bank 2)
            // D=page 13,14,15,16   (bank 3)
            // for monophonic (512 steps = 32 pages, this is)
            // A=page 1,2,3,4       / 5,6,7,8       (bank 0 & 4)
            // B=page 9,10,11,12    / 13,14,15,16   (bank 1 & 5)
            // C=page 17,18,19,20   / 21,22,23,24   (bank 2 & 6)
            // D=page 25,26,27,28   / 29,30,31,32   (bank 3 & 7)
            if (getPolyCount(selectedTrack) == 1) {
                if (key == BLIPR_KEY_A) { selectedTrack->selectedPageBank == 0 ? 4 : 0; } else
                if (key == BLIPR_KEY_B) { selectedTrack->selectedPageBank == 1 ? 5 : 1; } else
                if (key == BLIPR_KEY_C) { selectedTrack->selectedPageBank == 2 ? 6 : 2; } else
                if (key == BLIPR_KEY_D) { selectedTrack->selectedPageBank == 3 ? 7 : 3; }
            } else {
                if (key == BLIPR_KEY_A) { selectedTrack->selectedPageBank = 0; } else
                if (key == BLIPR_KEY_B) { selectedTrack->selectedPageBank = 1; } else
                if (key == BLIPR_KEY_C) { selectedTrack->selectedPageBank = 2; } else
                if (key == BLIPR_KEY_D) { selectedTrack->selectedPageBank = 3; }
            }
        }
    } else if(keyStates[BLIPR_KEY_SHIFT_2]) {
        // Update selected note or page:
        if (index >= 0 && selectedStep == -1) {
            // Select step
            selectedStep = index;
        } else if (selectedStep == -1) {
            // No step selected, but also no index, so this is one of the bottom buttons
            // Change selected note:
            if (key == BLIPR_KEY_C) { 
                selectedNote = MAX(0, selectedNote - 1); 
            } else if (key == BLIPR_KEY_D) { 
                selectedNote = MIN(getPolyCount(selectedTrack) - 1, selectedNote + 1); 
            }
        } else {
            // Handle key input with selected step
            handleKey(selectedTrack, key);
        }
    } else if(key == BLIPR_KEY_A) {
        setSelectedPage(selectedTrack, 0);
    } else if(key == BLIPR_KEY_B) {
        setSelectedPage(selectedTrack, 1);
    } else if(key == BLIPR_KEY_C) {
        setSelectedPage(selectedTrack, 2);
    } else if(key == BLIPR_KEY_D) {
        setSelectedPage(selectedTrack, 3);
    } else {
        // Default stp pressed
        if (index >= 0) {
            // Toggle key
            toggleStep(&selectedTrack->steps[index]);
        }
    }
}

/**
 * Check sequencer for key repeats
 */
void checkSequencerForKeyRepeats(
    struct Track *selectedTrack,
    bool keyStates[SDL_NUM_SCANCODES]
) {
    if(keyStates[BLIPR_KEY_SHIFT_2]) {
        if (keyStates[BLIPR_KEY_1]) { handleKey(selectedTrack, BLIPR_KEY_1); } else 
        if (keyStates[BLIPR_KEY_2]) { handleKey(selectedTrack, BLIPR_KEY_2); } else 
        if (keyStates[BLIPR_KEY_3]) { handleKey(selectedTrack, BLIPR_KEY_3); } else 
        if (keyStates[BLIPR_KEY_4]) { handleKey(selectedTrack, BLIPR_KEY_4); } else 
        if (keyStates[BLIPR_KEY_5]) { handleKey(selectedTrack, BLIPR_KEY_5); } else 
        if (keyStates[BLIPR_KEY_6]) { handleKey(selectedTrack, BLIPR_KEY_6); } else 
        if (keyStates[BLIPR_KEY_7]) { handleKey(selectedTrack, BLIPR_KEY_7); } else 
        if (keyStates[BLIPR_KEY_8]) { handleKey(selectedTrack, BLIPR_KEY_8); } else 
        if (keyStates[BLIPR_KEY_9]) { handleKey(selectedTrack, BLIPR_KEY_9); } else 
        if (keyStates[BLIPR_KEY_10]) { handleKey(selectedTrack, BLIPR_KEY_10); } else 
        if (keyStates[BLIPR_KEY_11]) { handleKey(selectedTrack, BLIPR_KEY_11); } else 
        if (keyStates[BLIPR_KEY_12]) { handleKey(selectedTrack, BLIPR_KEY_12); } else 
        if (keyStates[BLIPR_KEY_13]) { handleKey(selectedTrack, BLIPR_KEY_13); } else 
        if (keyStates[BLIPR_KEY_14]) { handleKey(selectedTrack, BLIPR_KEY_14); } else 
        if (keyStates[BLIPR_KEY_15]) { handleKey(selectedTrack, BLIPR_KEY_15); } else 
        if (keyStates[BLIPR_KEY_16]) { handleKey(selectedTrack, BLIPR_KEY_16); }        
    }
}

/**
 * Check if the note is triggered according to the trigg condition
 * @param triggValue    The Trigg value
 * @param repeatCount   How many times this note has already been played (determined by tracklength or page size)
 */
bool isNoteTrigged(int triggValue, int repeatCount) {
    bool isEnabled = triggValue > 0; // get2FByteFlag1(triggValue);
    if (!isEnabled) {
        // If a trig condition is not enabled it will always pass:
        return true;
    }

    bool isTrigged = true;
    bool isInversed = get2FByteFlag2(triggValue);
    int value = get2FByteValue(triggValue);
    
    switch (value) {
        case TRIG_DISABLED:
            isTrigged = true;
            break;
        case TRIG_1_2:
            isTrigged = repeatCount % 2 == 0;
            break;
        case TRIG_1_3:
            isTrigged = repeatCount % 3 == 0;
            break;
        case TRIG_2_3:
            isTrigged = repeatCount % 3 == 1;
            break;
        case TRIG_3_3:
            isTrigged = repeatCount % 3 == 2;
            break;
        case TRIG_1_4:
            isTrigged = repeatCount % 4 == 0;
            break;
        case TRIG_2_4:
            isTrigged = repeatCount % 4 == 1;
            break;
        case TRIG_3_4:
            isTrigged = repeatCount % 4 == 2;
            break;
        case TRIG_4_4:
            isTrigged = repeatCount % 4 == 3;
            break;
        case TRIG_1_5:
            isTrigged = repeatCount % 5 == 0;
            break;
        case TRIG_2_5:
            isTrigged = repeatCount % 5 == 1;
            break;
        case TRIG_3_5:
            isTrigged = repeatCount % 5 == 2;
            break;
        case TRIG_4_5:
            isTrigged = repeatCount % 5 == 3;
            break;
        case TRIG_5_5:
            isTrigged = repeatCount % 5 == 4;
            break;
        case TRIG_1_6:
            isTrigged = repeatCount % 6 == 0;
            break;
        case TRIG_2_6:
            isTrigged = repeatCount % 6 == 1;
            break;
        case TRIG_3_6:
            isTrigged = repeatCount % 6 == 2;
            break;
        case TRIG_4_6:
            isTrigged = repeatCount % 6 == 3;
            break;
        case TRIG_5_6:
            isTrigged = repeatCount % 6 == 4;
            break;
        case TRIG_6_6:
            isTrigged = repeatCount % 6 == 5;
            break;
        case TRIG_1_7:
            isTrigged = repeatCount % 7 == 0;
            break;
        case TRIG_2_7:
            isTrigged = repeatCount % 7 == 1;
            break;
        case TRIG_3_7:
            isTrigged = repeatCount % 7 == 2;
            break;
        case TRIG_4_7:
            isTrigged = repeatCount % 7 == 3;
            break;
        case TRIG_5_7:
            isTrigged = repeatCount % 7 == 4;
            break;
        case TRIG_6_7:
            isTrigged = repeatCount % 7 == 5;
            break;
        case TRIG_7_7:
            isTrigged = repeatCount % 7 == 6;
            break;
        case TRIG_1_8:
            isTrigged = repeatCount % 8 == 0;
            break;
        case TRIG_2_8:
            isTrigged = repeatCount % 8 == 1;
            break;
        case TRIG_3_8:
            isTrigged = repeatCount % 8 == 2;
            break;
        case TRIG_4_8:
            isTrigged = repeatCount % 8 == 3;
            break;
        case TRIG_5_8:
            isTrigged = repeatCount % 8 == 4;
            break;
        case TRIG_6_8:
            isTrigged = repeatCount % 8 == 5;
            break;
        case TRIG_7_8:
            isTrigged = repeatCount % 8 == 6;
            break;
        case TRIG_8_8:
            isTrigged = repeatCount % 8 == 7;
            break;
        case TRIG_1_PERCENT:
            isTrigged = rand() % 100 == 0;
            break;
        case TRIG_2_PERCENT:
            isTrigged = rand() % 50 == 0;
            break;
        case TRIG_5_PERCENT:
            isTrigged = rand() % 20 == 0;
            break;
        case TRIG_10_PERCENT:
            isTrigged = rand() % 10 == 0;
            break;
        case TRIG_25_PERCENT:
            isTrigged = rand() % 4 == 0;
            break;
        case TRIG_33_PERCENT:
            isTrigged = rand() % 3 == 0;
            break;
        case TRIG_50_PERCENT:
            isTrigged = rand() % 2 == 0;
            break;
        case TRIG_FIRST:
            isTrigged = repeatCount == 0;
            break;
    }

    return isInversed ? !isTrigged : isTrigged;
}

/**
 * Set the trigg text to a given string
 */
void setTriggText(int triggValue, char *text) {
    bool isEnabled = get2FByteFlag1(triggValue);
    if (!isEnabled) {
        sprintf(text, "OFF");
        return;
    }

    bool isTrigged = true;
    bool isInversed = get2FByteFlag2(triggValue);
    int value = get2FByteValue(triggValue);

    switch (value) {
        case TRIG_DISABLED:
            sprintf(text, "OFF");       
            case TRIG_1_2:
            sprintf(text, "%s1:2", isInversed? "!" : "");
            break;
        case TRIG_1_3:
            sprintf(text, "%s1:3", isInversed? "!" : "");
            break;
        case TRIG_2_3:
            sprintf(text, "%s2:3", isInversed? "!" : "");
            break;
        case TRIG_3_3:
            sprintf(text, "%s3:3", isInversed? "!" : "");
            break;
        case TRIG_1_4:
            sprintf(text, "%s1:4", isInversed? "!" : "");
            break;
        case TRIG_2_4:
            sprintf(text, "%s2:4", isInversed? "!" : "");
            break;
        case TRIG_3_4:
            sprintf(text, "%s3:4", isInversed? "!" : "");
            break;
        case TRIG_4_4:
            sprintf(text, "%s4:4", isInversed? "!" : "");
            break;
        case TRIG_1_5:
            sprintf(text, "%s1:5", isInversed? "!" : "");
            break;
        case TRIG_2_5:
            sprintf(text, "%s2:5", isInversed? "!" : "");
            break;
        case TRIG_3_5:
            sprintf(text, "%s3:5", isInversed? "!" : "");
            break;
        case TRIG_4_5:
            sprintf(text, "%s4:5", isInversed? "!" : "");
            break;
        case TRIG_5_5:
            sprintf(text, "%s5:5", isInversed? "!" : "");
            break;
        case TRIG_1_6:
            sprintf(text, "%s1:6", isInversed? "!" : "");
            break;
        case TRIG_2_6:
            sprintf(text, "%s2:6", isInversed? "!" : "");
            break;
        case TRIG_3_6:
            sprintf(text, "%s3:6", isInversed? "!" : "");
            break;
        case TRIG_4_6:
            sprintf(text, "%s4:6", isInversed? "!" : "");
            break;
        case TRIG_5_6:
            sprintf(text, "%s5:6", isInversed? "!" : "");
            break;
        case TRIG_6_6:
            sprintf(text, "%s6:6", isInversed? "!" : "");
            break;
        case TRIG_1_7:
            sprintf(text, "%s1:7", isInversed? "!" : "");
            break;
        case TRIG_2_7:
            sprintf(text, "%s2:7", isInversed? "!" : "");
            break;
        case TRIG_3_7:
            sprintf(text, "%s3:7", isInversed? "!" : "");
            break;
        case TRIG_4_7:
            sprintf(text, "%s4:7", isInversed? "!" : "");
            break;
        case TRIG_5_7:
            sprintf(text, "%s5:7", isInversed? "!" : "");
            break;
        case TRIG_6_7:
            sprintf(text, "%s6:7", isInversed? "!" : "");
            break;
        case TRIG_7_7:
            sprintf(text, "%s7:7", isInversed? "!" : "");
            break;
        case TRIG_1_8:
            sprintf(text, "%s1:8", isInversed? "!" : "");
            break;
        case TRIG_2_8:
            sprintf(text, "%s2:8", isInversed? "!" : "");
            break;
        case TRIG_3_8:
            sprintf(text, "%s3:8", isInversed? "!" : "");
            break;
        case TRIG_4_8:
            sprintf(text, "%s4:8", isInversed? "!" : "");
            break;
        case TRIG_5_8:
            sprintf(text, "%s5:8", isInversed? "!" : "");
            break;
        case TRIG_6_8:
            sprintf(text, "%s6:8", isInversed? "!" : "");
            break;
        case TRIG_7_8:
            sprintf(text, "%s7:8", isInversed? "!" : "");
            break;
        case TRIG_8_8:
            sprintf(text, "%s8:8", isInversed? "!" : "");
            break;
        case TRIG_1_PERCENT:
            strcpy(text, isInversed ? "99%" : "1%");
            break;
        case TRIG_2_PERCENT:
            strcpy(text, isInversed ? "98%" : "2%");
            break;
        case TRIG_5_PERCENT:
            strcpy(text, isInversed ? "95%" : "5%");
            break;
        case TRIG_10_PERCENT:
            strcpy(text, isInversed ? "90%" : "10%");
            break;
        case TRIG_25_PERCENT:
            strcpy(text, isInversed ? "75%" : "25%");
            break;
        case TRIG_33_PERCENT:
            strcpy(text, isInversed ? "66%" : "33%");
            break;
        case TRIG_50_PERCENT:
            strcpy(text, "50%");
            break;
        case TRIG_FILL:
            sprintf(text, "%sFILL", isInversed? "!" : "");
            break;
        case TRIG_FIRST:
            sprintf(text, "%s1ST", isInversed? "!" : "");
            break;
        case TRIG_TRANSITION:
            sprintf(text, "%sTRANS", isInversed? "!" : "");
            break;
        case TRIG_FIRST_PAGE:
            sprintf(text, "%s1ST.P", isInversed? "!" : "");
            break;
        case TRIG_TRANSITION_PAGE:
            sprintf(text, "%sTRN.P", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_FIRST:
            sprintf(text, "%s1ST.H", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_TRANSITION:
            sprintf(text, "%sTRN.H", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_FIRST_PAGE:
            sprintf(text, "%s1ST.HP", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_TRANSITION_PAGE:
            sprintf(text, "%sTRN.HP", isInversed? "!" : "");
            break;
        case TRIG_LOWER_FIRST:
            sprintf(text, "%s1ST.L", isInversed? "!" : "");
            break;
        case TRIG_LOWER_TRANSITION:
            sprintf(text, "%sTRN.L", isInversed? "!" : "");
            break;
        case TRIG_LOWER_FIRST_PAGE:
            sprintf(text, "%s1ST.LP", isInversed? "!" : "");
            break;
        case TRIG_LOWER_TRANSITION_PAGE:
            sprintf(text, "%sTRN.LP", isInversed? "!" : "");
            break;
        case TRIG_FIRST_PAGE_PLAY:
            sprintf(text, "%s1ST.PG", isInversed? "!" : "");
            break;
        case TRIG_FIRST_PATTERN_PLAY:
            sprintf(text, "%s1ST.PT", isInversed? "!" : "");
            break;
        default:
            strcpy(text, "---");
            break;
    }
}

/**
 * Get track step index - this is the index in the steps-array on the track
 * The property "isFirstPulseCallback" is called the first pulse of the step
 */
int getTrackStepIndex(
    const uint64_t *ppqnCounter, 
    const struct Track *track, 
    void (*isFirstPulseCallback)(void)
) {
    uint64_t clampedCounter;
    if (track->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        // By track:
        clampedCounter = *ppqnCounter % (PP16N * (track->trackLength + 1));
        // Set isFirstPulse-flag
        if (isFirstPulseCallback != NULL && clampedCounter == 0) {
            isFirstPulseCallback();
        }
    } else {
        // By page:
        int pageLength = track->pageLength + 1; // 1-16
        clampedCounter = *ppqnCounter % (PP16N * pageLength);
        // We need a callback here, because selectedPage might be changed to queued page, affecting the next note to be played
        if (isFirstPulseCallback != NULL && clampedCounter == 0) {
            isFirstPulseCallback();
        }

        // Increase clamped counter with selected page:
        clampedCounter += ((track->selectedPage % 4) * (PP16N * 16));   // 16 steps
    }

    return clampedCounter / PP16N;
}

/**
 * Get notes at a given track index - this takes into account the polyphony sacrifice for more steps
 */
void getNotesAtTrackStepIndex(int trackStepIndex, const struct Track *track, struct Note **notes) {
    switch (getPolyCount(track)) {
        case 8:
            // All enabled notes for this step
            for (int i=0; i<8; i++) {
                notes[i] = &track->steps[trackStepIndex].notes[i];
            }
            break;
        case 4:
            for (int i=0; i<4; i++) {
                int targetIndex = (track->selectedPageBank * 4) + (i % 4);
                notes[i] = &track->steps[trackStepIndex].notes[targetIndex];
            }
            break;
        case 2:
            for (int i=0; i<2; i++) {
                int targetIndex = (track->selectedPageBank * 2) + (i % 2);
                notes[i] = &track->steps[trackStepIndex].notes[targetIndex];
            }
            break;
        case 1:
            for (int i=0; i<1; i++) {
                int targetIndex = track->selectedPageBank;
                notes[0] = &track->steps[trackStepIndex].notes[targetIndex];
            }
            break;
    }
}

/**
 * Helper method to contain all the conditions that define if a note is played or not
 */
bool isNotePlayed(
    const struct Note *note,
    const struct Track *track,
    int nudgeCheck
) {
    return 
        note != NULL &&
        note->enabled && 
        (note->nudge - PP16N) == nudgeCheck && 
        isNoteTrigged(note->trigg, track->repeatCount);
}

/**
 * Standalone method to process a pulse
 */
void processPulse(
    const uint64_t *currentPulse,
    const struct Track *track,
    void (*isFirstPulseCallback)(void),
    void (*playNoteCallback)(const struct Note *note)
) {
    // Get the index for the current step
    int trackStepIndex = getTrackStepIndex(currentPulse, track, isFirstPulseCallback);

    // Get the nudge value that needs to be applied:
    int nudgeCheck = *currentPulse % PP16N;

    // Check for shuffle:
    if (trackStepIndex % 2 == 1) {
        nudgeCheck -= (track->shuffle - PP16N);
    }

    // Get all notes that are in this step
    int polyCount = getPolyCount(track);
    struct Note *notes[polyCount];
    getNotesAtTrackStepIndex(trackStepIndex, track, notes);

    // Iterate over all notes in this step:
    for (int i=0; i < polyCount; i++) {
        // Get the note:
        struct Note *note = notes[i];

        // If it's not null and matches the note boundaries it's viable for playing:
        if (isNotePlayed(note, track, nudgeCheck)) {
            playNoteCallback(note);
        }
    }

    // Check next step as well, because there might be notes with negative nudges:
    int nextNudgeCheck = (PP16N * -1) + (*currentPulse % PP16N);
    // Ignore next nudge check of it's equal to PP16N * -1, because that equals the current step and would send a double note
    if (nextNudgeCheck != PP16N * -1) {
        uint64_t nextStepPulse = *currentPulse + PP16N;
        int nextTrackStepIndex = getTrackStepIndex(&nextStepPulse, track, NULL);

        // Check for shuffle:
        if (nextTrackStepIndex % 2 == 1) {
            nextNudgeCheck -= (track->shuffle - PP16N);
        }

        struct Note *nextStepNotes[polyCount];
        getNotesAtTrackStepIndex(nextTrackStepIndex, track, nextStepNotes);
        for (int i=0; i < polyCount; i++) {
            // Get the note:
            struct Note *note = nextStepNotes[i];

            if (isNotePlayed(note, track, nextNudgeCheck)) {
                playNoteCallback(note);
            }
        }
    }
}

// Global properties used in callbacks:
static PmStream *tmpStream;
static struct Track *tmpTrack;

/**
 * Callback when the first pulse of a page/track is played
 * Note that this is AFTER the note has already played
 */
void isFirstPulseCallback() {
    tmpTrack->isFirstPulse = true;
    if (tmpTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        tmpTrack->repeatCount += 1;
    } else {
        if (tmpTrack->selectedPage != tmpTrack->queuedPage) {
            tmpTrack->selectedPage = tmpTrack->queuedPage;
            // Reset repeat count, since we're switching pages:
            tmpTrack->repeatCount = 0;
        } else {
            tmpTrack->repeatCount += 1;
        }
    }
}

/**
 * Callback when a note is played
 */
void playNoteCallback(const struct Note *note) {
    sendMidiNoteOn(tmpStream, tmpTrack->midiChannel, note->note, note->velocity);
    addNoteToTracker(tmpStream, tmpTrack->midiChannel, note);
}

/**
 * Apply track speed to pulse
 * returns FALSE if further processing is not required
 */
bool applySpeedToPulse(
    const struct Track *track,
    uint64_t *pulse
) {
    switch (track->speed) {
        case TRACK_SPEED_DIV_TWO:
            // Only process when modulo is 0 (to prevent doubles)
            if (*pulse % 2 != 0) {
                *pulse /= 2;
                return false;
            }
            *pulse /= 2;
            break;
        case TRACK_SPEED_DIV_FOUR:
            // Only process when modulo is 0 (to prevent doubles)
            if (*pulse % 4 != 0) {
                *pulse /= 4;
                return false;
            }
            *pulse /= 4;
            break;
        case TRACK_SPEED_DIV_EIGHT:
            // Only process when modulo is 0 (to prevent doubles)
            if (*pulse % 8 != 0) {
                *pulse /= 8;
                return false;
            }        
            *pulse /= 8;
            break;
        case TRACK_SPEED_TIMES_TWO:
            *pulse *= 2;
            break;
        case TRACK_SPEED_TIMES_FOUR:
            *pulse *= 4;
            break;
        case TRACK_SPEED_TIMES_EIGHT:
            *pulse *= 8;
            break;
    }

    return true;
}

/**
 * Run the sequencer
 * @todo refactor this so it can be testable
 */
void runSequencer(
    PmStream *outputStream,
    const uint64_t *ppqnCounter, 
    struct Track *selectedTrack
) {
    // Reset global properties:
    selectedTrack->isFirstPulse = false;
    tmpStream = outputStream;
    tmpTrack = selectedTrack;

    // Check track speed (we do this by manupulating the pulse):
    uint64_t pulse = *ppqnCounter;
    bool process = applySpeedToPulse(selectedTrack, &pulse);
    if (!process) {
        return;
    }

    // Process pulse
    processPulse(&pulse, selectedTrack, isFirstPulseCallback, playNoteCallback);

    // Check for repeats:
    // if (selectedTrack->isFirstPulse) {
    //     selectedTrack->repeatCount += 1;
        // if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        //     selectedTrack->repeatCount += 1;
        // } else {
        //     if (selectedTrack->selectedPage != selectedTrack->queuedPage) {
        //         selectedTrack->selectedPage = selectedTrack->queuedPage;
        //         // Reset repeat count, since we're switching pages:
        //         selectedTrack->repeatCount = 0;
        //     } else {
        //         selectedTrack->repeatCount += 1;
        //     }
        // }
    // }

    // Decrease note-off counters:
    updateNotesAndSendOffs(outputStream, selectedTrack->midiChannel);
}

/**
 * Draw the sequencer
 */
void drawSequencerMain(
    uint64_t *ppqnCounter, 
    bool keyStates[SDL_NUM_SCANCODES],
    struct Track *selectedTrack
) {
    // Outline currently active step:
    int width = HEIGHT / 6;
    int height = width;

    // Step indicator:
    int playingPage = 0;

    // Check track speed (we do this by manupulating the pulse):
    uint64_t pulse = *ppqnCounter;
    applySpeedToPulse(selectedTrack, &pulse);
    int trackStepIndex = getTrackStepIndex(&pulse, selectedTrack, NULL);

    if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        playingPage = trackStepIndex / 16;
        // Outline current page:
        drawHighlightedGridTile(selectedTrack->selectedPage + 16);
    } else {
        playingPage = selectedTrack->selectedPage;
        // Outline queued page:
        drawHighlightedGridTile(selectedTrack->queuedPage + 16);
    }

    // Draw outline on currently playing note:
    if (
        playingPage >= selectedTrack->selectedPage && playingPage < selectedTrack->selectedPage + 1
    ) {
            int x = trackStepIndex % 4;
            int y = (trackStepIndex / 4) % 4;
            drawSingleLineRectOutline(
            2 + x + (x * width),
            2 + y + (y * height),
            width,
            height,
            COLOR_WHITE
        );
    }
   
    // Highlight playing page:
    drawLine(
        2 + playingPage + (playingPage * BUTTON_WIDTH),
        HEIGHT - BUTTON_HEIGHT - 5,
        2 + playingPage + ((playingPage + 1) * BUTTON_WIDTH) - 1,
        HEIGHT - BUTTON_HEIGHT - 5,
        COLOR_RED
    );

    // Highlight non-empty steps:
    for (int j = 0; j < 4; j++) {
        int height = width;
        for (int i = 0; i < 4; i++) {
            struct Step step = selectedTrack->steps[(i + (j * 4)) + (selectedTrack->selectedPage * 16)];
            // Check if this is within the track length, or outside the page length:
            if (
                ((selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) && (selectedTrack->selectedPage * 16) + i + (j * 4) > selectedTrack->trackLength) ||
                ((selectedTrack->pagePlayMode == PAGE_PLAY_MODE_REPEAT) && (i + (j * 4)) > selectedTrack->pageLength)
            ) {
                // No note here
                drawRect(
                    4 + i + (i * width),
                    4 + j + (j * height),
                    width - 4,
                    height - 4,
                    COLOR_GRAY
                );
            } else {
                if (step.notes[selectedNote].enabled) {
                    drawRect(
                        4 + i + (i * width),
                        4 + j + (j * height),
                        width - 4,
                        height - 4,
                        step.notes[selectedNote].velocity >= defaultVelocity ? COLOR_RED : COLOR_DARK_RED
                    );
                    drawTextOnButton((i + (j * 4)), getMidiNoteName(step.notes[selectedNote].note));
                }
            }
        }
    }   

    // ABCD Buttons:
    if (keyStates[BLIPR_KEY_SHIFT_1]) {
        // Show page bank numbers:
        int polyCount = getPolyCount(selectedTrack);
        if (polyCount == 1) {
            char descriptions[4][4] = {"1/5", "2/6", "3/7", "4/8"};
            drawABCDButtons(descriptions);
        } else if (polyCount == 2) {
            char descriptions[4][4] = {"1", "2", "3", "4"};
            drawABCDButtons(descriptions);    
        } else if (polyCount == 4) {
            char descriptions[4][4] = {"1", "2", "-", "-"};
            drawABCDButtons(descriptions);    
        } else {
            // 8 poly only has 1 page bank
            char descriptions[4][4] = {"1", "-", "-", "-"};
            drawABCDButtons(descriptions);    
        }
    } else if (keyStates[BLIPR_KEY_SHIFT_2]) {
        // Note (for polyphony)
        char descriptions[4][4] = {"-", "-", "<", ">"};
        drawABCDButtons(descriptions);
    } else {
        // Page numbers:
        char descriptions[4][4] = {"P00", "P00", "P00", "P00"};
        int startPage = (selectedTrack->selectedPageBank * 4) + 1;
        
        // Fill the descriptions array
        for (int i = 0; i < 4; i++) {
            int pageNumber = startPage + i;
            // Format with leading zero if needed
            if (pageNumber < 10) {
                sprintf(descriptions[i], "P0%d", pageNumber);
            } else {
                sprintf(descriptions[i], "P%d", pageNumber);
            }
        }

        drawABCDButtons(descriptions);
    }
}

/**
 * Draw the step editor
 */
void drawStepEditor(struct Step *step) {
    /*
        - 1-4   : Transpose -12 / -1 / +1 / +12
        - 5-6   : Increase / decrease velocity
        - 7-8   : Increase / decrease length
        - 9-10  : Increase / decrease nudge
        - 11-12 : Increase / decrease trig condition
        - 13-14 : Increase / decrease CC1 value
        - 15-16 : Increase / decrease CC2 value    
    */

    // Title:
    drawCenteredLine(2, 133, "STEP OPTIONS", TITLE_WIDTH, COLOR_WHITE);

    struct Note *note = &step->notes[selectedNote];

    // Transpose:
    drawCenteredLine(2, 7, "TRANSPOSE", TITLE_WIDTH, COLOR_WHITE);
    char *midiNoteName = getMidiNoteName(note->note);
    drawCenteredLine(2, 22, midiNoteName, TITLE_WIDTH, COLOR_YELLOW);
    drawTextOnButton(0, "-12");
    drawTextOnButton(1, "-1");
    drawTextOnButton(2, "+1");
    drawTextOnButton(3, "+12");

    // Velocity:
    drawCenteredLine(2, 37, "VELOCITY", BUTTON_WIDTH * 2, COLOR_WHITE);
    char velocityChar[4];
    snprintf(velocityChar, sizeof(velocityChar), "%d", note->velocity);
    drawCenteredLine(2, 47, velocityChar, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(4, "-");
    drawTextOnButton(5, "+");

    // Length:
    drawCenteredLine(62, 37, "LENGTH", BUTTON_WIDTH * 2, COLOR_WHITE);
    char lengthChar[4];
    snprintf(lengthChar, sizeof(lengthChar), "%d", note->length);
    drawCenteredLine(62, 47, lengthChar, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(6, "-");
    drawTextOnButton(7, "+");

    // Nudge:
    drawCenteredLine(2, 67, "NUDGE", BUTTON_WIDTH * 2, COLOR_WHITE);
    char nudgeChar[4];
    snprintf(nudgeChar, sizeof(nudgeChar), "%d", note->nudge - PP16N);
    drawCenteredLine(2, 77, nudgeChar, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(8, "-");
    drawTextOnButton(9, "+");

    // Trig:
    drawCenteredLine(62, 67, "TRIGG", BUTTON_WIDTH * 2, COLOR_WHITE);
    char triggChar[4];
    snprintf(
        triggChar, 
        sizeof(triggChar), 
        "%d", 
        get2FByteValue(note->trigg) + (get2FByteFlag2(note->trigg) ? TRIG_HIGHEST_VALUE : 0)
    );
    drawCenteredLine(62, 77, triggChar, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(10, "-");
    drawTextOnButton(11, "+");
    // Trigg description:
    char triggText[6];
    setTriggText(note->trigg, triggText);
    drawCenteredLine(62, 87, triggText, BUTTON_WIDTH * 2, COLOR_WHITE);

    // CC1:
    drawCenteredLine(2, 97, "CC1 VALUE", BUTTON_WIDTH * 2, COLOR_WHITE);
    char cc1Value[4];
    snprintf(cc1Value, sizeof(cc1Value), "%d", note->cc1Value);
    drawCenteredLine(2, 107, cc1Value, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(12, "-");
    drawTextOnButton(13, "+");

    // CC2:
    drawCenteredLine(62, 97, "CC2 VALUE", BUTTON_WIDTH * 2, COLOR_WHITE);
    char cc2Value[4];
    snprintf(cc2Value, sizeof(cc2Value), "%d", note->cc2Value);
    drawCenteredLine(62, 107, cc2Value, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(14, "-");
    drawTextOnButton(15, "+");

    // Step selected, show copy paste options:
    char descriptions[4][4] = {"-", "-", "CPY", "PST"};
    drawABCDButtons(descriptions);
}

void drawSequencer(
    uint64_t *ppqnCounter, 
    bool keyStates[SDL_NUM_SCANCODES],
    struct Track *selectedTrack
) {
    if (selectedStep == -1) {
        drawSequencerMain(ppqnCounter, keyStates, selectedTrack);
    } else {
        drawStepEditor(&selectedTrack->steps[selectedStep]);
    }
}


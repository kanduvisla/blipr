#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include <string.h>
#include <cstdio>
#include <functional>
#include "sequencer.hpp"
#include "../project.h"
#include "../constants.h"
#include "../colors.h"
#include "../drawing.h"
#include "../drawing_components.h"
#include "../drawing_text.h"
#include "../utils.h"
#include "../midi.h"
#include "../print.h"
#include "../drawing_icons.h"

// TODO: These should be the global things I guess?

// Pages can be global, or per track configured
static int selectedNote = 0;
static int selectedPageBank = 0;               // Selected page bank, not currently active playing page bank
static bool selectedSteps[16] = {false};       // Boolean that determines if this step is selected or not
static struct Step* clipBoard[16] = {NULL};
static bool isNoteEditorVisible = false;
static int cutCounter = 0;     // 0=none   1=note  2=step (all notes)
static int copyCounter = 0;
static bool isEditOnAllNotes = false;

// Boolean arrays that determine if for a step all note properties are the same:
#define PROPERTY_CC1 0
#define PROPERTY_CC2 1
#define PROPERTY_NOTE 2
#define PROPERTY_LENGTH 3
#define PROPERTY_VELOCITY 4
#define PROPERTY_NUDGE 5
#define PROPERTY_TRIG 6
#define PROPERTY_ENABLED 7

static bool areAllStepPropertiesTheSame[8] = {false};

// Template note that is used for pipet tooling
static struct Note templateNote;

// Constructor
Sequencer::Sequencer() {}

// Destructor
Sequencer::~Sequencer() {}

/**
 * Method to check if all step properties are the same
 */
void Sequencer::checkIfAllStepPropertiesAreTheSame(const struct Track *track) {
    for (int i=0; i<8; i++) {
        areAllStepPropertiesTheSame[i] = true;
    }

    // Iterate over the selected steps, and check if there is difference in properties of the selected steps:
    for (int i=1; i<16; i++) {
        if (selectedSteps[i] && selectedSteps[i - 1]) {
            int stepIndex = i + (track->selectedPage * 16);
            const struct Step *lhStep = &track->steps[stepIndex - 1];
            const struct Step *rhStep = &track->steps[stepIndex];
            areAllStepPropertiesTheSame[PROPERTY_CC1] &= lhStep->notes[selectedNote].cc1Value == rhStep->notes[selectedNote].cc1Value;
            areAllStepPropertiesTheSame[PROPERTY_CC2] &= lhStep->notes[selectedNote].cc2Value == rhStep->notes[selectedNote].cc2Value;
            areAllStepPropertiesTheSame[PROPERTY_NOTE] &= lhStep->notes[selectedNote].note == rhStep->notes[selectedNote].note;
            areAllStepPropertiesTheSame[PROPERTY_LENGTH] &= lhStep->notes[selectedNote].length == rhStep->notes[selectedNote].length;
            areAllStepPropertiesTheSame[PROPERTY_VELOCITY] &= lhStep->notes[selectedNote].velocity == rhStep->notes[selectedNote].velocity;
            areAllStepPropertiesTheSame[PROPERTY_NUDGE] &= lhStep->notes[selectedNote].nudge == rhStep->notes[selectedNote].nudge;
            areAllStepPropertiesTheSame[PROPERTY_TRIG] &= lhStep->notes[selectedNote].trigg == rhStep->notes[selectedNote].trigg;
            areAllStepPropertiesTheSame[PROPERTY_ENABLED] &= lhStep->notes[selectedNote].enabled == rhStep->notes[selectedNote].enabled;
        }
    }
}

/**
 * Check if there are steps selected
 */
bool Sequencer::isStepsSelected() {
    int count = 0;
    for (int i=0; i<16; i++) {
        if (selectedSteps[i]) {
            count ++;
        }
    }
    return count > 0;
}

/**
 * Reset the selected step
 */
void Sequencer::resetSequencerSelectedStep() {
    for (int i=0; i<16; i++) {
        selectedSteps[i] = false;
    }
    isNoteEditorVisible = false;
    cutCounter = 0;
    copyCounter = 0;
}

/**
 * Reset the selected note
 */
void Sequencer::resetSelectedNote() {
    selectedNote = 0;
}

/**
 * Clear note
 */
void Sequencer::clearNote(struct Note *note) {
    note->cc1Value = 0;
    note->cc2Value = 0;
    note->enabled = false;
    note->length = 0;
    note->velocity = 0;
    note->trigg = 0;
    note->note = 0;
    note->nudge = PP16N;
}

/**
 * Clear step
 */
void Sequencer::clearStep(struct Step *step) {
    if (cutCounter == 0) {
        // This is the first cut, clear only the selected note:
        clearNote(&step->notes[selectedNote]);
    } else {
        // This is the second cut, clear all notes in this step
        for (int i=0; i<NOTES_IN_STEP; i++) {
            clearNote(&step->notes[i]);
        }
    }
}

/**
 * Copy note
 */
void Sequencer::copyNote(const struct Note *src, struct Note *dst) {
    dst->cc1Value = src->cc1Value;
    dst->cc2Value = src->cc2Value;
    dst->enabled = src->enabled;
    dst->length = src->length;
    dst->velocity = src->velocity;
    dst->trigg = src->trigg;
    dst->note = src->note;
    dst->nudge = src->nudge;
}

/**
 * Copy step
 */
void Sequencer::copyStep(const struct Step *src, struct Step *dst) {
    if (copyCounter == 0) {
        // This is the first copy, only copy the selected note:
        copyNote(&src->notes[selectedNote], &dst->notes[selectedNote]);
    } else {
        // This is the second copy, copy all the notes:
        for (int i=0; i<NOTES_IN_STEP; i++) {
            copyNote(&src->notes[i], &dst->notes[i]);
        }
    }
}

/**
 * Reset template note to default values
 */
void Sequencer::resetTemplateNote() {
    templateNote.enabled = false;
    templateNote.note = 60;     // C-4
    templateNote.velocity = 100;
    templateNote.length = 1;
    templateNote.nudge = PP16N; // PP16N = middle, nudge 0
    templateNote.trigg = 0;
    templateNote.cc1Value = 0;
    templateNote.cc2Value = 0;
}

/**
 * Toggle a step
 */
void Sequencer::toggleStep(struct Step *step, int noteIndex) {
    step->notes[noteIndex].enabled = !step->notes[noteIndex].enabled;
    // Set default values:
    if (step->notes[noteIndex].enabled) {
        // Use template note
        copyNote(&templateNote, &step->notes[noteIndex]);
        // Reset enabled state, because template note might be a disabled note :-/
        step->notes[noteIndex].enabled = true;
    }
}

/**
 * Toggle a step when in drumkit mode
 */
void Sequencer::toggleDrumkitStep(struct Step *step, int noteIndex) {
    if (step->notes[noteIndex].enabled) {
        // Decrease velocity:
        if (step->notes[noteIndex].velocity > 100) {
            step->notes[noteIndex].velocity = 100;
        } else if (step->notes[noteIndex].velocity > 50) {
            step->notes[noteIndex].velocity = 50;
        } else if (step->notes[noteIndex].velocity > 25) {
            step->notes[noteIndex].velocity = 25;
        } else {
            step->notes[noteIndex].enabled = false;    
        }
    } else {
        step->notes[noteIndex].enabled = true;
        // Use template note
        copyNote(&templateNote, &step->notes[noteIndex]);
        // Reset enabled state, because template note might be a disabled note :-/
        step->notes[noteIndex].enabled = true;
    }
}

/**
 * Set the proper template note for the drumkit sequencer
 * @TODO: Does this need to be fetched from the configuration? Or is this per pattern / track (seems cumbersome)
 */
void Sequencer::setTemplateNoteForDrumkitSequencer(const struct Track *track, int index) {
    // Also set selected note
    int polyCount = getPolyCount(track);
    templateNote.velocity = 100;
    switch(index) {
        case 0:
            // Kick
            selectedNote = 0;
            templateNote.note = 36;
            break;
        case 1:
            // Snare
            selectedNote = 1;
            templateNote.note = 40;
            break;
        case 2:
            // Clap
            selectedNote = polyCount == 8 ? 2 : 1;
            templateNote.note = 39;
            break;
        case 3:
            // Rimshot
            selectedNote = 1;
            templateNote.note = 37;
            break;
        case 4:
            // C.Hat 1
            selectedNote = polyCount == 8 ? 3 : (polyCount == 4 ? 2 : 1);
            templateNote.note = 42;
            break;
        case 5:
            // C.Hat 2
            selectedNote = polyCount == 8 ? 3 : (polyCount == 4 ? 2 : 1);
            templateNote.note = 44;
            break;
        case 6:
            // O.Hat
            selectedNote = polyCount == 8 ? 4 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 46;
            break;
        case 7:
            // Ride
            selectedNote = polyCount == 8 ? 4 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 51;
            break;
        case 8:
            // Crash
            selectedNote = polyCount == 8 ? 4 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 55;
            break;
        case 9:
            // L.Tom
            selectedNote = polyCount == 8 ? 5 : 1;
            templateNote.note = 45;
            break;
        case 10:
            // M.Tom
            selectedNote = polyCount == 8 ? 5 : 1;
            templateNote.note = 48;
            break;
        case 11:
            // H.Tom
            selectedNote = polyCount == 8 ? 5 : 1;
            templateNote.note = 50;
            break;
        case 12:
            // Cowbell
            selectedNote = polyCount == 8 ? 6 : 1;
            templateNote.note = 56;
            break;
        case 13:
            // Extra 1
            selectedNote = polyCount == 8 ? 6 : 0;
            templateNote.note = 71;
            break;
        case 14:
            // Extra 2
            selectedNote = polyCount == 8 ? 7 : (polyCount == 4 ? 2 : 1);
            templateNote.note = 73;
            break;
        case 15:
            // Extra 3
            selectedNote = polyCount == 8 ? 7 : (polyCount == 4 ? 3 : 1);
            templateNote.note = 75;
            break;
        default:
            // Nothing
            break;
    }

    if (polyCount == 1) {
        selectedNote = 0;
    }
}

/**
 * Set selected page
 */
void Sequencer::setSelectedPage(struct Track *selectedTrack, int index) {
    if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        // Investigate: how is queuing working with continuous play?
        selectedTrack->selectedPage = index;
    } else {
        selectedTrack->queuedPage = index;
    }
}

/**
 * Transpose an amount of steps, clamped
 */
unsigned char Sequencer::transposeMidiNote(unsigned char midiNote, int steps) {
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
    templateNote.note = transposed;

    return (unsigned char)transposed;   
}

/**
 * Apply a key on a single note in the note editor
 */
void Sequencer::applyKeyToNote(struct Note *note, SDL_Scancode key, bool isDrumkitSequencer) {
    if (key == BLIPR_KEY_1) { 
        if (!isDrumkitSequencer) {
            note->note = transposeMidiNote(note->note, -12); 
        }
    } else if (key == BLIPR_KEY_2) { 
        if (!isDrumkitSequencer) {
            note->note = transposeMidiNote(note->note, -1); 
        }
    } else if (key == BLIPR_KEY_3) { 
        if (!isDrumkitSequencer) {
            note->note = transposeMidiNote(note->note, 1); 
        }
    } else if (key == BLIPR_KEY_4) { 
        if (!isDrumkitSequencer) {
            note->note = transposeMidiNote(note->note, 12); 
        }
    } else if (key == BLIPR_KEY_5) { note->velocity = MAX(0, note->velocity - 1); } else 
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
    if (key == BLIPR_KEY_16) { note->cc2Value = MIN(127, note->cc2Value + 1); }
}

/**
 * Handle a key in the note editor
 */
void Sequencer::handleKey(struct Track *selectedTrack, SDL_Scancode key, bool isDrumkitSequencer) {
    for (int i=0; i<16; i++) {
        if (selectedSteps[i]) {
            // We need to apply this key input on this step, but only on the selected note
            int stepIndex = i + (selectedTrack->selectedPage * 16);
            if (isEditOnAllNotes) {
                for (int j=0; j<NOTES_IN_STEP; j++) {
                    struct Note *note = &selectedTrack->steps[stepIndex].notes[j];
                    applyKeyToNote(note, key, isDrumkitSequencer);
                }
            } else {
                struct Note *note = &selectedTrack->steps[stepIndex].notes[selectedNote];
                applyKeyToNote(note, key, isDrumkitSequencer);
            }
        }
    }   
}

/**
 * Select all steps between the first and last selected step
 */
void Sequencer::sanitizeSelection(int indexPressed) {
    int first = -1;
    int last = -1;
    for (int i=0; i<16; i++) {
        if (selectedSteps[i] == true) {
            first = i;
            break;
        } 
    }

    for (int i=15; i>=0; i--) {
        if (selectedSteps[i] == true) {
            last = i;
            break;
        }
    }

    if (first != -1 && last != -1) {
        // Select from first to last
        for (int i=first; i<=last; i++) {
            selectedSteps[i] = true;
        }        
        // If the selected step is smaller than the greatest selected step, disable all after:
        if (indexPressed < last) {
            for (int i=indexPressed + 1; i<16; i++) {
                selectedSteps[i] = false;
            }
        }
    }
}

/**
 * Get the maximum number of page banks
 */
int Sequencer::getMaxPageBanks(const struct Track *track) {
    int polyCount = getPolyCount(track);
    // polycount 8=1 page bank
    // polycount 4=2 page banks
    // polycount 2=4 page banks
    // polycount 1=8 page banks
    int totalPageBanks = 9 - polyCount; // 1 or 8
    if (polyCount == 4) { totalPageBanks = 2; }
    if (polyCount == 2) { totalPageBanks = 4; }
    return totalPageBanks;
}

/**
 * Update the sequencer according to user input
 */
void Sequencer::update(
    struct Track *track,
    bool keyStates[SDL_NUM_SCANCODES], 
    SDL_Scancode key,
    bool isDrumkitSequencer
) {
    int index = scancodeToStep(key);
    if (keyStates[BLIPR_KEY_SHIFT_1]) {
        // Shift 1 = utils:
        // ^1 + 1-16 = Select (multiple) step(s)
        // ^1 + A    = Open note editor for step(s)
        // ^1 + B    = Cut notes/steps      press once=cut notes,   press twice=cut all notes on step
        // ^1 + C    = Copy notes/steps
        // ^1 + D    = Paste notes/steps    press once=paste notes, press twice=paste all notes on step
        if (!isNoteEditorVisible) {
            if (index >= 0) {
                // Set template note:
                copyNote(&track->steps[index + (track->selectedPage * 16)].notes[selectedNote], &templateNote);
                // Set selected steps for utilities:
                selectedSteps[index] = !selectedSteps[index];
                // Select all steps between the first and last selected step:
                sanitizeSelection(index);
                checkIfAllStepPropertiesAreTheSame(track);
                // If Shift2 is also down, immeadiately open the note editor for this step:
                if (keyStates[BLIPR_KEY_SHIFT_2]) {
                    isNoteEditorVisible = true;
                }
            } else {
                if (isStepsSelected()) {
                    if (key == BLIPR_KEY_A) {
                        // Open editor:
                        isNoteEditorVisible = true;
                    } else
                    if (key == BLIPR_KEY_B) {
                        // Cut all steps:
                        for (int i=0; i<16; i++) {
                            if (selectedSteps[i]) {
                                clearStep(&track->steps[i + (track->selectedPage * 16)]);
                            }
                        }
                        cutCounter ++;
                        // No resetSequencerSelectedStep(), because paste can be done twice
                    } else
                    if (key == BLIPR_KEY_C) {
                        // Copy steps:
                        for (int i=0; i<16; i++) {
                            if (selectedSteps[i]) {
                                clipBoard[i] = &track->steps[i + (track->selectedPage * 16)];
                            } else {
                                clipBoard[i] = NULL;
                            }
                        }
                        // Clear selection after copying to clipboard:
                        resetSequencerSelectedStep();
                    } else
                    if (key == BLIPR_KEY_D) {
                        // Paste steps (will be pasted on first step in selection):
                        int pastePosition = 0;
                        for (int i=0; i<16; i++) {
                            if (selectedSteps[i]) {
                                pastePosition = i;
                                break;
                            }
                        }
                        printLog("paste position is %d", pastePosition);
                        // Paste copied steps:
                        // Subtract the first empty entries of the clipboard:
                        int offset = 0;
                        for (int i=0; i<16; i++) {
                            if (clipBoard[i] != NULL) {
                                // a step is found, so we have our offset:
                                break;
                            }
                            offset++;
                        }
                        printLog("clipboard offset is %d", offset);
                        // Now paste until we reach a NULL object:
                        for (int i=offset; i<16; i++) {
                            if (clipBoard[i] != NULL) {
                                copyStep(
                                    clipBoard[i],
                                    &track->steps[pastePosition + (track->selectedPage * 16) + i - offset]
                                );
                                printLog("copied step %d from clipboard to position %d", i,pastePosition + (track->selectedPage * 16));
                            } else {
                                break;
                            }
                        }
                        copyCounter ++;
                        // No resetSequencerSelectedStep(), because paste can be done twice
                    }                    
                } else {
                    // #TODO No steps selected,
                    // cut, copy paste applies to whole track
                }
            }
        } else {
            // Note editor is visible, handle note editor keys:
            if (index >= 0) {
                handleKey(track, key, isDrumkitSequencer);
            } else {
                if (key == BLIPR_KEY_A) {
                    isEditOnAllNotes = isEditOnAllNotes == false;
                }
            }
        }
    } else if(keyStates[BLIPR_KEY_SHIFT_2]) {
        // Shift 2 = page selector / note selector
        // ^2 + 1-16 = select page
        // ^2 + A-B  = page bank -/+
        // ^2 + C-D  = note / channel -/+
        int polyCount = getPolyCount(track);

        // 1-16 = select page
        if (index >= 0) {
            // Only the drumkit sequencer has options here
            if (isDrumkitSequencer) {
                // Set template note to proper drumkit note:
                setTemplateNoteForDrumkitSequencer(track, index);
            }
        } else {
            // Bottom buttons:
            if (polyCount < 8) {
                if (key == BLIPR_KEY_A) {
                    selectedPageBank = MAX(0, track->playingPageBank - 1);
                } if (key == BLIPR_KEY_B) {
                    int totalPageBanks = getMaxPageBanks(track);
                    selectedPageBank = MIN(totalPageBanks - 1, track->playingPageBank + 1);
                }
                // isHighPageBankSelected = track->selectedPageBank >= 4;
            }
            // Drumkit sequencer has no options to manually select note
            if (!isDrumkitSequencer) {
                if (key == BLIPR_KEY_C) { 
                    selectedNote = MAX(0, selectedNote - 1); 
                } else if (key == BLIPR_KEY_D) { 
                    selectedNote = MIN(getPolyCount(track) - 1, selectedNote + 1); 
                }
            }   
        }
    } else if(key == BLIPR_KEY_A) {
        setSelectedPage(track, (selectedPageBank * 4));
    } else if(key == BLIPR_KEY_B) {
        setSelectedPage(track, (selectedPageBank * 4) + 1);
    } else if(key == BLIPR_KEY_C) {
        setSelectedPage(track, (selectedPageBank * 4) + 2);
    } else if(key == BLIPR_KEY_D) {
        setSelectedPage(track, (selectedPageBank * 4) + 3);
    } else {
        // Default step pressed
        if (index >= 0) {
            // Toggle key
            int polyCount = getPolyCount(track);
            int stepIndex = (index + (track->selectedPage * 16)) % 64;
            printLog("setting note %d on step %d", (track->playingPageBank * polyCount) + selectedNote, stepIndex);
            if (!isDrumkitSequencer) {
                toggleStep(&track->steps[stepIndex], (track->playingPageBank * polyCount) + selectedNote);
            } else {
                toggleDrumkitStep(&track->steps[stepIndex], (track->playingPageBank * polyCount) + selectedNote);
            }
        }
    }
}

void Sequencer::update(
    struct Track *track,
    bool keyStates[SDL_NUM_SCANCODES], 
    SDL_Scancode key
) {
    update(track, keyStates, key, false);
}

/**
 * Check sequencer for key repeats
 */
/*
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
*/

/**
 * Check if the note is triggered according to the trigg condition
 * @param triggValue    The Trigg value
 * @param repeatCount   How many times this note has already been played (determined by tracklength or page size)
 */
bool Sequencer::isNoteTrigged(int triggValue, int repeatCount) {
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
void Sequencer::setTriggText(int triggValue, char *text) {
    bool isEnabled = get2FByteFlag1(triggValue);
    if (!isEnabled) {
        snprintf(text, 4, "OFF");
        return;
    }

    bool isInversed = get2FByteFlag2(triggValue);
    int value = get2FByteValue(triggValue);

    switch (value) {
        case TRIG_DISABLED:
            snprintf(text, 4, "OFF");       
            case TRIG_1_2:
            snprintf(text, 4, "%s1:2", isInversed? "!" : "");
            break;
        case TRIG_1_3:
            snprintf(text, 4, "%s1:3", isInversed? "!" : "");
            break;
        case TRIG_2_3:
            snprintf(text, 4, "%s2:3", isInversed? "!" : "");
            break;
        case TRIG_3_3:
            snprintf(text, 4, "%s3:3", isInversed? "!" : "");
            break;
        case TRIG_1_4:
            snprintf(text, 4, "%s1:4", isInversed? "!" : "");
            break;
        case TRIG_2_4:
            snprintf(text, 4, "%s2:4", isInversed? "!" : "");
            break;
        case TRIG_3_4:
            snprintf(text, 4, "%s3:4", isInversed? "!" : "");
            break;
        case TRIG_4_4:
            snprintf(text, 4, "%s4:4", isInversed? "!" : "");
            break;
        case TRIG_1_5:
            snprintf(text, 4, "%s1:5", isInversed? "!" : "");
            break;
        case TRIG_2_5:
            snprintf(text, 4, "%s2:5", isInversed? "!" : "");
            break;
        case TRIG_3_5:
            snprintf(text, 4, "%s3:5", isInversed? "!" : "");
            break;
        case TRIG_4_5:
            snprintf(text, 4, "%s4:5", isInversed? "!" : "");
            break;
        case TRIG_5_5:
            snprintf(text, 4, "%s5:5", isInversed? "!" : "");
            break;
        case TRIG_1_6:
            snprintf(text, 4, "%s1:6", isInversed? "!" : "");
            break;
        case TRIG_2_6:
            snprintf(text, 4, "%s2:6", isInversed? "!" : "");
            break;
        case TRIG_3_6:
            snprintf(text, 4, "%s3:6", isInversed? "!" : "");
            break;
        case TRIG_4_6:
            snprintf(text, 4, "%s4:6", isInversed? "!" : "");
            break;
        case TRIG_5_6:
            snprintf(text, 4, "%s5:6", isInversed? "!" : "");
            break;
        case TRIG_6_6:
            snprintf(text, 4, "%s6:6", isInversed? "!" : "");
            break;
        case TRIG_1_7:
            snprintf(text, 4, "%s1:7", isInversed? "!" : "");
            break;
        case TRIG_2_7:
            snprintf(text, 4, "%s2:7", isInversed? "!" : "");
            break;
        case TRIG_3_7:
            snprintf(text, 4, "%s3:7", isInversed? "!" : "");
            break;
        case TRIG_4_7:
            snprintf(text, 4, "%s4:7", isInversed? "!" : "");
            break;
        case TRIG_5_7:
            snprintf(text, 4, "%s5:7", isInversed? "!" : "");
            break;
        case TRIG_6_7:
            snprintf(text, 4, "%s6:7", isInversed? "!" : "");
            break;
        case TRIG_7_7:
            snprintf(text, 4, "%s7:7", isInversed? "!" : "");
            break;
        case TRIG_1_8:
            snprintf(text, 4, "%s1:8", isInversed? "!" : "");
            break;
        case TRIG_2_8:
            snprintf(text, 4, "%s2:8", isInversed? "!" : "");
            break;
        case TRIG_3_8:
            snprintf(text, 4, "%s3:8", isInversed? "!" : "");
            break;
        case TRIG_4_8:
            snprintf(text, 4, "%s4:8", isInversed? "!" : "");
            break;
        case TRIG_5_8:
            snprintf(text, 4, "%s5:8", isInversed? "!" : "");
            break;
        case TRIG_6_8:
            snprintf(text, 4, "%s6:8", isInversed? "!" : "");
            break;
        case TRIG_7_8:
            snprintf(text, 4, "%s7:8", isInversed? "!" : "");
            break;
        case TRIG_8_8:
            snprintf(text, 4, "%s8:8", isInversed? "!" : "");
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
            snprintf(text, 4, "%sFILL", isInversed? "!" : "");
            break;
        case TRIG_FIRST:
            snprintf(text, 4, "%s1ST", isInversed? "!" : "");
            break;
        case TRIG_TRANSITION:
            snprintf(text, 4, "%sTRANS", isInversed? "!" : "");
            break;
        case TRIG_FIRST_PAGE:
            snprintf(text, 4, "%s1ST.P", isInversed? "!" : "");
            break;
        case TRIG_TRANSITION_PAGE:
            snprintf(text, 4, "%sTRN.P", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_FIRST:
            snprintf(text, 4, "%s1ST.H", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_TRANSITION:
            snprintf(text, 4, "%sTRN.H", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_FIRST_PAGE:
            snprintf(text, 4, "%s1ST.HP", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_TRANSITION_PAGE:
            snprintf(text, 4, "%sTRN.HP", isInversed? "!" : "");
            break;
        case TRIG_LOWER_FIRST:
            snprintf(text, 4, "%s1ST.L", isInversed? "!" : "");
            break;
        case TRIG_LOWER_TRANSITION:
            snprintf(text, 4, "%sTRN.L", isInversed? "!" : "");
            break;
        case TRIG_LOWER_FIRST_PAGE:
            snprintf(text, 4, "%s1ST.LP", isInversed? "!" : "");
            break;
        case TRIG_LOWER_TRANSITION_PAGE:
            snprintf(text, 4, "%sTRN.LP", isInversed? "!" : "");
            break;
        case TRIG_FIRST_PAGE_PLAY:
            snprintf(text, 4, "%s1ST.PG", isInversed? "!" : "");
            break;
        case TRIG_FIRST_PATTERN_PLAY:
            snprintf(text, 4, "%s1ST.PT", isInversed? "!" : "");
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
int Sequencer::getTrackStepIndex(
    const uint64_t *ppqnCounter, 
    const struct Track *track, 
    std::function<void()> isFirstPulseCallback
) {
    uint64_t clampedCounter;
    if (track->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        // By track:
        clampedCounter = *ppqnCounter % (PP16N * (track->trackLength + 1));
        // Set isFirstPulse-flag
        if (isFirstPulseCallback && clampedCounter == 0) {
            isFirstPulseCallback();
        }
    } else {
        // By page:
        int pageLength = track->pageLength + 1; // 1-16
        clampedCounter = *ppqnCounter % (PP16N * pageLength);
        // We need a callback here, because selectedPage might be changed to queued page, affecting the next note to be played
        if (isFirstPulseCallback && clampedCounter == 0) {
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
void Sequencer::getNotesAtTrackStepIndex(int trackStepIndex, const struct Track *track, const struct Note **notes) {
    switch (getPolyCount(track)) {
        case 8:
            // All enabled notes for this step
            for (int i=0; i<8; i++) {
                notes[i] = &track->steps[trackStepIndex].notes[i];
            }
            break;
        case 4:
            for (int i=0; i<4; i++) {
                int targetIndex = (track->playingPageBank * 4) + (i % 4);
                notes[i] = &track->steps[trackStepIndex].notes[targetIndex];
            }
            break;
        case 2:
            for (int i=0; i<2; i++) {
                int targetIndex = (track->playingPageBank * 2) + (i % 2);
                notes[i] = &track->steps[trackStepIndex].notes[targetIndex];
            }
            break;
        case 1:
            for (int i=0; i<1; i++) {
                int targetIndex = track->playingPageBank;
                notes[0] = &track->steps[trackStepIndex].notes[targetIndex];
            }
            break;
    }
}

/**
 * Helper method to contain all the conditions that define if a note is played or not
 */
bool Sequencer::isNotePlayed(const struct Note *note, const struct Track *track, int nudgeCheck) {
    return 
        note != NULL &&
        note->enabled && 
        (note->nudge - PP16N) == nudgeCheck && 
        isNoteTrigged(note->trigg, track->repeatCount);
}

/**
 * Standalone method to process a pulse
 */
void Sequencer::processPulse(
    const uint64_t *currentPulse,
    const struct Track *track,
    std::function<void()> firstPulseCallback,
    std::function<void(const struct Note*)> playNoteCallback
) {
    // Get the index for the current step
    // int trackStepIndex = getTrackStepIndex(currentPulse, track, isFirstPulseCallback);

    // Create a lambda function that calls the member function
    auto firstPulseCallbackFn = [this]() {
        this->isFirstPulseCallback();
    };

    int trackStepIndex = getTrackStepIndex(currentPulse, track, firstPulseCallbackFn);

    // Get the nudge value that needs to be applied:
    int nudgeCheck = *currentPulse % PP16N;

    // Check for shuffle:
    if (trackStepIndex % 2 == 1) {
        nudgeCheck -= (track->shuffle - PP16N);
    }

    // Get all notes that are in this step
    int polyCount = getPolyCount(track);
    const struct Note *notes[polyCount];
    getNotesAtTrackStepIndex(trackStepIndex, track, notes);

    // Iterate over all notes in this step:
    for (int i=0; i < polyCount; i++) {
        // Get the note:
        const struct Note *note = notes[i];

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
        int nextTrackStepIndex = getTrackStepIndex(&nextStepPulse, track, std::function<void()>());

        // Check for shuffle:
        if (nextTrackStepIndex % 2 == 1) {
            nextNudgeCheck -= (track->shuffle - PP16N);
        }

        const struct Note *nextStepNotes[polyCount];
        getNotesAtTrackStepIndex(nextTrackStepIndex, track, nextStepNotes);
        for (int i=0; i < polyCount; i++) {
            // Get the note:
            const struct Note *note = nextStepNotes[i];

            if (isNotePlayed(note, track, nextNudgeCheck)) {
                playNoteCallback(note);
            }
        }
    }
}

// Private properties used in callbacks:
// static PmStream *tmpStream;
// static struct Track *tmpTrack;

/**
 * Callback when the first pulse of a page/track is played
 * Note that this is AFTER the note has already played
 */
void Sequencer::isFirstPulseCallback() {
    tmpTrack->isFirstPulse = true;
    if (tmpTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        tmpTrack->repeatCount += 1;
    } else {
        if (tmpTrack->selectedPage != tmpTrack->queuedPage && (tmpTrack->repeatCount + 1) % (tmpTrack->transitionRepeats + 1) == 0) {
            tmpTrack->playingPageBank = selectedPageBank;
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
void Sequencer::playNoteCallback(const struct Note *note) {
    // Send CC:
    if (note->cc1Value > 0) {
        sendMidiMessage(tmpStream, tmpTrack->midiChannel | 0xB0, tmpTrack->cc1Assignment, note->cc1Value - 1);
    }
    if (note->cc2Value > 0) {
        sendMidiMessage(tmpStream, tmpTrack->midiChannel | 0xB0, tmpTrack->cc2Assignment, note->cc2Value - 1);
    }

    sendMidiNoteOn(tmpStream, tmpTrack->midiChannel, note->note, note->velocity);
    addNoteToTracker(tmpStream, tmpTrack->midiChannel, note);
}

/**
 * Apply track speed to pulse
 * returns FALSE if further processing is not required
 */
bool Sequencer::applySpeedToPulse(
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
 */
void Sequencer::run(
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
    processPulse(
        &pulse, 
        selectedTrack, 
        std::bind(&Sequencer::isFirstPulseCallback, this),
        std::bind(&Sequencer::playNoteCallback, this, std::placeholders::_1)
    );
}

/**
 * Draw the page indicator
 */
void Sequencer::drawPageIndicator(const struct Track *track, int playingPage) 
{
    char bottomText[64];
    int totalPageBanks = getMaxPageBanks(track);
    snprintf(
        bottomText, 
        64,
        "PAGE %d/%d - REP %d/%d", 
        playingPage + 1, 
        totalPageBanks * 4,
        (track->repeatCount % (track->transitionRepeats + 1)) + 1,
        track->transitionRepeats + 1
    );
    drawCenteredLine(2, HEIGHT - BUTTON_HEIGHT - 12, bottomText, BUTTON_WIDTH * 4, COLOR_YELLOW);
}

/**
 * Draw the template note
 */
void Sequencer::drawTemplateNote() {
    // drawRect(SIDEBAR_OFFSET + 1, 27, SIDEBAR_WIDTH - 2, (CHAR_HEIGHT * 8) + 8, COLOR_BLACK);
    drawRect(SIDEBAR_OFFSET + 1, 27, SIDEBAR_WIDTH - 2, CHAR_HEIGHT + 2, COLOR_BLACK);
    drawText(SIDEBAR_OFFSET + 1, 28, "TEMPLATE", 20, COLOR_ORANGE);
    char text[8];
    drawSidebarTemplate(34, "NOT");
    const char *midiNote = getMidiNoteName(templateNote.note);
    drawText(SIDEBAR_OFFSET + 24, 35, midiNote, 20, COLOR_ORANGE);
    drawSidebarTemplate(40, "VEL");
    snprintf(text, sizeof(text), "%d", templateNote.velocity);
    drawText(SIDEBAR_OFFSET + 24, 41, text, 18, COLOR_ORANGE);
    drawSidebarTemplate(46, "LEN");
    snprintf(text, sizeof(text), "%d", templateNote.length);
    drawText(SIDEBAR_OFFSET + 24, 47, text, 18, COLOR_ORANGE);
    drawSidebarTemplate(52, "NDG");
    snprintf(text, sizeof(text), "%d", templateNote.nudge - PP16N);
    drawText(SIDEBAR_OFFSET + 24, 53, text, 18, COLOR_ORANGE);
    drawSidebarTemplate(58, "TRG");
    setTriggText(templateNote.trigg, text);
    drawText(SIDEBAR_OFFSET + 24, 59, text, 18, COLOR_ORANGE);
    drawSidebarTemplate(64, "CC1");
    snprintf(text, sizeof(text), "%d", templateNote.cc1Value);
    drawText(SIDEBAR_OFFSET + 24, 65, text, 18, COLOR_ORANGE);
    drawSidebarTemplate(70, "CC2");
    snprintf(text, sizeof(text), "%d", templateNote.cc2Value);
    drawText(SIDEBAR_OFFSET + 24, 71, text, 18, COLOR_ORANGE);
}

/**
 * Draw the sequencer
 */
void Sequencer::drawSequencerMain(
    uint64_t *ppqnCounter, 
    bool keyStates[SDL_NUM_SCANCODES],
    struct Track *selectedTrack,
    bool isDrumkitSequencer
) {
    if (keyStates[BLIPR_KEY_SHIFT_2] && isDrumkitSequencer) {
        // Draw drumkit instrument selector:
        drawIconOnIndex(0, BLIPR_ICON_KICK);
        // Snare
        // Clap
        // Rimshot

        drawIconOnIndex(4, BLIPR_ICON_CHAT_1);
        drawIconOnIndex(5, BLIPR_ICON_CHAT_2);
        drawIconOnIndex(6, BLIPR_ICON_OHAT);
        drawIconOnIndex(7, BLIPR_ICON_RIDE);

        drawIconOnIndex(8, BLIPR_ICON_CRASH);
        // L. Tom
        // M. Tom
        // H. Tom

        // Cowbell
        // Extra 1
        // Extra 2
        // Extra 3
    } else {
        // Draw currently playing steps:

        // Outline currently active step:
        int width = HEIGHT / 6;
        int height = width;

        // Step indicator:
        int playingPage = 0;

        // Check track speed (we do this by manupulating the pulse):
        uint64_t pulse = *ppqnCounter;
        applySpeedToPulse(selectedTrack, &pulse);
        int trackStepIndex = getTrackStepIndex(&pulse, selectedTrack, std::function<void()>());

        // Get playing page:
        if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
            playingPage = trackStepIndex / 16;
        } else {
            playingPage = selectedTrack->selectedPage;
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
        int polyCount = getPolyCount(selectedTrack);

        // Show cut & copy information:
        if (cutCounter > 0 || copyCounter > 0) {
            char bottomText[64];
            if (cutCounter == 1) {
                snprintf(bottomText, 64, "CUTTED 1 NOTE");
            } else if (cutCounter > 1) {
                snprintf(bottomText, 64, "CUTTED ALL NOTES");
            }

            if (copyCounter == 1) {
                snprintf(bottomText, 64, "PASTED 1 NOTE");
            } else if (copyCounter > 1) {
                snprintf(bottomText, 64, "PASTED ALL NOTES");
            }
            drawCenteredLine(2, HEIGHT - BUTTON_HEIGHT - 12, bottomText, BUTTON_WIDTH * 4, COLOR_YELLOW);
        } else if (!keyStates[BLIPR_KEY_SHIFT_2]) {
            drawPageIndicator(selectedTrack, playingPage);
        }

        int noteIndicatorOffset = 12 - polyCount;

        // Highlight non-empty steps:
        for (int j = 0; j < 4; j++) {
            int height = width;
            for (int i = 0; i < 4; i++) {
                int stepIndex = ((i + (j * 4)) + (selectedTrack->selectedPage * 16)) % 64;

                struct Step step = selectedTrack->steps[stepIndex];
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
                    int noteIndex = (selectedTrack->playingPageBank * polyCount) + selectedNote;

                    if (step.notes[noteIndex].enabled) {
                        struct Note *note = &step.notes[noteIndex];
                        SDL_Color noteColor = note->velocity >= 100 ? COLOR_RED : 
                            (note->velocity >= 50 ? COLOR_DARK_RED : COLOR_LIGHT_GRAY);
                        if (isNoteTrigged(note->trigg, selectedTrack->repeatCount)) {
                            drawRect(
                                4 + i + (i * width),
                                4 + j + (j * height),
                                width - 4,
                                height - 4,
                                noteColor
                            );
                            // Check if this note has a trigg condition
                            if (note->trigg > 0) {
                                drawSingleLineRectOutline(
                                    4 + i + (i * width),
                                    4 + j + (j * height),
                                    width - 4,
                                    height - 4,
                                    mixColors(COLOR_RED, COLOR_WHITE, 0.5f)
                                );    
                            }                      
                            if (!isDrumkitSequencer) {  
                                drawTextOnButton((i + (j * 4)), getMidiNoteName(step.notes[noteIndex].note));
                            } else {
                                // If this note is not equal to the template note, it means that it is a different drumkit
                                // Instrument. So we need to make that visually clear:
                                if (note->note != templateNote.note) {
                                    drawDimmedOverlay(
                                        4 + i + (i * width),
                                        4 + j + (j * height),
                                        width - 4,
                                        height - 4
                                    );
                                }
                            }
                        } else {
                            // Here is a note, but it is not trigged by the fill condition:
                            drawSingleLineRectOutline(
                                4 + i + (i * width),
                                4 + j + (j * height),
                                width - 4,
                                height - 4,
                                noteColor
                            );
                        }

                        // Draw nudge box:
                        if (note->nudge < PP16N) {
                            drawRect(
                                2 + i + (i * width),
                                2 + j + (j * height) + (BUTTON_HEIGHT * 0.4),
                                2,
                                5,
                                noteColor
                            );
                        } else if (note->nudge > PP16N) {
                            drawRect(
                                i + (i * width) + BUTTON_WIDTH,
                                2 + j + (j * height) + (BUTTON_HEIGHT * 0.4),
                                2,
                                5,
                                noteColor
                            );
                        }
                    }
                }

                // Draw selection outline:
                if (selectedSteps[i + (j * 4)]) {
                    drawSingleLineRectOutline(
                        4 + i + (i * width),
                        4 + j + (j * height),
                        width - 4,
                        height - 4,
                        COLOR_YELLOW
                    );
                }
                
                // Poly count dots:
                if (!isDrumkitSequencer) {
                    drawRect(
                        5 + i + (i * width) + noteIndicatorOffset,
                        5 + j + (j * height),
                        (polyCount * 2) + 1,
                        3,
                        COLOR_GRAY
                    );

                    int baseNoteIndex = (selectedTrack->playingPageBank * polyCount);
                    for (int p=0; p<polyCount; p++) {
                        drawPixel(
                            6 + i + (i * width) + (p * 2) + noteIndicatorOffset,
                            6 + j + (j * height),
                            p == selectedNote ? COLOR_WHITE : (step.notes[baseNoteIndex + p].enabled ? COLOR_RED : COLOR_LIGHT_GRAY)
                        );
                    }
                }
            }
        }   
    }

    // Draw template Note details:
    drawTemplateNote();

    // ABCD Buttons:
    if (keyStates[BLIPR_KEY_SHIFT_1]) {
        // Show utilities:
        char descriptions[4][4] = {"OPT", "CUT", "CPY", "PST"};        
        drawABCDButtons(descriptions);
    } else if (keyStates[BLIPR_KEY_SHIFT_2]) {
        // Note (for polyphony)
        char descriptions[4][4] = {"-", "-", "<", ">"};
        int polyCount = getPolyCount(selectedTrack);
        if (polyCount < 8) {
            snprintf(descriptions[0], 4, "<");
            snprintf(descriptions[1], 4, ">");
        }
        if (isDrumkitSequencer) {
            snprintf(descriptions[2], 4, "-");
            snprintf(descriptions[3], 4, "-");
        }
        drawABCDButtons(descriptions);
        // Draw page bank title:
        drawCenteredLine(
            2, 
            HEIGHT - BUTTON_HEIGHT - 12, 
            "PAGEBANK",
            BUTTON_WIDTH * 2,
            COLOR_YELLOW
        );
        // Draw page bank number:
        char pageBankText[2];
        snprintf(pageBankText, 2, "%d", selectedPageBank + 1);
        drawText(2 + 28, HEIGHT - BUTTON_HEIGHT + 2, pageBankText, BUTTON_WIDTH, COLOR_WHITE);
        // Draw channel title:
        drawCenteredLine(
            6 + (BUTTON_WIDTH * 2), 
            HEIGHT - BUTTON_HEIGHT - 12, 
            "CHANNEL",
            BUTTON_WIDTH * 2,
            COLOR_YELLOW
        );
        // Draw channel number:
        char channelText[2];
        snprintf(channelText, 2, "%d", selectedNote + 1);
        drawText(92, HEIGHT - BUTTON_HEIGHT + 2, channelText, BUTTON_WIDTH, COLOR_WHITE);
    } else {
        // Page numbers:
        char descriptions[4][4] = {"P00", "P00", "P00", "P00"};
        int startPage = (selectedPageBank * 4) + 1;
        
        // Fill the descriptions array
        for (int i = 0; i < 4; i++) {
            int pageNumber = startPage + i;
            // Format with leading zero if needed
            if (pageNumber < 10) {
                snprintf(descriptions[i], 4, "P0%d", pageNumber);
            } else {
                snprintf(descriptions[i], 4, "P%d", pageNumber);
            }
        }

        drawABCDButtons(descriptions);
    
        // Draw highlighted page:
        if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
            if (selectedTrack->selectedPage >= selectedTrack->playingPageBank * 4 && 
                selectedTrack->selectedPage < (selectedTrack->playingPageBank + 1) * 4) {
                // Outline current page:
                drawHighlightedGridTile((selectedTrack->selectedPage % 4) + 16);
            }
        } else {
            // Highlight playing page + queued page:
            if (selectedTrack->queuedPage != selectedTrack->selectedPage) {
                if (selectedTrack->queuedPage >= selectedPageBank * 4 && 
                    selectedTrack->queuedPage < (selectedPageBank + 1) * 4) {
                    // Outline queued page:
                    drawHighlightedGridTileInColor((selectedTrack->queuedPage % 4) + 16, COLOR_RED);
                }
            }

            if (selectedTrack->selectedPage >= selectedPageBank * 4 && 
                selectedTrack->selectedPage < (selectedPageBank + 1) * 4) {
                // Outline queued page:
                drawHighlightedGridTile((selectedTrack->selectedPage % 4) + 16);
            }
        }            
    }
}

/**
 * Draw the step editor
 */
void Sequencer::drawStepEditor(struct Track *track, bool isDrumkitSequencer) {
    /*
        - 1-4   : Transpose -12 / -1 / +1 / +12
        - 5-6   : Increase / decrease velocity
        - 7-8   : Increase / decrease length
        - 9-10  : Increase / decrease nudge
        - 11-12 : Increase / decrease trig condition
        - 13-14 : Increase / decrease CC1 value
        - 15-16 : Increase / decrease CC2 value    
    */

    // Iterate over selected Steps, and replace the values with "##" if there are differences in selected steps 
    // for these values. The values will change each on their own.
    // We can use areAllStepPropertiesTheSame for this

    // Title:
    drawCenteredLine(2, 133, "STEP OPTIONS", TITLE_WIDTH, COLOR_WHITE);

    // Step is the first selected step:
    struct Note *note;
    for (int i=0; i<16; i++) {
        if (selectedSteps[i]) {
            int stepIndex = i + (track->selectedPage * 16);
            note = &track->steps[stepIndex].notes[selectedNote];
            break;
        }
    }     

    // Note/Transpose:
    if (!isDrumkitSequencer) {
        drawCenteredLine(2, 7, "TRANSPOSE", TITLE_WIDTH, COLOR_WHITE);
        const char *midiNoteName;
        if (areAllStepPropertiesTheSame[PROPERTY_NOTE]) {
            midiNoteName = getMidiNoteName(note->note);
        } else {
            midiNoteName = "##";
        }
        drawCenteredLine(2, 22, midiNoteName, TITLE_WIDTH, COLOR_YELLOW);
        drawTextOnButton(0, "-12");
        drawTextOnButton(1, "-1");
        drawTextOnButton(2, "+1");
        drawTextOnButton(3, "+12");
    }

    // Velocity:
    drawCenteredLine(2, 37, "VELOCITY", BUTTON_WIDTH * 2, COLOR_WHITE);
    char velocityChar[4];
    if (areAllStepPropertiesTheSame[PROPERTY_VELOCITY]) {
        snprintf(velocityChar, sizeof(velocityChar), "%d", note->velocity);
    } else {
        snprintf(velocityChar, sizeof(velocityChar), "##");
    }
    drawCenteredLine(2, 47, velocityChar, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(4, "-");
    drawTextOnButton(5, "+");

    // Length:
    drawCenteredLine(62, 37, "LENGTH", BUTTON_WIDTH * 2, COLOR_WHITE);
    char lengthChar[4];
    if (areAllStepPropertiesTheSame[PROPERTY_LENGTH]) {
        snprintf(lengthChar, sizeof(lengthChar), "%d", note->length);
    } else {
        snprintf(lengthChar, sizeof(lengthChar), "##");
    }
    drawCenteredLine(62, 47, lengthChar, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(6, "-");
    drawTextOnButton(7, "+");

    // Nudge:
    drawCenteredLine(2, 67, "NUDGE", BUTTON_WIDTH * 2, COLOR_WHITE);
    char nudgeChar[4];
    if (areAllStepPropertiesTheSame[PROPERTY_NUDGE]) {
        snprintf(nudgeChar, sizeof(nudgeChar), "%d", note->nudge - PP16N);
    } else {
        snprintf(nudgeChar, sizeof(nudgeChar), "##");
    }
    drawCenteredLine(2, 77, nudgeChar, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(8, "-");
    drawTextOnButton(9, "+");

    // Trig:
    drawCenteredLine(62, 67, "TRIGG", BUTTON_WIDTH * 2, COLOR_WHITE);
    char triggChar[4];
    if (areAllStepPropertiesTheSame[PROPERTY_TRIG]) {
        snprintf(
            triggChar, 
            sizeof(triggChar), 
            "%d", 
            get2FByteValue(note->trigg) + (get2FByteFlag2(note->trigg) ? TRIG_HIGHEST_VALUE : 0)
        );
    } else {
        snprintf(triggChar, sizeof(triggChar), "##");
    }
    drawCenteredLine(62, 77, triggChar, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(10, "-");
    drawTextOnButton(11, "+");
    // Trigg description:
    char triggText[6];
    if (areAllStepPropertiesTheSame[PROPERTY_TRIG]) {
        setTriggText(note->trigg, triggText);
    } else {
        snprintf(triggText, 6, "##");
    }
    drawCenteredLine(62, 87, triggText, BUTTON_WIDTH * 2, COLOR_WHITE);

    // CC1:
    drawCenteredLine(2, 97, "CC1 VALUE", BUTTON_WIDTH * 2, COLOR_WHITE);
    char cc1Value[4];
    if (areAllStepPropertiesTheSame[PROPERTY_CC1]) {
        if (note->cc1Value == 0) {
            snprintf(cc1Value, sizeof(cc1Value), "OFF");
        } else {
            snprintf(cc1Value, sizeof(cc1Value), "%d", note->cc1Value - 1);
        }
    } else {
        snprintf(cc1Value,  sizeof(cc1Value), "##");
    }
    drawCenteredLine(2, 107, cc1Value, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(12, "-");
    drawTextOnButton(13, "+");

    // CC2:
    drawCenteredLine(62, 97, "CC2 VALUE", BUTTON_WIDTH * 2, COLOR_WHITE);
    char cc2Value[4];
    if (areAllStepPropertiesTheSame[PROPERTY_CC2]) {
        if (note->cc2Value == 0) {
            snprintf(cc2Value, sizeof(cc2Value), "OFF");
        } else {
            snprintf(cc2Value, sizeof(cc2Value), "%d", note->cc2Value);
        }        
    } else {
        snprintf(cc2Value, sizeof(cc2Value), "##");
    }
    drawCenteredLine(62, 107, cc2Value, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(14, "-");
    drawTextOnButton(15, "+");

    // Step selected, show options:
    // A = apply to all notes on this step (ALL), or only this note (ONE)
    // B = note value options (default)
    // C = ... time options? like increase or decrease of values?
    // D = ... fx? side chaining?

    char descriptions[4][4] = {"ONE", "OPT", "-", "-"};
    snprintf(descriptions[0], 4, isEditOnAllNotes ? "ALL" : "ONE");
    drawABCDButtons(descriptions);
}

void Sequencer::draw(
    uint64_t *ppqnCounter, 
    bool keyStates[SDL_NUM_SCANCODES],
    struct Track *selectedTrack,
    bool isDrumkitSequencer
) {
    if (!isNoteEditorVisible) {
        drawSequencerMain(ppqnCounter, keyStates, selectedTrack, isDrumkitSequencer);
    } else {
        // int stepIndex = selectedStep + (selectedTrack->selectedPage * 16);
        drawStepEditor(selectedTrack, isDrumkitSequencer);
    }
}

void Sequencer::draw(
    uint64_t *ppqnCounter, 
    bool keyStates[SDL_NUM_SCANCODES],
    struct Track *selectedTrack
) {
    draw(ppqnCounter, keyStates, selectedTrack, false);
}
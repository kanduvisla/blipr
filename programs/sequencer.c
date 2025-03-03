#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include <string.h>
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
    if (key == BLIPR_KEY_9) { note->nudge = MAX(0, note->nudge - 1); } else 
    if (key == BLIPR_KEY_10) { note->nudge = MIN(127, note->nudge + 1); } else 
    if (key == BLIPR_KEY_11) { note->trigg = MAX(0, note->trigg - 1); } else 
    if (key == BLIPR_KEY_12) { note->trigg = MIN(127, note->trigg + 1); } else 
    if (key == BLIPR_KEY_13) { note->cc1Value = MAX(0, note->cc1Value - 1); } else 
    if (key == BLIPR_KEY_14) { note->cc1Value = MIN(127, note->cc1Value + 1); } else 
    if (key == BLIPR_KEY_15) { note->cc2Value = MAX(0, note->cc2Value - 1); } else 
    if (key == BLIPR_KEY_16) { note->cc2Value = MIN(127, note->cc2Value + 1); }
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
            if (key == BLIPR_KEY_A) { 
                selectedNote = MAX(0, selectedNote - 1); 
            } else if (key == BLIPR_KEY_B) { 
                selectedNote = MIN(getPolyCount(selectedTrack) - 1, selectedNote + 1); 
            } else if (key == BLIPR_KEY_C) {
                // TODO: Copy (once=copy note, twice=copy step)
            } else if (key == BLIPR_KEY_D) {
                // TODO: Paste
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
 * Returns the index of the step in the track.steps array
 */
int calculateTrackStepIndex(int ppqnStep, int pageSize, int totalTrackLength, int pagePlayMode) {
    // Note: this might change sometimes if I decide to include page length into the continuous play mode, 
    //       but that seems not required for now (makes no sense anyway, why would you want that?)

    // Ensure pageSize is between 1 and 16
    pageSize = (pageSize < 1) ? 1 : (pageSize > 16) ? 16 : pageSize;

    if (pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        // Continuous Mode, this ignores page length
        return ppqnStep % totalTrackLength;
    } else {
        // Page Repeat Mode, this ignores track length
        int pageNumber = ppqnStep / pageSize;
        return (16 * pageNumber) + (ppqnStep % pageSize);
    }
}

/**
 * Run the sequencer
 */
void runSequencer(
    PmStream *outputStream,
    int *ppqnCounter, 
    struct Track *selectedTrack
) {
    // TODO: we need to run this every step, and do the modulo with the nudge property :-)
    // We always divide by 4, because there are four 16th notes in one quarter note:
    // TODO: Change this when resolution is increased:
    if (*ppqnCounter % (PPQN_MULTIPLIED / 4) == 0) {
        int pp16 = *ppqnCounter / 4;        // pulses per 16th note (6 pulses idealy)
        int stepCounter = pp16 / 6;

        // TODO: Now it's individual track pages by default, needs sync implementation:
        if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_REPEAT) {
            int pageLength = selectedTrack->pageLength + 1;
            stepCounter %= pageLength;
            // Check for queued page:
            if (stepCounter % pageLength == 0 && selectedTrack->selectedPage != selectedTrack->queuedPage) {
                selectedTrack->selectedPage = selectedTrack->queuedPage;
            }
            stepCounter += (selectedTrack->selectedPage * pageLength);
        }

        int trackStepIndex = calculateTrackStepIndex(
            stepCounter, 
            selectedTrack->pageLength + 1,
            selectedTrack->trackLength + 1,
            selectedTrack->pagePlayMode
        );

        struct Step *step = &selectedTrack->steps[trackStepIndex];
        // Depending on polyphony, iterate over notes:

        struct Note *note = &step->notes[selectedNote];
        if (note->enabled) {
            sendMidiNoteOn(outputStream, selectedTrack->midiChannel, note->note, note->velocity);
            addNoteToTracker(outputStream, selectedTrack->midiChannel, note);
        }
    }

    // Decrease note-off counters:
    updateNotesAndSendOffs(outputStream, selectedTrack->midiChannel);
}

/**
 * Draw the sequencer
 */
void drawSequencerMain(
    int *ppqnCounter, 
    struct Track *selectedTrack
) {
    // Outline currently active step:
    int width = HEIGHT / 6;
    int height = width;

    // Step indicator:
    int pp16 = *ppqnCounter / 4;                            // pulses per 16th note (6 pulses idealy)
    int stepCounter = pp16 / 6;
    int playingPage = 0;

    int trackStepIndex = calculateTrackStepIndex(
        stepCounter, 
        selectedTrack->pageLength + 1,
        selectedTrack->trackLength + 1,
        selectedTrack->pagePlayMode
    );

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
                // Nope
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
    char descriptions[4][4] = {"A.1", "B.1", "C.1", "D.1"};
    drawABCDButtons(descriptions);
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
    snprintf(nudgeChar, sizeof(nudgeChar), "%d", note->nudge);
    drawCenteredLine(2, 77, nudgeChar, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(8, "-");
    drawTextOnButton(9, "+");

    // Trig:
    drawCenteredLine(62, 67, "TRIGG", BUTTON_WIDTH * 2, COLOR_WHITE);
    char triggChar[4];
    snprintf(triggChar, sizeof(triggChar), "%d", note->trigg);
    drawCenteredLine(62, 77, triggChar, BUTTON_WIDTH * 2, COLOR_YELLOW);
    drawTextOnButton(10, "-");
    drawTextOnButton(11, "+");

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
}

void drawSequencer(
    int *ppqnCounter, 
    struct Track *selectedTrack
) {
    if (selectedStep == -1) {
        drawSequencerMain(ppqnCounter, selectedTrack);
    } else {
        drawStepEditor(&selectedTrack->steps[selectedStep]);
    }
}


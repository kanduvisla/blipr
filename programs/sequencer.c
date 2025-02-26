#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include "../project.h"
#include "../constants.h"
#include "../colors.h"
#include "../drawing.h"
#include "../drawing_components.h"
#include "../utils.h"
#include "../midi.h"

// TODO: These should be the global things I guess?
int selectedPage = 0;
int queuedPage = 0;
int selectedNote = 0;
int defaultNote = 80;
int defaultVelocity = 100;
int halfVelocity = 50;
int totalPpqnCounter = 0;

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
        selectedPage = index;
    } else {
        queuedPage = index;
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
    int index = scancodeToStep(key) + (selectedPage * 16);
    if (keyStates[BLIPR_KEY_SHIFT_1]) {
        if (index >= 0) {
            // Toggle velocity
            toggleVelocity(&selectedTrack->steps[index]);
        }
    } else if(keyStates[BLIPR_KEY_SHIFT_2]) {
        // Update selected note or page:
    } else if(key == BLIPR_KEY_A) {
        setSelectedPage(selectedTrack, 0);
    } else if(key == BLIPR_KEY_B) {
        setSelectedPage(selectedTrack, 1);
    } else if(key == BLIPR_KEY_C) {
        setSelectedPage(selectedTrack, 2);
    } else if(key == BLIPR_KEY_D) {
        setSelectedPage(selectedTrack, 3);
    } else {
        if (index >= 0) {
            // Toggle key
            toggleStep(&selectedTrack->steps[index]);
        }
    }
}

/**
 * Returns the index of the step in the track.steps array
 */
int calculateTrackStepIndex(int ppqnStep, int pageSize, int totalTrackLength, int pagePlayMode) {
    // Each track has a page size, this can be 1 to 16.
    // Pages are divided in the track.steps data in slots of 16.
    // So for example: if page size is 12, slot 1 will be [0-11], slot 2 will be [16-27], slot 3 will be [32-43], etc.
    // Meanwhile, the step calculated from the PPQN passed will be ever increased. So the challenge here will be to map this step to the index in the track.steps array
    // In the example above, if ppqnStep is 11, the trackStepIndex will be 11, but if ppqnStep is 12, the next slot is triggered and the trackStepIndex will be 16.
    // This can also be challenging for smaller page sizes. For example, with a page size of 4:
    // if ppqnStep is 3, trackStepindex will be 3.
    // if ppqnStep is 4, trackStepIndex will be 16. 
    // if ppqnStep is 7, trackStepIndex will be 19.
    // if ppqnStep is 8, trackStepIndex will be 32.
    // Also - to make things more difficult - the complete track can have a maximum length as well, and the trackStepIndex can never exceed that. If that happens, it needs to go back to the first slot.
    // For example, with a page size of 12, and a total track length of 44:
    // if ppqnStep is 11, trackStepIndex will be 11.
    // if ppqnStep is 12, trackStepIndex will be 16.
    // if ppqnStep is 23, trackStepIndex will be 27.
    // if ppqnStep is 24, trackStepIndex will be 32.
    // if ppqnStep is 36, trackStepIndex would be 44, but this will exceed the total track length (since trackStepIndex is zero-indexed), so trackStepIndex will be 0
    // if ppqnStep is 37, trackStepindex will be 1.
    // etc...

    /*
    page size 12, track length 44
    0=0
    12=16
    24=32
    36=48
    PPQN Step: 34, Track Step Index: 42
    PPQN Step: 35, Track Step Index: 43
    PPQN Step: 36, Track Step Index: 4      -- WHY?
    PPQN Step: 37, Track Step Index: 5
    */

    // [................] [................] [................] [................] = 64 steps
    // [xxxxxxxxxxxx....] [xxxxxxxxxxxx....] [xxxxxxxxxxxx....] [xxxxxxxxxxxx....] = page length 12
    // [xxxxxxxxxxxx....] [xxxxxxxxxxxx....] [xxxxxxxxxxxx|                        = page length 12 / track length 44

    // with continuous play, it's just ppqnStep % totalTrackLength
    // with page repeat page, it's (16 * pageNumber) + (ppqnStep % pageLength)

    // Ensure pageSize is between 1 and 16
    pageSize = (pageSize < 1) ? 1 : (pageSize > 16) ? 16 : pageSize;

    if (pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        // Continuous Mode, this ignores page length
        return ppqnStep & totalTrackLength;
    } else {
        // Page Repeat Mode, this ignores track length
        int pageNumber = ppqnStep / pageSize;
        return (16 * pageNumber) + (ppqnStep % pageSize);
    }

    // Calculate the number of steps in a complete cycle
    // int cycleLength = pageSize * ((totalTrackLength + 15) / 16);
    // cycleLength = totalTrackLength;

    // Wrap ppqnStep to the cycle length
    ppqnStep %= totalTrackLength;

    // Calculate the page number and step within the page
    int pageNumber = ppqnStep / pageSize;
    int stepInPage = ppqnStep % pageSize;

    // Calculate the base index for the current page
    int baseIndex = (pageNumber * 16) % totalTrackLength;
    // printf("ppqn step: %d\n", ppqnStep);
    // printf("page number: %d\n", pageNumber);
    // printf("step in page: %d\n", stepInPage);
    // printf("base index: %d\n", baseIndex);

    // Calculate the final index
    int trackStepIndex = (baseIndex + stepInPage) % totalTrackLength;

    return trackStepIndex;
}

/**
 * Run the sequencer
 */
void runSequencer(
    PmStream *outputStream,
    int *ppqnCounter, 
    struct Track *selectedTrack
) {
    // We always divide by 4, because there are four 16th notes in one quarter note:
    // TODO: Change this when resolution is increased:
    if (*ppqnCounter % (PPQN_MULTIPLIED / 4) == 0) {
        int pp16 = *ppqnCounter / 4;        // pulses per 16th note (6 pulses idealy)
        int stepIndex = 0;
        int stepCounter = pp16 / 6;
        int pageLength = selectedTrack->pageLength + 1;
        // TODO: To work with page length, we have to do something with stepCounter & pageLength
        // The challenge here is that if page length is for example 12, then step 13 for the counter will
        // step 17 in the sequencer. So the stepIndex needs to add n when (stepCounter % 16) > pageLength.
        // What needs to be added in this case is ((stepCounter / 16) + 1) * (16 % pageLength) or something like that
        // Need to come up with a formula...

        if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
            stepIndex = stepCounter % (selectedTrack->trackLength + 1);
        } else {
            // Check for queued page:
            if (stepCounter % pageLength == 0 && selectedPage != queuedPage) {
                selectedPage = queuedPage;
            }
            // Repeat current page:
            int remainingLength = (selectedTrack->trackLength + 1) - (selectedPage * pageLength);
            if (remainingLength > pageLength) {
                stepIndex = (selectedPage * pageLength) + (stepCounter % pageLength);
            } else {
                stepIndex = (selectedPage * pageLength) + (stepCounter % remainingLength);
            }
        }
        
        struct Step *step = &selectedTrack->steps[stepIndex];
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
void drawSequencer(
    int *ppqnCounter, 
    struct Track *selectedTrack
) {
    // Outline currently active step:
    int width = HEIGHT / 6;
    int height = width;

    // Step indicator:
    int pp16 = *ppqnCounter / 4;                            // pulses per 16th note (6 pulses idealy)
    int stepCounter = pp16 / 6;
    int stepIndex = 0;
    int playingPage = 0;
    int pageLength = selectedTrack->pageLength + 1;
    // printf("page length: %d\n", pageLength);

    if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) {
        stepIndex = stepCounter % (selectedTrack->trackLength + 1);
        playingPage = stepIndex / 16;
        // Outline current page:
        drawHighlightedGridTile(selectedPage + 16);
    } else {
        int remainingLength = (selectedTrack->trackLength + 1) - (selectedPage * pageLength);
        if (remainingLength > pageLength) {
            stepIndex = (selectedPage * pageLength) + (stepCounter % pageLength);
        } else {
            stepIndex = (selectedPage * pageLength) + (stepCounter % remainingLength);
        }
        playingPage = selectedPage;
        drawHighlightedGridTile(queuedPage + 16);
    }

    // Draw outline on currently playing note:
    if (
        playingPage >= selectedPage && playingPage < selectedPage + 1
    ) {
            int x = stepIndex % 4;
            int y = (stepIndex / 4) % 4;
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
            struct Step step = selectedTrack->steps[(i + (j * 4)) + (selectedPage * 16)];
            // Check if this is within the track length, or outside the page length:
            if (
                (selectedPage * 16) + i + (j * 4) > selectedTrack->trackLength ||
                (i + (j * 4)) > selectedTrack->pageLength
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
                }
            }
        }
    }   
}

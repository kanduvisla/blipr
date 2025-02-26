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
int selectedPage = 0;   // The page currently playing
int queuedPage = 0;     // The page in queue to play after this page
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
    // We always divide by 4, because there are four 16th notes in one quarter note:
    // TODO: Change this when resolution is increased:
    if (*ppqnCounter % (PPQN_MULTIPLIED / 4) == 0) {
        int pp16 = *ppqnCounter / 4;        // pulses per 16th note (6 pulses idealy)
        int stepCounter = pp16 / 6;

        if (selectedTrack->pagePlayMode == PAGE_PLAY_MODE_REPEAT) {
            int pageLength = selectedTrack->pageLength + 1;
            stepCounter %= pageLength;
            // Check for queued page:
            if (stepCounter % pageLength == 0 && selectedPage != queuedPage) {
                selectedPage = queuedPage;
            }
            stepCounter += (selectedPage * pageLength);
        }

        int trackStepIndex = calculateTrackStepIndex(
            stepCounter, 
            selectedTrack->pageLength + 1,
            selectedTrack->trackLength + 1,
            selectedTrack->pagePlayMode
        );

        struct Step *step = &selectedTrack->steps[trackStepIndex];
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
        drawHighlightedGridTile(selectedPage + 16);
    } else {
        playingPage = selectedPage;
        // Outline queued page:
        drawHighlightedGridTile(queuedPage + 16);
    }

    // Draw outline on currently playing note:
    if (
        playingPage >= selectedPage && playingPage < selectedPage + 1
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
            struct Step step = selectedTrack->steps[(i + (j * 4)) + (selectedPage * 16)];
            // Check if this is within the track length, or outside the page length:
            if (
                ((selectedTrack->pagePlayMode == PAGE_PLAY_MODE_CONTINUOUS) && (selectedPage * 16) + i + (j * 4) > selectedTrack->trackLength) ||
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
                }
            }
        }
    }   
}

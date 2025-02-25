#include <SDL.h>
#include <stdbool.h>
#include "../project.h"
#include "../constants.h"
#include "../colors.h"
#include "../drawing.h"
#include "../drawing_components.h"
#include "../utils.h"

// TODO: These should be the global things I guess?
int selectedPage = 0;
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
void setSelectedPage(int index) {
    printf("Set selected page to %d\n", index);
    selectedPage = index;
}

/**
 * Update the sequencer according to user input
 */
void updateSequencer(
    struct Project *project, 
    bool keyStates[SDL_NUM_SCANCODES], 
    SDL_Scancode key, 
    int selectedSequence, 
    int selectedPattern, 
    int selectedTrack
) {
    int index = scancodeToStep(key);
    if (keyStates[BLIPR_KEY_SHIFT_1]) {
        if (index >= 0) {
            // Toggle velocity
            toggleVelocity(&project->sequences[selectedSequence].patterns[selectedPattern].tracks[selectedTrack].steps[index]);
        }
    } else if(keyStates[BLIPR_KEY_SHIFT_2]) {
        // Update selected note or page:
    } else if(key == BLIPR_KEY_A) {
        setSelectedPage(0);
    } else if(key == BLIPR_KEY_B) {
        setSelectedPage(1);
    } else if(key == BLIPR_KEY_C) {
        setSelectedPage(2);
    } else if(key == BLIPR_KEY_D) {
        setSelectedPage(3);
    } else {
        if (index >= 0) {
            // Toggle key
            toggleStep(&project->sequences[selectedSequence].patterns[selectedPattern].tracks[selectedTrack].steps[index]);
        }
    }
}

/**
 * Run the sequencer
 */
void runSequencer(
    struct Project *project, 
    int *ppqnCounter, 
    int selectedSequence, 
    int selectedPattern,
    struct Track *selectedTrack
) {
    /*
    totalPpqnCounter++;
    if (totalPpqnCounter >= MAX_PULSES) {
        totalPpqnCounter = 0;
    }

    int totalSteps = totalPpqnCounter / PPQN;
    short int trackLength = project->sequences[selectedSequence].patterns[selectedPattern].tracks[0].trackLength;
    */
}

/**
 * Draw the sequencer
 */
void drawSequencer(
    int *ppqnCounter, 
    struct Track *track
) {
    // Outline currently active step:
    int width = HEIGHT / 6;
    int height = width;
    int x = (*ppqnCounter / (PPQN_MULTIPLIED / 4)) % 4;
    int y = ((*ppqnCounter / (PPQN_MULTIPLIED / 4)) / 4) % 4;

    drawSingleLineRectOutline(
        2 + x + (x * width),
        2 + y + (y * height),
        width,
        height,
        COLOR_WHITE
    );

    // Outline current page:
    drawHighlightedGridTile(selectedPage + 16);

    // Highlight non-empty steps:
    for (int j = 0; j < 4; j++) {
        int height = width;
        for (int i = 0; i < 4; i++) {
            struct Step step = track->steps[i + (j * 4)];
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

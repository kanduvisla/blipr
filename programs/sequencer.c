#include <SDL.h>
#include <stdbool.h>
#include "../project.h"
#include "../constants.h"
#include "../colors.h"
#include "../drawing.h"

// TODO: These should be the global things I guess?
int selectedTrack = 0;
int selectedPattern = 0;
int selectedSequence = 0;
int defaultNote = 80;
int defaultVelocity = 100;
int halfVelocity = 50;

/**
 * Toggle a step
 */
void toggleStep(struct Step *step) {
    if (step->note == 0) {
        step->note = defaultNote;
        step->velocity = defaultVelocity;
    } else {
        step->note = 0;
    }
}

/**
 * Toggle velocity
 */
void toggleVelocity(struct Step *step) {
    if (step->note != 0) {
        if (step->velocity == defaultVelocity) {
            step->velocity = halfVelocity;
        } else {
            step->velocity = defaultVelocity;
        }
    } else {
        // If no note is set, create a new one on half velocity:
        toggleStep(step);
        step->velocity = halfVelocity;
    }
}

/**
 * Update the sequencer according to user input
 */
void updateSequencer(struct Project *project, bool keyStates[SDL_NUM_SCANCODES], SDL_Scancode key) {
    if (keyStates[BLIPR_KEY_SHIFT_1]) {
        // Toggle velocity
        switch(key) {
            case BLIPR_KEY_1:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[0]);
                break;
            case BLIPR_KEY_2:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[1]);
                break;
            case BLIPR_KEY_3:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[2]);
                break;
            case BLIPR_KEY_4:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[3]);
                break;
            case BLIPR_KEY_5:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[4]);
                break;
            case BLIPR_KEY_6:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[5]);
                break;
            case BLIPR_KEY_7:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[6]);
                break;
            case BLIPR_KEY_8:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[7]);
                break;
            case BLIPR_KEY_9:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[8]);
                break;
            case BLIPR_KEY_10:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[9]);
                break;
            case BLIPR_KEY_11:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[10]);
                break;
            case BLIPR_KEY_12:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[11]);
                break;
            case BLIPR_KEY_13:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[12]);
                break;
            case BLIPR_KEY_14:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[13]);
                break;
            case BLIPR_KEY_15:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[14]);
                break;
            case BLIPR_KEY_16:
                toggleVelocity(&project->sequences[0].patterns[0].tracks[0].steps[15]);
                break;
        }
    } else {
        // Toggle key
        switch(key) {
            case BLIPR_KEY_1:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[0]);
                break;
            case BLIPR_KEY_2:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[1]);
                break;
            case BLIPR_KEY_3:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[2]);
                break;
            case BLIPR_KEY_4:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[3]);
                break;
            case BLIPR_KEY_5:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[4]);
                break;
            case BLIPR_KEY_6:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[5]);
                break;
            case BLIPR_KEY_7:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[6]);
                break;
            case BLIPR_KEY_8:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[7]);
                break;
            case BLIPR_KEY_9:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[8]);
                break;
            case BLIPR_KEY_10:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[9]);
                break;
            case BLIPR_KEY_11:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[10]);
                break;
            case BLIPR_KEY_12:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[11]);
                break;
            case BLIPR_KEY_13:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[12]);
                break;
            case BLIPR_KEY_14:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[13]);
                break;
            case BLIPR_KEY_15:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[14]);
                break;
            case BLIPR_KEY_16:
                toggleStep(&project->sequences[0].patterns[0].tracks[0].steps[15]);
                break;
        }
    }
}

/**
 * Run the sequencer
 */
void runSequencer(struct Project *project, int *ppqnCounter) {

}

/**
 * Draw the sequencer
 */
void drawSequencer(struct Project *project, int *noteCounter) {
    // Outline currently active step:
    int width = HEIGHT / 6;
    int height = width;
    int x = *noteCounter % 4;
    int y = *noteCounter / 4;

    drawSingleLineRectOutline(
        2 + x + (x * width),
        2 + y + (y * height),
        width,
        height,
        COLOR_WHITE
    );

    // Highlight non-empty steps:
    for (int j = 0; j < 4; j++) {
        int height = width;
        for (int i = 0; i < 4; i++) {
            struct Step step = project->sequences[0].patterns[0].tracks[0].steps[i + (j * 4)];
            if (step.note != 0) {
                drawRect(
                    4 + i + (i * width),
                    4 + j + (j * height),
                    width - 4,
                    height - 4,
                    step.velocity >= defaultVelocity ? COLOR_RED : COLOR_DARK_RED
                );
            }
        }
    }   
}

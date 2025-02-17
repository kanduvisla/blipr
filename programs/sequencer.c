#include <SDL.h>
#include <stdbool.h>
#include "../project.h"
#include "../constants.h"
#include "../colors.h"
#include "../drawing.h"
#include "../utils.h"

// TODO: These should be the global things I guess?
int selectedPage = 0;
int selectedNote = 0;
int defaultNote = 80;
int defaultVelocity = 100;
int halfVelocity = 50;

/**
 * Toggle a step
 */
void toggleStep(struct Step *step) {
    step->notes[selectedNote].enabled = !step->notes[selectedNote].enabled;
    // Set default values:
    if (step->notes[selectedNote].enabled) {
        step->notes[selectedNote].velocity = defaultVelocity;
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
 * Update the sequencer according to user input
 */
void updateSequencer(struct Project *project, bool keyStates[SDL_NUM_SCANCODES], SDL_Scancode key, int selectedSequence, int selectedPattern, int selectedTrack) {
    int index = scancodeToStep(key);
    if (index == -1) {
        return;
    }

    if (keyStates[BLIPR_KEY_SHIFT_1]) {
        // Toggle velocity
        toggleVelocity(&project->sequences[selectedSequence].patterns[selectedPattern].tracks[selectedTrack].steps[index]);
    } else if(keyStates[BLIPR_KEY_SHIFT_2]) {
        // Update selected note or page:

    } else {
        // Toggle key
        toggleStep(&project->sequences[selectedSequence].patterns[selectedPattern].tracks[selectedTrack].steps[index]);
    }
}

/**
 * Run the sequencer
 */
void runSequencer(struct Project *project, int *ppqnCounter, int selectedSequence, int selectedPattern) {

}

/**
 * Draw the sequencer
 */
void drawSequencer(struct Project *project, int *noteCounter, int selectedSequence, int selectedPattern, int selectedTrack) {
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
            struct Step step = project->sequences[selectedSequence].patterns[selectedPattern].tracks[selectedTrack].steps[i + (j * 4)];
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

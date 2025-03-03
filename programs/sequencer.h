#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include "../project.h"

// Speed conditions:
#define SPEED_1_1 0
#define SPEED_1_2 1
#define SPEED_1_4 2
#define SPEED_1_8 3
#define SPEED_1_16 4
#define SPEED_2_1 5
#define SPEED_4_1 6
#define SPEED_8_1 7
#define SPEED_16_1 8
#define SPEED_1_3 9
#define SPEED_1_6 10
#define SPEED_1_12 11
#define SPEED_1_24 12
#define SPEED_3_1 13
#define SPEED_6_1 14
#define SPEED_12_1 15
#define SPEED_24_1 16

// Trig conditions:
// Byte structure
// Bit 1:   Enabled / Disabled
// Bit 2:   Inversed / Not inversed (for example, Trig 4-4 = ...x, inversed = xxx.)
// Bit 3-8: Mode (see list below, 64 modes in total)
#define TRIG_1_2 0 // TRIG_2_1 is TRIG_1_1 inversed
#define TRIG_1_3 1
#define TRIG_2_3 2
#define TRIG_3_3 3
#define TRIG_1_4 4
#define TRIG_2_4 5
#define TRIG_3_4 6
#define TRIG_4_4 7
#define TRIG_1_5 8
#define TRIG_2_5 9
#define TRIG_3_5 10
#define TRIG_4_5 11
#define TRIG_5_5 12
#define TRIG_1_6 13
#define TRIG_2_6 14
#define TRIG_3_6 15
#define TRIG_4_6 16
#define TRIG_5_6 17
#define TRIG_6_6 18
#define TRIG_1_7 19
#define TRIG_2_7 20
#define TRIG_3_7 21
#define TRIG_4_7 22
#define TRIG_5_7 23
#define TRIG_6_7 24
#define TRIG_7_7 25
#define TRIG_1_8 26
#define TRIG_2_8 27
#define TRIG_3_8 28
#define TRIG_4_8 29
#define TRIG_5_8 30
#define TRIG_6_8 31
#define TRIG_7_8 32
#define TRIG_8_8 33
#define TRIG_1_PERCENT 34
#define TRIG_2_PERCENT 35
#define TRIG_5_PERCENT 36
#define TRIG_10_PERCENT 37
#define TRIG_25_PERCENT 38
#define TRIG_33_PERCENT 39
#define TRIG_50_PERCENT 40  // Higher than 50% are the inversed versions of these
#define TRIG_FILL 41        // When fill is trigged
#define TRIG_FIRST 42       // When first entering a pattern
#define TRIG_TRANSITION 43  // When transitioning to another pattern
#define TRIG_FIRST_PAGE 44          // When first entering a page
#define TRIG_TRANSITION_PAGE 45     // When transitioning to another page
#define TRIG_HIGHER_FIRST 46        // When first entering a higher pattern
#define TRIG_HIGHER_TRANSITION 47   // When transitioning to a higher pattern
#define TRIG_HIGHER_FIRST_PAGE 48       // When first entering a higher page
#define TRIG_HIGHER_TRANSITION_PAGE 49  // When transitioning to a higher page
#define TRIG_LOWER_FIRST 50             // When first entering a lower pattern
#define TRIG_LOWER_TRANSITION 51        // When transitioning to a lower pattern
#define TRIG_LOWER_FIRST_PAGE 52       // When first entering a lower page
#define TRIG_LOWER_TRANSITION_PAGE 53  // When transitioning to a lower page

/**
 * Update the sequencer according to user input
 */
void updateSequencer(
    struct Track *selectedTrack,
    bool keyStates[SDL_NUM_SCANCODES], 
    SDL_Scancode key
);

/**
 * Check if we need to do some key repeat actions
 */
void checkSequencerForKeyRepeats(
    struct Track *selectedTrack,
    bool keyStates[SDL_NUM_SCANCODES]
);

/**
 * Run the sequencer
 */
void runSequencer(
    PmStream *outputStream,
    int *ppqnCounter, 
    struct Track *selectedTrack
);

/**
 * Draw the sequencer
 */
void drawSequencer(
    int *ppqnCounter,
    bool keyStates[SDL_NUM_SCANCODES],
    struct Track *track
);

/**
 * Calculate the step index
 */
int calculateTrackStepIndex(
    int ppqnStep, 
    int pageSize, 
    int totalTrackLength,
    int pagePlayMode
);

/**
 * Reset the selected step
 */
void resetSequencerSelectedStep();

#endif
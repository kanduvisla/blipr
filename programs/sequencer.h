#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <SDL.h>
#include <stdbool.h>
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
#define TRIG_NONE 0
#define TRIG_5_PERCENT 1
#define TRIG_10_PERCENT 2
#define TRIG_25_PERCENT 3
#define TRIG_50_PERCENT 4
#define TRIG_75_PERCENT 5
#define TRIG_90_PERCENT 6
#define TRIG_FIRST 7
#define TRIG_NOT_FIRST 8
#define TRIG_TRANSITION 9
#define TRIG_NOT_TRANSITION 10
#define TRIG_1_2 11
#define TRIG_2_2 12
#define TRIG_1_3 13
#define TRIG_2_3 14
#define TRIG_3_3 15
#define TRIG_1_4 16
#define TRIG_2_4 17
#define TRIG_3_4 18
#define TRIG_4_4 19
#define TRIG_1_5 20
#define TRIG_2_5 21
#define TRIG_3_5 22
#define TRIG_4_5 23
#define TRIG_5_5 24
#define TRIG_1_6 25
#define TRIG_2_6 26
#define TRIG_3_6 27
#define TRIG_4_6 28
#define TRIG_5_6 29
#define TRIG_6_6 30
#define TRIG_1_7 31
#define TRIG_2_7 32
#define TRIG_3_7 33
#define TRIG_4_7 34
#define TRIG_5_7 35
#define TRIG_6_7 36
#define TRIG_7_7 37
#define TRIG_1_8 38
#define TRIG_2_8 39
#define TRIG_3_8 40
#define TRIG_4_8 41
#define TRIG_5_8 42
#define TRIG_6_8 43
#define TRIG_7_8 44
#define TRIG_8_8 45
#define TRIG_1_3_INVERSED 46
#define TRIG_2_3_INVERSED 47
#define TRIG_3_3_INVERSED 48
#define TRIG_1_4_INVERSED 49
#define TRIG_2_4_INVERSED 50
#define TRIG_3_4_INVERSED 51
#define TRIG_4_4_INVERSED 52
#define TRIG_1_5_INVERSED 53
#define TRIG_2_5_INVERSED 54
#define TRIG_3_5_INVERSED 55
#define TRIG_4_5_INVERSED 56
#define TRIG_5_5_INVERSED 57
#define TRIG_1_6_INVERSED 58
#define TRIG_2_6_INVERSED 59
#define TRIG_3_6_INVERSED 60
#define TRIG_4_6_INVERSED 61
#define TRIG_5_6_INVERSED 62
#define TRIG_6_6_INVERSED 63
#define TRIG_1_7_INVERSED 64
#define TRIG_2_7_INVERSED 65
#define TRIG_3_7_INVERSED 66
#define TRIG_4_7_INVERSED 67
#define TRIG_5_7_INVERSED 68
#define TRIG_6_7_INVERSED 69
#define TRIG_7_7_INVERSED 70
#define TRIG_1_8_INVERSED 71
#define TRIG_2_8_INVERSED 72
#define TRIG_3_8_INVERSED 73
#define TRIG_4_8_INVERSED 74
#define TRIG_5_8_INVERSED 75
#define TRIG_6_8_INVERSED 76
#define TRIG_7_8_INVERSED 77
#define TRIG_8_8_INVERSED 78

/**
 * Update the sequencer according to user input
 */
void updateSequencer(struct Project *project, bool keyStates[SDL_NUM_SCANCODES], SDL_Scancode key, int selectedSequence, int selectedPatern, int selectedTrack);

/**
 * Run the sequencer
 */
void runSequencer(
    struct Project *project, 
    int *ppqnCounter, 
    int selectedSequence, 
    int selectedPatern,
    struct Track *selectedTrack
);

/**
 * Draw the sequencer
 */
void drawSequencer(
    int *ppqnCounter, 
    struct Track *track
);

#endif
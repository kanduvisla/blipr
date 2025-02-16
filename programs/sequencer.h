#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <SDL.h>
#include <stdbool.h>
#include "../project.h"

/**
 * Update the sequencer according to user input
 */
void updateSequencer(struct Project *project, bool keyStates[SDL_NUM_SCANCODES], SDL_Scancode key, int selectedSequence, int selectedPatern, int selectedTrack);

/**
 * Run the sequencer
 */
void runSequencer(struct Project *project, int *ppqnCounter, int selectedSequence, int selectedPatern, int selectedTrack);

/**
 * Draw the sequencer
 */
void drawSequencer(struct Project *project, int *noteCounter, int selectedSequence, int selectedPatern, int selectedTrack);

#endif
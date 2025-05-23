#ifndef CONFIG_SELECTION_H
#define CONFIG_SELECTION_H

#include <SDL.h>
#include "../project.h"

/**
 * Draw the config selection
 */
void drawConfigSelection(struct Project *project);

/**
 * Process a key during configuration mode
 * Has some additional boolean flags that can affect the state
 */
void updateConfiguration(struct Project *project, SDL_Scancode key, bool *reloadMidi, bool *quit);

/**
 * Reset configuration screen to start state
 */
void resetConfigurationScreen();

#endif
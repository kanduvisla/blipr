#ifndef CONFIG_SELECTION_H
#define CONFIG_SELECTION_H

#include <SDL.h>
#include "../project.h"

/**
 * Draw the config selection
 */
void drawConfigSelection();

/**
 * Process a key during configuration mode
 */
void updateConfiguration(struct Project *project, SDL_Scancode key);

/**
 * Reset configuration screen to start state
 */
void resetConfigurationScreen();

#endif
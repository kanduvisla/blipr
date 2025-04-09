#ifndef CONFIG_SELECTION_H
#define CONFIG_SELECTION_H

#include <SDL.h>
#include "../project.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Draw the config selection
 */
void drawConfigSelection(const struct Project *project, const bool isPlaying);

/**
 * Process a key during configuration mode
 * Has some additional boolean flags that can affect the state
 */
void updateConfiguration(struct Project *project, SDL_Scancode key, bool *reloadMidi, bool *quit, bool *isPlaying);

/**
 * Reset configuration screen to start state
 */
void resetConfigurationScreen();

#ifdef __cplusplus
}
#endif

#endif
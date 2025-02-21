#ifndef UTILS_H
#define UTILS_H

#include <SDL.h>

/**
 * Convert a scancode to a step index
 */
int scancodeToStep(SDL_Scancode key);

/**
 * Uppercase a string
 */
void upperCase(char *str);

#endif
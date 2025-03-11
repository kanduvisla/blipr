#ifndef PATTERN_OPTIONS_H
#define PATTERN_OPTIONS_H

#include <SDL.h>
#include "../project.h"

void drawPatternOptions(struct Pattern* pattern);
void updatePatternOptions(struct Pattern* pattern, SDL_Scancode key);

#endif
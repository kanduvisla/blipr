#ifndef PATTERN_OPTIONS_H
#define PATTERN_OPTIONS_H

#include <SDL.h>
#include "../project.h"

#ifdef __cplusplus
extern "C" {
#endif

void drawPatternOptions(struct Pattern* pattern);
void updatePatternOptions(struct Pattern* pattern, SDL_Scancode key);

#ifdef __cplusplus
}
#endif

#endif
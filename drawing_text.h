#ifndef DRAWING_TEXT_H
#define DRAWING_TEXT_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Draw a single character
 */
void drawCharacter(int startX, int startY, const char character, SDL_Color color);

/**
 * Draw a line of text
 */
void drawText(int startX, int startY, const char* text, int maxWidth, SDL_Color color);

/**
 * Draw a centered line of text
 */
void drawCenteredLine(int startX, int startY, const char* text, int totalWidth, SDL_Color color);

/**
 * Initialize the textures to speed up text rendering
 */
void initializeTextures();

/**
 * Cleanup textures
 */
void cleanupTextures();

#ifdef __cplusplus
}
#endif

#endif
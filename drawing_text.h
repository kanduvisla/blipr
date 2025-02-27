#ifndef DRAWING_TEXT_H
#define DRAWING_TEXT_H

#include <SDL.h>

void drawCharacter(int startX, int startY, char character, SDL_Color color);
void drawText(int startX, int startY, const char* text, int maxWidth, SDL_Color color);
void drawCenteredLine(int startX, int startY, const char* text, int totalWidth, SDL_Color color);
void initializeTextures();
void cleanupTextures();

#endif
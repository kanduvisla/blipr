#ifndef DRAWING_H
#define DRAWING_H

#include <SDL.h>
#include <stdbool.h>

void drawRect(int x, int y, int width, int height, SDL_Color color);
void drawLine(int x1, int y1, int x2, int y2, SDL_Color color);
void drawBeveledRectOutline(int x, int y, int width, int height, SDL_Color color, bool inversed);
void drawRectOutline(int x, int y, int width, int height, int thickness, SDL_Color color);
void drawSingleLineRectOutline(int x, int y, int width, int height, SDL_Color color);
void drawBeveledRect(int x, int y, int width, int height, SDL_Color color);
void drawPixel(int x, int y, SDL_Color color);

#endif
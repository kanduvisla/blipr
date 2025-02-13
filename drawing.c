#include <SDL.h>
#include <stdbool.h>
#include "globals.h"
#include "drawing_utils.h"
#include "drawing.h"

/**
 * Draw a rectangle
 */
void drawRect(int x, int y, int width, int height, SDL_Color color) {
    // Set the draw color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw top line
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderFillRect(renderer, &rect);
}

/**
 * Draw a single line
 */
void drawLine(int x1, int y1, int x2, int y2, SDL_Color color) {
    setColor(color);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

/**
 * Draw an outlined rect
 */ 
void drawBeveledRectOutline(int x, int y, int width, int height, SDL_Color color, bool inversed) {
    SDL_Color darker = adjustColorBrightness(color, -0.5f);
    SDL_Color lighter = adjustColorBrightness(color, 0.5f);

    // Set the draw color
    setColor(inversed ? darker : lighter);

    // Draw top line
    SDL_RenderDrawLine(renderer, x + 1, y, x + width - 2, y);

    // Draw left line
    SDL_RenderDrawLine(renderer, x, y + 1, x, y + height - 2);

    // Set the draw color
    setColor(inversed ? lighter : darker);

    // Draw bottom line
    SDL_RenderDrawLine(renderer, x + 1, y + height - 1, x + width - 2, y + height - 1);

    // // Draw right line
    SDL_RenderDrawLine(renderer, x + width - 1, y + 1, x + width - 1, y + height - 2);
}

/**
 * Draw an outlined rect
 */
void drawRectOutline(int x, int y, int width, int height, int thickness, SDL_Color color) {
    // Set the draw color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // // Set the rect
    // SDL_Rect rectToDraw = {x, y, width, height};
    
    // Draw top line
    SDL_Rect topRect = {x, y, width, thickness};
    SDL_RenderFillRect(renderer, &topRect);

    // Draw bottom line
    SDL_Rect bottomRect = {x, y + height - thickness, width, thickness};
    SDL_RenderFillRect(renderer, &bottomRect);

    // Draw left line
    SDL_Rect leftRect = {x, y, thickness, height};
    SDL_RenderFillRect(renderer, &leftRect);

    // // Draw right line
    SDL_Rect rightRect = {x + width - thickness, y, thickness, height};
    SDL_RenderFillRect(renderer, &rightRect);
}

void drawSingleLineRectOutline(int x, int y, int width, int height, SDL_Color color) {
    
}

/**
 * Draw a beveled rect
 */
void drawBeveledRect(int x, int y, int width, int height, SDL_Color color) {
    drawRect(x + 1, y + 1, width - 2, height - 2, color);
    drawBeveledRectOutline(x, y, width, height, color, false);
}

/**
 * Draw a single pixel
 */
void drawPixel(int x, int y, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, x, y);
}


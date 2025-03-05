#include <SDL.h>
#include "drawing_text.h"
#include "drawing.h"
#include "constants.h"
#include "colors.h"
#include "globals.h"

// Define the size of each character
#define CHAR_WIDTH 5
#define CHAR_HEIGHT 5
#define CHAR_SPACING 1
#define SPACE_WIDTH 6
#define LINE_SPACING 2

#define NUM_CHARACTERS 54  // A-Z, 0-9, and special characters

SDL_Texture* charTextures[NUM_CHARACTERS] = {NULL};

// Define the 5x5 matrix for each character
// 1 represents a pixel to be drawn, 0 represents an empty space
const int characters[][CHAR_HEIGHT][CHAR_WIDTH] = {
    // A
    {{0,1,1,1,0},
     {1,0,0,0,1},
     {1,1,1,1,1},
     {1,0,0,0,1},
     {1,0,0,0,1}},
    // B
    {{1,1,1,1,0},
     {1,0,0,0,1},
     {1,1,1,1,0},
     {1,0,0,0,1},
     {1,1,1,1,0}},
    // C
    {{0,1,1,1,1},
     {1,0,0,0,0},
     {1,0,0,0,0},
     {1,0,0,0,0},
     {0,1,1,1,1}},
    // D
    {{1,1,1,1,0},
     {1,0,0,0,1},
     {1,0,0,0,1},
     {1,0,0,0,1},
     {1,1,1,1,0}},
    // E
    {{1,1,1,1,1},
     {1,0,0,0,0},
     {1,1,1,1,0},
     {1,0,0,0,0},
     {1,1,1,1,1}},
    // F
    {{1,1,1,1,1},
     {1,0,0,0,0},
     {1,1,1,1,0},
     {1,0,0,0,0},
     {1,0,0,0,0}},
    // G
    {{0,1,1,1,1},
     {1,0,0,0,0},
     {1,0,1,1,1},
     {1,0,0,0,1},
     {0,1,1,1,0}},
    // H
    {{1,0,0,0,1},
     {1,0,0,0,1},
     {1,1,1,1,1},
     {1,0,0,0,1},
     {1,0,0,0,1}},
    // I
    {{1,1,1,1,1},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {1,1,1,1,1}},
    // J
    {{0,0,0,0,1},
     {0,0,0,0,1},
     {0,0,0,0,1},
     {1,0,0,0,1},
     {0,1,1,1,0}},
    // K
    {{1,0,0,0,1},
     {1,0,0,1,0},
     {1,1,1,0,0},
     {1,0,0,1,0},
     {1,0,0,0,1}},
    // L
    {{1,0,0,0,0},
     {1,0,0,0,0},
     {1,0,0,0,0},
     {1,0,0,0,0},
     {1,1,1,1,1}},
    // M
    {{1,0,0,0,1},
     {1,1,0,1,1},
     {1,0,1,0,1},
     {1,0,0,0,1},
     {1,0,0,0,1}},
    // N
    {{1,0,0,0,1},
     {1,1,0,0,1},
     {1,0,1,0,1},
     {1,0,0,1,1},
     {1,0,0,0,1}},
    // O
    {{0,1,1,1,0},
     {1,0,0,0,1},
     {1,0,0,0,1},
     {1,0,0,0,1},
     {0,1,1,1,0}},
    // P
    {{1,1,1,1,0},
     {1,0,0,0,1},
     {1,1,1,1,0},
     {1,0,0,0,0},
     {1,0,0,0,0}},
    // Q
    {{0,1,1,1,0},
     {1,0,0,0,1},
     {1,0,1,0,1},
     {1,0,0,1,0},
     {0,1,1,0,1}},
    // R
    {{1,1,1,1,0},
     {1,0,0,0,1},
     {1,1,1,1,0},
     {1,0,0,1,0},
     {1,0,0,0,1}},
    // S
    {{0,1,1,1,1},
     {1,0,0,0,0},
     {0,1,1,1,0},
     {0,0,0,0,1},
     {1,1,1,1,0}},
    // T
    {{1,1,1,1,1},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0}},
    // U
    {{1,0,0,0,1},
     {1,0,0,0,1},
     {1,0,0,0,1},
     {1,0,0,0,1},
     {0,1,1,1,0}},
    // V
    {{1,0,0,0,1},
     {1,0,0,0,1},
     {1,0,0,0,1},
     {0,1,0,1,0},
     {0,0,1,0,0}},
    // W
    {{1,0,0,0,1},
     {1,0,0,0,1},
     {1,0,1,0,1},
     {1,0,1,0,1},
     {0,1,0,1,0}},
    // X
    {{1,0,0,0,1},
     {0,1,0,1,0},
     {0,0,1,0,0},
     {0,1,0,1,0},
     {1,0,0,0,1}},
    // Y
    {{1,0,0,0,1},
     {0,1,0,1,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0}},
    // Z
    {{1,1,1,1,1},
     {0,0,0,1,0},
     {0,0,1,0,0},
     {0,1,0,0,0},
     {1,1,1,1,1}},
    // 0
    {{0,1,1,1,0},
     {1,0,0,1,1},
     {1,0,1,0,1},
     {1,1,0,0,1},
     {0,1,1,1,0}},
    // 1
    {{0,0,1,0,0},
     {0,1,1,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,1,1,1,0}},
    // 2
    {{0,1,1,1,0},
     {1,0,0,0,1},
     {0,0,1,1,0},
     {0,1,0,0,0},
     {1,1,1,1,1}},
    // 3
    {{0,1,1,1,0},
     {1,0,0,0,1},
     {0,0,1,1,0},
     {1,0,0,0,1},
     {0,1,1,1,0}},
    // 4
    {{0,0,1,1,0},
     {0,1,0,1,0},
     {1,0,0,1,0},
     {1,1,1,1,1},
     {0,0,0,1,0}},
    // 5
    {{1,1,1,1,1},
     {1,0,0,0,0},
     {1,1,1,1,0},
     {0,0,0,0,1},
     {1,1,1,1,0}},
    // 6
    {{0,1,1,1,0},
     {1,0,0,0,0},
     {1,1,1,1,0},
     {1,0,0,0,1},
     {0,1,1,1,0}},
    // 7
    {{1,1,1,1,1},
     {0,0,0,0,1},
     {0,0,0,1,0},
     {0,0,1,0,0},
     {0,1,0,0,0}},
    // 8
    {{0,1,1,1,0},
     {1,0,0,0,1},
     {0,1,1,1,0},
     {1,0,0,0,1},
     {0,1,1,1,0}},
    // 9
    {{0,1,1,1,0},
     {1,0,0,0,1},
     {0,1,1,1,1},
     {0,0,0,0,1},
     {0,1,1,1,0}},
    // . (period)
    {{0,0,0,0,0},
     {0,0,0,0,0},
     {0,0,0,0,0},
     {0,0,0,0,0},
     {0,0,1,0,0}},
    // , (comma)
    {{0,0,0,0,0},
     {0,0,0,0,0},
     {0,0,0,0,0},
     {0,0,1,0,0},
     {0,1,0,0,0}},
    // ! (exclamation mark)
    {{0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,0,0,0},
     {0,0,1,0,0}},
    // ? (question mark)
    {{0,1,1,1,0},
     {1,0,0,0,1},
     {0,0,1,1,0},
     {0,0,0,0,0},
     {0,0,1,0,0}},
    // : (colon)
    {{0,0,0,0,0},
     {0,0,1,0,0},
     {0,0,0,0,0},
     {0,0,1,0,0},
     {0,0,0,0,0}},
    // - (hyphen)
    {{0,0,0,0,0},
     {0,0,0,0,0},
     {1,1,1,1,1},
     {0,0,0,0,0},
     {0,0,0,0,0}},
    // - [
    {{0,0,1,1,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,1,1,0}},
    // - ]
    {{0,1,1,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,1,1,0,0}},
    // - (
    {{0,0,0,1,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,0,1,0}},
    // - )
    {{0,1,0,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,0,1,0,0},
     {0,1,0,0,0}},
    // - ^
    {{0,0,1,0,0},
     {0,1,0,1,0},
     {0,0,0,0,0},
     {0,0,0,0,0},
     {0,0,0,0,0}},
    // - *
    {{1,0,1,0,1},
     {0,1,1,1,0},
     {1,1,1,1,1},
     {0,1,1,1,0},
     {1,0,1,0,1}},
    // - &
    {{0,0,1,0,0},
     {0,1,0,1,0},
     {0,0,1,1,1},
     {0,1,0,1,0},
     {0,0,1,0,1}},
    // - <
    {{0,0,0,1,0},
     {0,0,1,0,0},
     {0,1,0,0,0},
     {0,0,1,0,0},
     {0,0,0,1,0}},
    // - >
    {{0,1,0,0,0},
     {0,0,1,0,0},
     {0,0,0,1,0},
     {0,0,1,0,0},
     {0,1,0,0,0}},
    // - +
    {{0,0,1,0,0},
     {0,0,1,0,0},
     {1,1,1,1,1},
     {0,0,1,0,0},
     {0,0,1,0,0}},
    // - #
    {{0,1,0,1,0},
     {1,1,1,1,1},
     {0,1,0,1,0},
     {1,1,1,1,1},
     {0,1,0,1,0}},
    // - %
    {{1,0,0,1,1},
     {0,0,0,1,1},
     {0,0,1,0,0},
     {1,1,0,0,0},
     {1,0,0,0,1}}
};

void initializeTextures() {
    for (int i = 0; i < NUM_CHARACTERS; i++) {
        SDL_Surface* surface = SDL_CreateRGBSurface(0, CHAR_WIDTH, CHAR_HEIGHT, 32, 0, 0, 0, 0);
        if (surface == NULL) {
            SDL_Log("Failed to create surface for character %d: %s", i, SDL_GetError());
            continue;
        }

        SDL_LockSurface(surface);
        Uint32* pixels = (Uint32*)surface->pixels;

        for (int y = 0; y < CHAR_HEIGHT; y++) {
            for (int x = 0; x < CHAR_WIDTH; x++) {
                if (characters[i][y][x] == 1) {
                    pixels[y * CHAR_WIDTH + x] = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
                } else {
                    pixels[y * CHAR_WIDTH + x] = SDL_MapRGBA(surface->format, 0, 0, 0, 0);
                }
            }
        }

        SDL_UnlockSurface(surface);

        charTextures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (charTextures[i] == NULL) {
            SDL_Log("Failed to create texture for character %d: %s", i, SDL_GetError());
        }
    }
}

void cleanupTextures() {
    for (int i = 0; i < NUM_CHARACTERS; i++) {
        if (charTextures[i]) {
            SDL_DestroyTexture(charTextures[i]);
            charTextures[i] = NULL;
        }
    }
}

/**
 * Draw a single character
 */
void drawCharacter(int startX, int startY, char character, SDL_Color color) {
    int charIndex;

    if (character >= 'A' && character <= 'Z') {
        charIndex = character - 'A';
    } else if (character >= '0' && character <= '9') {
        charIndex = character - '0' + 26;  // 26 letters before numbers
    } else {
        switch (character) {
            case '.': charIndex = 36; break;
            case ',': charIndex = 37; break;
            case '!': charIndex = 38; break;
            case '?': charIndex = 39; break;
            case ':': charIndex = 40; break;
            case '-': charIndex = 41; break;
            case '[': charIndex = 42; break;
            case ']': charIndex = 43; break;
            case '(': charIndex = 44; break;
            case ')': charIndex = 45; break;
            case '^': charIndex = 46; break;
            case '*': charIndex = 47; break;
            case '&': charIndex = 48; break;
            case '<': charIndex = 49; break;
            case '>': charIndex = 50; break;
            case '+': charIndex = 51; break;
            case '#': charIndex = 52; break;
            case '%': charIndex = 53; break;
            default: return;  // Unsupported character
        }
    }

    // Draw the character
    if (charTextures[charIndex]) {
        SDL_Rect destRect = {startX, startY, CHAR_WIDTH, CHAR_HEIGHT};
        SDL_SetTextureColorMod(charTextures[charIndex], color.r, color.g, color.b);
        SDL_RenderCopy(renderer, charTextures[charIndex], NULL, &destRect);
    }
}

void drawText(int startX, int startY, const char* text, int maxWidth, SDL_Color color) {
    int currentX = startX;
    int currentY = startY;
    const char* wordStart = text;

    while (*text) {
        if (*text == ' ') {
            // Space character
            if (currentX + SPACE_WIDTH > startX + maxWidth) {
                // Start a new line
                currentX = startX;
                currentY += (CHAR_HEIGHT + LINE_SPACING);
            } else {
                currentX += SPACE_WIDTH;
            }
            wordStart = text + 1;
            if (*wordStart) {
                // Calculate the width of the next word
                int wordWidth = 0;
                const char* wordEnd = wordStart;
                while (*wordEnd && *wordEnd != ' ') {
                    wordWidth += (CHAR_WIDTH + CHAR_SPACING);
                    wordEnd++;
                }
                 // Check if the word fits on the current line
                if (currentX + wordWidth > startX + maxWidth && currentX > startX) {
                    // Start a new line
                    currentX = startX;
                    currentY += (CHAR_HEIGHT + LINE_SPACING);
                }
            }
        } else {
            // Draw the character
            drawCharacter(currentX, currentY, *text, color);
            currentX += (CHAR_WIDTH + CHAR_SPACING);
        }

        text++;
    }
}

void drawCenteredLine(int startX, int startY, const char* text, int totalWidth, SDL_Color color) {
    int textWidth = (strlen(text) * (CHAR_WIDTH + CHAR_SPACING));

    drawText(
        startX + (totalWidth - textWidth) / 2,
        startY, 
        text, 
        totalWidth, 
        color
    );
}
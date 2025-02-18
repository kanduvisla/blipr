#include <SDL.h>
#include "../drawing_components.h"
#include "../utils.h"

/**
 * Update the pattern selection according to user input
 */
void updatePatternSelection(int *selectedPattern, SDL_Scancode key) {
    int index = scancodeToStep(key);
    if (index == -1) {
        return;
    }

    *selectedPattern = index;
}

/**
 * Draw the pattern selection
 */
void drawPatternSelection(int *selectedPattern) {
    drawHighlightedGridTile(*selectedPattern);

    // Draw numbers 1-16:

}
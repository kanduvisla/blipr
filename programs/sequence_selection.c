#include <SDL.h>
#include "../drawing_components.h"
#include "../utils.h"

/**
 * Update the sequence selection according to user input
 */
void updateSequenceSelection(int *selectedSequence, SDL_Scancode key) {
    int index = scancodeToStep(key);
    if (index == -1) {
        return;
    }

    *selectedSequence = index;
}

/**
 * Draw the sequence selection
 */
void drawSequenceSelection(int *selectedSequence) {
    drawHighlightedGridTile(*selectedSequence);

    // Draw numbers 1-16:

}
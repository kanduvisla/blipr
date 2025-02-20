#include <SDL.h>
#include "../drawing_components.h"
#include "../utils.h"
#include "../constants.h"
#include "../drawing_text.h"
#include "../colors.h"

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
    drawBasicNumbers();
    drawHighlightedGridTile(*selectedSequence);

    // Title:
    drawCenteredLine(2, 133, "SELECT SEQUENCE", TITLE_WIDTH, COLOR_WHITE);
}
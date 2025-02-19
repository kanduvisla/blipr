#include <SDL.h>
#include "../drawing_components.h"
#include "../utils.h"
#include "../constants.h"
#include "../drawing_text.h"
#include "../colors.h"

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
    drawBasicNumbers();
    drawHighlightedGridTile(*selectedPattern);

    // Title:
    drawCenteredLine(2, 133, "SELECT PATTERN", TITLE_WIDTH, COLOR_WHITE);
}
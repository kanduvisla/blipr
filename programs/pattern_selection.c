#include <SDL.h>
#include "../drawing_components.h"
#include "../utils.h"
#include "../constants.h"
#include "../drawing_text.h"
#include "../colors.h"

/**
 * Update the pattern selection according to user input
 */
int updatePatternSelection(SDL_Scancode key) {
    int index = scancodeToStep(key);
    return index;
}

/**
 * Draw the pattern selection
 */
void drawPatternSelection(const int selectedPattern, const int queuedPattern) {
    drawBasicNumbers();
    drawHighlightedGridTile(selectedPattern);

    if (selectedPattern != queuedPattern) {
        drawHighlightedGridTileInColor(queuedPattern, COLOR_RED);
    }

    // Title:
    drawCenteredLine(2, 133, "SELECT PATTERN", TITLE_WIDTH, COLOR_WHITE);
    
    // ABCD Buttons:
    char descriptions[4][4] = {"PAT", "SEQ", "CFG", "TRN"};
    drawABCDButtons(descriptions);
    drawHighlightedGridTile(16);
}
#include <SDL.h>
#include "../drawing_components.h"
#include "../drawing_text.h"
#include "../utils.h"
#include "../project.h"
#include "../constants.h"
#include "../colors.h"

/**
 * Draw the program selection
 */
void drawProgramSelection(struct Track *track) {
    drawBasicNumbers();
    drawHighlightedGridTile(track->program);

    // Title:
    drawCenteredLine(2, 133, "SELECT PROGRAM", TITLE_WIDTH, COLOR_WHITE);
}

/**
 * Process a key during program selection
 */
void updateProgram(struct Track *track, SDL_Scancode key) {
    int index = scancodeToStep(key);
    if (index == -1) {
        return;
    }

    track->program = index;    
}

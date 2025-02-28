#include <SDL.h>
#include "../drawing_components.h"
#include "../drawing_text.h"
#include "../drawing_icons.h"
#include "../utils.h"
#include "../project.h"
#include "../constants.h"
#include "../colors.h"

/**
 * Draw the program selection
 */
void drawProgramSelection(struct Track *track) {
    drawIconOnIndex(BLIPR_PROGRAM_NONE, BLIPR_ICON_CROSS);
    drawIconOnIndex(BLIPR_PROGRAM_SEQUENCER, BLIPR_ICON_SEQUENCER);
    drawIconOnIndex(BLIPR_PROGRAM_FOUR_ON_THE_FLOOR, BLIPR_ICON_FOOT_DOWN);
    drawHighlightedGridTile(track->program);

    // Title:
    drawCenteredLine(2, 133, "SELECT PROGRAM", TITLE_WIDTH, SDL_COLOR_WHITE);

    // ABCD Buttons:
    char descriptions[4][4] = {"TRK", "PRG", "PAT", "UTI"};
    drawABCDButtons(descriptions);
    drawHighlightedGridTile(17);    
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

#include <SDL.h>
#include <string.h>
#include "../constants.h"
#include "../colors.h"
#include "../project.h"
#include "../drawing_components.h"
#include "../drawing_text.h"

void drawPatternOptions(struct Pattern* pattern) {
    // BPM:
    drawCenteredLine(2, 5, "BPM", BUTTON_WIDTH * 2, COLOR_WHITE);
    drawTextOnButton(0, "<");
    drawTextOnButton(1, ">");
    char bpmText[4];
    sprintf(bpmText, "%d", pattern->bpm + 45);
    drawCenteredLine(2, 14, bpmText, BUTTON_WIDTH * 2, COLOR_WHITE);

    // ABCD Buttons:
    char descriptions[4][4] = {"TRK", "OPT", "PRG", "PAT"};
    drawABCDButtons(descriptions);
    drawHighlightedGridTile(19);    
}

void updatePatternOptions(struct Pattern* pattern, SDL_Scancode key) {
    switch (key) {
        case BLIPR_KEY_1:
            pattern->bpm = MAX(0, pattern->bpm - 1);
            break;
        case BLIPR_KEY_2:
            pattern->bpm = MIN(255, pattern->bpm + 1);
            break;
    }
}

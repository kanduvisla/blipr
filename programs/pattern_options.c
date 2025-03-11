#include <SDL.h>
#include <string.h>
#include "../constants.h"
#include "../colors.h"
#include "../project.h"
#include "../drawing_components.h"
#include "../drawing_text.h"
#include "../utils.h"

/**
 * Draw default increase- and decrease buttons
 */
void drawIncreaseAndDecreaseButtons(
    int firstButtonIndex,
    const char *header,
    const char *text
) {
    drawCenteredLine(
        2 + (BUTTON_WIDTH * (firstButtonIndex % 4)) + (firstButtonIndex % 4), 
        5 + (BUTTON_HEIGHT * (firstButtonIndex / 4)) + (firstButtonIndex % 4), 
        header, 
        BUTTON_WIDTH * 2, 
        COLOR_WHITE
    );
    drawTextOnButton(firstButtonIndex, "<");
    drawTextOnButton(firstButtonIndex + 1, ">");
    drawCenteredLine(
        2 + (BUTTON_WIDTH * (firstButtonIndex % 4)) + (firstButtonIndex % 4), 
        14 + (BUTTON_HEIGHT * (firstButtonIndex / 4)) + (firstButtonIndex % 4), 
        text, 
        BUTTON_WIDTH * 2, 
        COLOR_WHITE
    );
}

void drawPatternOptions(struct Pattern* pattern) {
    // BPM:
    char bpmText[4];
    sprintf(bpmText, "%d", pattern->bpm + 45);
    drawIncreaseAndDecreaseButtons(0, "BPM", bpmText);

    // Program (PC)
    char prog[6];
    sprintf(prog, "%d:%d", (pattern->programA >> 4) & 0x0F, pattern->programA & 0x0F);
    drawIncreaseAndDecreaseButtons(8, "PROG.A", prog);
    sprintf(prog, "%d:%d", (pattern->programB >> 4) & 0x0F, pattern->programB & 0x0F);
    drawIncreaseAndDecreaseButtons(10, "PROG.B", prog);
    sprintf(prog, "%d:%d", (pattern->programC >> 4) & 0x0F, pattern->programC & 0x0F);
    drawIncreaseAndDecreaseButtons(12, "PROG.C", prog);
    sprintf(prog, "%d:%d", (pattern->programD >> 4) & 0x0F, pattern->programD & 0x0F);
    drawIncreaseAndDecreaseButtons(14, "PROG.D", prog);

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
        case BLIPR_KEY_9:
            pattern->programA = MAX(0, pattern->programA - 1);
            break;
        case BLIPR_KEY_10:
            pattern->programA = MIN(127, pattern->programA + 1);
            break;
        case BLIPR_KEY_11:
            pattern->programB = MAX(0, pattern->programB - 1);
            break;
        case BLIPR_KEY_12:
            pattern->programB = MIN(127, pattern->programB + 1);
            break;
        case BLIPR_KEY_13:
            pattern->programC = MAX(0, pattern->programC - 1);
            break;
        case BLIPR_KEY_14:
            pattern->programC = MIN(127, pattern->programC + 1);
            break;
        case BLIPR_KEY_15:
            pattern->programD = MAX(0, pattern->programD - 1);
            break;
        case BLIPR_KEY_16:
            pattern->programD = MIN(127, pattern->programD + 1);
            break;
    }
}

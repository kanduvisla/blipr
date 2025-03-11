#include <SDL.h>
#include "../drawing_components.h"
#include "../utils.h"
#include "../constants.h"
#include "../drawing_text.h"
#include "../colors.h"

void updateTrackSelection(int *selectedTrack, SDL_Scancode key) {
    int index = scancodeToStep(key);
    if (index == -1) {
        return;
    }

    *selectedTrack = index;
}

void drawTrackSelection(int *selectedTrack) {
    drawBasicNumbers();
    drawHighlightedGridTile(*selectedTrack);

    // Title:
    drawCenteredLine(2, 133, "SELECT TRACK", TITLE_WIDTH, COLOR_WHITE);

    // ABCD Buttons:
    char descriptions[4][4] = {"TRK", "OPT", "PRG", "PAT"};
    drawABCDButtons(descriptions);
    drawHighlightedGridTile(16);
}
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

    // Track Selection doubles as entrypoint to conifguration:
    char descriptions[4][4] = {"PAT", "SEQ", "TRA", "CFG"};
    drawABCDButtons(descriptions);
}
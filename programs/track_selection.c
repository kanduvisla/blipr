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
    drawText(29, 133, "SELECT TRACK", 100, COLOR_WHITE);
}
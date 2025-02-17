#include <SDL.h>
#include <stdbool.h>
#include "../project.h"
#include "../constants.h"
#include "../colors.h"
#include "../drawing.h"
#include "../drawing_components.h"
#include "../utils.h"

void updateTrackSelection(int *selectedTrack, SDL_Scancode key) {
    int index = scancodeToStep(key);
    if (index == -1) {
        return;
    }

    *selectedTrack = index;
}

void drawTrackSelection(int *selectedTrack) {
    drawHighlightedGridTile(*selectedTrack);

    // Draw numbers 1-16:

}
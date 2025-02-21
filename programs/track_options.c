#include <string.h>
#include "../project.h"
#include "../drawing_components.h"
#include "../utils.h"
#include "../constants.h"
#include "../drawing_text.h"
#include "../colors.h"

void drawTrackOptions(struct Track* track) {
    // Title:
    drawCenteredLine(2, 133, "TRACK OPTIONS", TITLE_WIDTH, COLOR_WHITE);

    // Track length:
    char line[32];
    snprintf(line, sizeof(line), "TRACK LENGTH:%d", track->trackLength + 1);
    drawCenteredLine(2, 22, line, TITLE_WIDTH, COLOR_WHITE);
    drawTextOnButton(0, "<<");
    drawTextOnButton(1, "<");
    drawTextOnButton(2, ">");
    drawTextOnButton(3, ">>");
}

void updateTrackOptions(struct Track* track, SDL_Scancode key) {
    track->polyCount = 8;

    int maxLength = (9 - track->polyCount) * 64;

    switch (key) {
        case BLIPR_KEY_1:
            track->trackLength = MAX(0, track->trackLength - 16);
            break;
        case BLIPR_KEY_2:
            track->trackLength = MAX(0, track->trackLength - 1);
            break;
        case BLIPR_KEY_3:
            track->trackLength = MIN(maxLength - 1, track->trackLength + 1);
            break;
        case BLIPR_KEY_4:
            track->trackLength = MIN(maxLength - 1, track->trackLength + 16);
            break;
    }
}
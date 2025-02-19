#include <SDL.h>
#include <stdbool.h>
#include "../drawing_icons.h"
#include "../colors.h"
#include "../drawing_text.h"
#include "../drawing_components.h"
#include "../constants.h"
#include "../midi.h"

bool isMidiConfigActive = false;
int selectedMidiSlot = -1;

bool isMainScreen() {
    return !isMidiConfigActive;
}

void resetConfigurationScreen() {
    isMidiConfigActive = false;
    selectedMidiSlot = -1;
}

void drawConfigSelection() {
    if (isMainScreen()) {
        drawIcon(9, 9, BLIPR_ICON_MIDI, COLOR_WHITE);
        drawCenteredLine(2, 133, "CONFIGURATION", TITLE_WIDTH, COLOR_WHITE);        
    } else {
        if (isMidiConfigActive) {
            if (selectedMidiSlot < 0) {
                // Draw midi config screen
                drawCenteredLine(2, 133, "SELECT MIDI SLOT", TITLE_WIDTH, COLOR_WHITE);
                // Draw numbers 1-4:
                for (int i = 0; i < 4; i++) {
                    drawSingleNumber(i);
                }
            } else {
                // Show list of midi devices:
                drawCenteredLine(2, 133, "SELECT MIDI DEVICE", TITLE_WIDTH, COLOR_WHITE);
            }
        }            
    }
}

void updateConfiguration(SDL_Scancode key) {
    if (isMainScreen()) {
        // No config selected, so we're on the main screen
        if (key == BLIPR_KEY_1) {
            isMidiConfigActive = true;
        }
    } else {
        if (isMidiConfigActive) {
            if (selectedMidiSlot < 0) {
                if (key == BLIPR_KEY_1) { selectedMidiSlot = 0; }
                else if (key == BLIPR_KEY_2) { selectedMidiSlot = 1; }
                else if (key == BLIPR_KEY_3) { selectedMidiSlot = 2; }
                else if (key == BLIPR_KEY_3) { selectedMidiSlot = 3; }
            }
        }
    }
}
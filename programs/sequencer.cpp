#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include <string.h>
#include <cstdio>
#include <functional>
#include "sequencer.hpp"
#include "../project.h"
#include "../constants.h"
#include "../colors.h"
#include "../drawing.h"
#include "../drawing_components.h"
#include "../drawing_text.h"
#include "../utils.h"
#include "../midi.h"
#include "../print.h"
#include "../drawing_icons.h"
#include "../note_utilities.hpp"
#include "../trigg_helper.hpp"

/**
 * Constructor
 */
Sequencer::Sequencer() {}

/**
 * Destructor
 */
Sequencer::~Sequencer() {}

/**
 * Reset template note to default values
 */
void Sequencer::resetTemplateNote() {
    templateNote.enabled = false;
    templateNote.note = 60;     // C-4
    templateNote.velocity = 100;
    templateNote.length = 1;
    templateNote.nudge = PP16N; // PP16N = middle, nudge 0
    templateNote.trigg = 0;
    templateNote.cc1Value = 0;
    templateNote.cc2Value = 0;
}

/**
 * Toggle a step
 */
void Sequencer::toggleStep(struct Step *step, int noteIndex) {
    step->notes[noteIndex].enabled = !step->notes[noteIndex].enabled;
    // Set default values:
    if (step->notes[noteIndex].enabled) {
        // Use template note
        copyNote(&templateNote, &step->notes[noteIndex]);
        // Reset enabled state, because template note might be a disabled note :-/
        step->notes[noteIndex].enabled = true;
    }
}

/**
 * Apply a key on a single note in the note editor
 */
void Sequencer::applyKeyToNoteInNoteEditor(struct Note *note, SDL_Scancode key) {
    if (key == BLIPR_KEY_1) { 
        note->note = NoteUtilities::transpose(note->note, -12); 
        templateNote.note = note->note;
    } else if (key == BLIPR_KEY_2) {
        note->note = NoteUtilities::transpose(note->note, -1); 
        templateNote.note = note->note;
    } else if (key == BLIPR_KEY_3) {
        note->note = NoteUtilities::transpose(note->note, 1); 
        templateNote.note = note->note;
    } else if (key == BLIPR_KEY_4) {
        note->note = NoteUtilities::transpose(note->note, 12); 
        templateNote.note = note->note;
    } else {
        BaseSequencer::applyKeyToNoteInNoteEditor(note, key);
    }
}

/**
 * Draw the template note
 */
void Sequencer::drawTemplateNote() {
    // drawRect(SIDEBAR_OFFSET + 1, 27, SIDEBAR_WIDTH - 2, (CHAR_HEIGHT * 8) + 8, COLOR_BLACK);
    drawRect(SIDEBAR_OFFSET + 1, 27, SIDEBAR_WIDTH - 2, CHAR_HEIGHT + 2, COLOR_BLACK);
    drawText(SIDEBAR_OFFSET + 1, 28, "TEMPLATE", 20, COLOR_ORANGE);
    char text[8];
    drawSidebarTemplate(34, "NOT");
    const char *midiNote = getMidiNoteName(templateNote.note);
    drawText(SIDEBAR_OFFSET + 24, 35, midiNote, 20, COLOR_ORANGE);
    drawSidebarTemplate(40, "VEL");
    snprintf(text, sizeof(text), "%d", templateNote.velocity);
    drawText(SIDEBAR_OFFSET + 24, 41, text, 18, COLOR_ORANGE);
    drawSidebarTemplate(46, "LEN");
    snprintf(text, sizeof(text), "%d", templateNote.length);
    drawText(SIDEBAR_OFFSET + 24, 47, text, 18, COLOR_ORANGE);
    drawSidebarTemplate(52, "NDG");
    snprintf(text, sizeof(text), "%d", templateNote.nudge - PP16N);
    drawText(SIDEBAR_OFFSET + 24, 53, text, 18, COLOR_ORANGE);
    drawSidebarTemplate(58, "TRG");
    TriggHelper::setTriggText(templateNote.trigg, text);
    drawText(SIDEBAR_OFFSET + 24, 59, text, 18, COLOR_ORANGE);
    drawSidebarTemplate(64, "CC1");
    snprintf(text, sizeof(text), "%d", templateNote.cc1Value);
    drawText(SIDEBAR_OFFSET + 24, 65, text, 18, COLOR_ORANGE);
    drawSidebarTemplate(70, "CC2");
    snprintf(text, sizeof(text), "%d", templateNote.cc2Value);
    drawText(SIDEBAR_OFFSET + 24, 71, text, 18, COLOR_ORANGE);
}

/**
 * Draw an overlay on the step button (depends on sequencer type)
 */
void Sequencer::drawStepButtonOverlay(const int index, const struct Note *note, const struct Track *track) {
    int polyCount = getPolyCount(track);

    // Draw the note name:
    drawTextOnButton(index, getMidiNoteName(note->note));

    // Offset (in pixels) for the note indicator
    int noteIndicatorOffset = 12 - polyCount;

    // Draw the dots for poly count:
    drawRect(
        5 + index + ((index % 4) * BUTTON_WIDTH) + noteIndicatorOffset,
        5 + (index / 4) + ((index / 4) * BUTTON_HEIGHT),
        (polyCount * 2) + 1,
        3,
        COLOR_GRAY
    );

    for (int p=0; p<polyCount; p++) {
        drawPixel(
            6 + index + ((index % 4) * BUTTON_WIDTH) + (p * 2) + noteIndicatorOffset,
            6 + (index / 4) + ((index / 4) * BUTTON_HEIGHT),
            p == selectedNote ? COLOR_WHITE : (note->enabled ? COLOR_RED : COLOR_LIGHT_GRAY)
        );
    }
}

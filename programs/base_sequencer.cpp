#include "base_sequencer.hpp"
#include "../constants.h"
#include "../project.h"

// MARK: - Selecting steps

/**
 * Check if there are steps selected
 */
bool BaseSequencer::isStepsSelected() {
    int count = 0;
    for (int i=0; i<16; i++) {
        if (selectedSteps[i]) {
            count ++;
        }
    }
    return count > 0;
}

/**
 * Method to check if all step properties are the same
 */
void BaseSequencer::checkIfAllStepPropertiesAreTheSame(const struct Track *track) {
    for (int i=0; i<8; i++) {
        areAllStepPropertiesTheSame[i] = true;
    }

    // Iterate over the selected steps, and check if there is difference in properties of the selected steps:
    for (int i=1; i<16; i++) {
        if (selectedSteps[i] && selectedSteps[i - 1]) {
            int stepIndex = i + (track->selectedPage * 16);
            const struct Step *lhStep = &track->steps[stepIndex - 1];
            const struct Step *rhStep = &track->steps[stepIndex];
            areAllStepPropertiesTheSame[PROPERTY_CC1] &= lhStep->notes[selectedNote].cc1Value == rhStep->notes[selectedNote].cc1Value;
            areAllStepPropertiesTheSame[PROPERTY_CC2] &= lhStep->notes[selectedNote].cc2Value == rhStep->notes[selectedNote].cc2Value;
            areAllStepPropertiesTheSame[PROPERTY_NOTE] &= lhStep->notes[selectedNote].note == rhStep->notes[selectedNote].note;
            areAllStepPropertiesTheSame[PROPERTY_LENGTH] &= lhStep->notes[selectedNote].length == rhStep->notes[selectedNote].length;
            areAllStepPropertiesTheSame[PROPERTY_VELOCITY] &= lhStep->notes[selectedNote].velocity == rhStep->notes[selectedNote].velocity;
            areAllStepPropertiesTheSame[PROPERTY_NUDGE] &= lhStep->notes[selectedNote].nudge == rhStep->notes[selectedNote].nudge;
            areAllStepPropertiesTheSame[PROPERTY_TRIG] &= lhStep->notes[selectedNote].trigg == rhStep->notes[selectedNote].trigg;
            areAllStepPropertiesTheSame[PROPERTY_ENABLED] &= lhStep->notes[selectedNote].enabled == rhStep->notes[selectedNote].enabled;
        }
    }
}

/**
 * Reset the selected step
 */
void BaseSequencer::resetSelectedSteps() {
    for (int i=0; i<16; i++) {
        selectedSteps[i] = false;
    }
    isNoteEditorVisible = false;
    cutCounter = 0;
    copyCounter = 0;
}

/**
 * Reset the selected note
 */
void BaseSequencer::resetSelectedNote() {
    selectedNote = 0;
}

// MARK: - Cut / Copy / Paste actions

/**
 * Clear note
 */
void BaseSequencer::clearNote(struct Note *note) {
    note->cc1Value = 0;
    note->cc2Value = 0;
    note->enabled = false;
    note->length = 0;
    note->velocity = 0;
    note->trigg = 0;
    note->note = 0;
    note->nudge = PP16N;
}

/**
 * Clear step
 */
void BaseSequencer::clearStep(struct Step *step) {
    if (cutCounter == 0) {
        // This is the first cut, clear only the selected note:
        clearNote(&step->notes[selectedNote]);
    } else {
        // This is the second cut, clear all notes in this step
        for (int i=0; i<NOTES_IN_STEP; i++) {
            clearNote(&step->notes[i]);
        }
    }
}

/**
 * Copy note
 */
void BaseSequencer::copyNote(const struct Note *src, struct Note *dst) {
    dst->cc1Value = src->cc1Value;
    dst->cc2Value = src->cc2Value;
    dst->enabled = src->enabled;
    dst->length = src->length;
    dst->velocity = src->velocity;
    dst->trigg = src->trigg;
    dst->note = src->note;
    dst->nudge = src->nudge;
}

/**
 * Copy step
 */
void BaseSequencer::copyStep(const struct Step *src, struct Step *dst) {
    if (copyCounter == 0) {
        // This is the first copy, only copy the selected note:
        copyNote(&src->notes[selectedNote], &dst->notes[selectedNote]);
    } else {
        // This is the second copy, copy all the notes:
        for (int i=0; i<NOTES_IN_STEP; i++) {
            copyNote(&src->notes[i], &dst->notes[i]);
        }
    }
}

// MARK: - Pulse manipulation

/**
 * Apply track speed to pulse
 * returns FALSE if further processing is not required
 */
bool BaseSequencer::applySpeedToPulse(
    const struct Track *track,
    uint64_t *pulse
) {
    switch (track->speed) {
        case TRACK_SPEED_DIV_TWO:
            // Only process when modulo is 0 (to prevent doubles)
            if (*pulse % 2 != 0) {
                *pulse /= 2;
                return false;
            }
            *pulse /= 2;
            break;
        case TRACK_SPEED_DIV_FOUR:
            // Only process when modulo is 0 (to prevent doubles)
            if (*pulse % 4 != 0) {
                *pulse /= 4;
                return false;
            }
            *pulse /= 4;
            break;
        case TRACK_SPEED_DIV_EIGHT:
            // Only process when modulo is 0 (to prevent doubles)
            if (*pulse % 8 != 0) {
                *pulse /= 8;
                return false;
            }        
            *pulse /= 8;
            break;
        case TRACK_SPEED_TIMES_TWO:
            *pulse *= 2;
            break;
        case TRACK_SPEED_TIMES_FOUR:
            *pulse *= 4;
            break;
        case TRACK_SPEED_TIMES_EIGHT:
            *pulse *= 8;
            break;
    }

    return true;
}

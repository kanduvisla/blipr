#ifndef BASE_SEQUENCER_H
#define BASE_SEQUENCER_H

#include <portmidi.h>
#include "../project.h"
#include "abstract_program.hpp"

// Boolean arrays that determine if for a step all note properties are the same:
#define PROPERTY_CC1 0
#define PROPERTY_CC2 1
#define PROPERTY_NOTE 2
#define PROPERTY_LENGTH 3
#define PROPERTY_VELOCITY 4
#define PROPERTY_NUDGE 5
#define PROPERTY_TRIG 6
#define PROPERTY_ENABLED 7

// Speed conditions:
#define SPEED_1_1 0
#define SPEED_1_2 1
#define SPEED_1_4 2
#define SPEED_1_8 3
#define SPEED_1_16 4
#define SPEED_2_1 5
#define SPEED_4_1 6
#define SPEED_8_1 7
#define SPEED_16_1 8
#define SPEED_1_3 9
#define SPEED_1_6 10
#define SPEED_1_12 11
#define SPEED_1_24 12
#define SPEED_3_1 13
#define SPEED_6_1 14
#define SPEED_12_1 15
#define SPEED_24_1 16

// Trig conditions:
// Byte structure
// Bit 1:   Enabled / Disabled
// Bit 2:   Inversed / Not inversed (for example, Trig 4-4 = ...x, inversed = xxx.)
// Bit 3-8: Mode (see list below, 64 modes in total)
#define TRIG_DISABLED 0
#define TRIG_1_2 1 // TRIG_2_2 is TRIG_1_2 inversed
#define TRIG_1_3 2
#define TRIG_2_3 3
#define TRIG_3_3 4
#define TRIG_1_4 5
#define TRIG_2_4 6
#define TRIG_3_4 7
#define TRIG_4_4 8
#define TRIG_1_5 9
#define TRIG_2_5 10
#define TRIG_3_5 11
#define TRIG_4_5 12
#define TRIG_5_5 13
#define TRIG_1_6 14
#define TRIG_2_6 15
#define TRIG_3_6 16
#define TRIG_4_6 17
#define TRIG_5_6 18
#define TRIG_6_6 19
#define TRIG_1_7 20
#define TRIG_2_7 21
#define TRIG_3_7 22
#define TRIG_4_7 23
#define TRIG_5_7 24
#define TRIG_6_7 25
#define TRIG_7_7 26
#define TRIG_1_8 27
#define TRIG_2_8 28
#define TRIG_3_8 29
#define TRIG_4_8 30
#define TRIG_5_8 31
#define TRIG_6_8 32
#define TRIG_7_8 33
#define TRIG_8_8 34
#define TRIG_1_PERCENT 35
#define TRIG_2_PERCENT 36
#define TRIG_5_PERCENT 37
#define TRIG_10_PERCENT 38
#define TRIG_25_PERCENT 39
#define TRIG_33_PERCENT 40
#define TRIG_50_PERCENT 41  // Higher than 50% are the inversed versions of these
#define TRIG_FILL 42        // When fill is trigged (TODO: determine how "FILL" is used)
#define TRIG_FIRST 43       // When first entering a pattern
#define TRIG_TRANSITION 44  // When transitioning to another pattern
#define TRIG_FIRST_PAGE 45          // When first entering a page
#define TRIG_TRANSITION_PAGE 46     // When transitioning to another page
#define TRIG_HIGHER_FIRST 47        // When first entering a higher pattern
#define TRIG_HIGHER_TRANSITION 48   // When transitioning to a higher pattern
#define TRIG_HIGHER_FIRST_PAGE 49       // When first entering a higher page
#define TRIG_HIGHER_TRANSITION_PAGE 50  // When transitioning to a higher page
#define TRIG_LOWER_FIRST 51             // When first entering a lower pattern
#define TRIG_LOWER_TRANSITION 52        // When transitioning to a lower pattern
#define TRIG_LOWER_FIRST_PAGE 53       // When first entering a lower page
#define TRIG_LOWER_TRANSITION_PAGE 54  // When transitioning to a lower page
#define TRIG_FIRST_PAGE_PLAY 55         // Trigged when the page is played the 1st time (does not re-trig when the page is played a second time when coming back from another page)
#define TRIG_FIRST_PATTERN_PLAY 56      // Trigged when the pattern is played the 1st time (does not re-trig when the pattern is played a second time when coming back from another pattern)
#define TRIG_TRANSITION_LAST_PAGE 57     // When transitioning to another page and the last repeat of transition repeats is played
#define TRIG_HIGHEST_VALUE 56         // Used internally for decision making, make sure to change this if you add new trigg conditions

/**
 * Base Sequencer class
 * This is the base sequencer, for all sequencer-like programs
 * This includes operations such as:
 *  - Providing a way to select steps
 *  - Cut / Copy / Paste steps
 *  - Toggling a step on/off
 */
class BaseSequencer : public Program {
public:
    // MARK: - Public methods

    /**
     * Reset the selected step
     */
    void resetSelectedSteps();

    /**
     * Reset the selected note
     */
    void resetSelectedNote();

protected:
    // MARK: - Protected properties
    
    /**
     * Selected note / channel
     */
    int selectedNote = 0;

    /**
     * Boolean that determines if this step is selected or not
     */ 
    bool selectedSteps[16] = {false};

    /**
     * Boolean to keep track if all selected step properties are the same
     */
    bool areAllStepPropertiesTheSame[8] = {false};

    /**
     * Cut counter, 0=None, 1=Cutted Note, 2=Cutted Step (all notes)
     */
    int cutCounter = 0;

    /**
     * Copy counter, 0=None, 1=Cutted Note, 2=Cutted Step (all notes)
     */
    int copyCounter = 0;

    /**
     * Flag to determine if the note editor is visible or not
     */
    bool isNoteEditorVisible = false;

    /**
     * Are we editing all notes of this step? Or just one (the currently selected note / channel)
     */
    bool isEditOnAllNotes = false;

    // MARK: - Protected methods

    /**
     * Check if there are steps selected
     */
    bool isStepsSelected();

    /**
     * Method to check if all step properties are the same
     */
    void checkIfAllStepPropertiesAreTheSame(const struct Track *track);

    /**
     * Apply track speed to pulse
     * returns FALSE if further processing is not required
     */
    bool applySpeedToPulse(const struct Track *track, uint64_t *pulse);

    /**
     * Clear this note
     */
    void clearNote(struct Note *note);

    /**
     * Clear this step
     */
    void clearStep(struct Step *step);

    /**
     * Copy a note from source to destination
     */
    void copyNote(const struct Note *src, struct Note *dst);

    /**
     * Copy a step from source to destination
     */
    void copyStep(const struct Step *src, struct Step *dst);

    /**
     * Toggle a step
     */
    virtual void toggleStep(struct Step *step, int noteIndex) = 0;

    /**
     * Handle key input when note editor is visible
     */
    void handleKeyInNoteEditor(struct Track *selectedTrack, SDL_Scancode key);

    /**
     * Apply Key to Note in note editor
     */
    virtual void applyKeyToNoteInNoteEditor(struct Note *note, SDL_Scancode key) = 0;

    /**
     * Handle key input when shift 2 is down
     */
    virtual void handleKeyWithShift2Down(struct Track *track, int index) = 0;
private:

};    

#endif
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
     * Update method. 
     * This happens if there is input to update the parameters (such as a keydown event)
     */ 
    void update(struct Track *selectedTrack, bool keyStates[SDL_NUM_SCANCODES], SDL_Scancode key) override;

    /**
     * Run this program, gets triggered every clock pulse
     */ 
    void run(PmStream *outputStream, const uint64_t *ppqnCounter, struct Track *selectedTrack) override;

    /**
     * Draw this program, gets triggered every step
     */ 
    void draw(uint64_t *ppqnCounter, bool keyStates[SDL_NUM_SCANCODES], struct Track *track) override;

    /**
     * Reset the selected step
     */
    void resetSelectedSteps();

    /**
     * Reset the selected note
     */
    void resetSelectedNote();

    /**
     * Reset template note to default values
     */
    virtual void resetTemplateNote() = 0;

protected:
    // MARK: - Protected properties
    
    /**
     * Selected note / channel
     */
    int selectedNote = 0;

    /**
     * Selected page bank, not currently active playing page bank
     */
    int selectedPageBank = 0;

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
     * Clipboard of selected steps
     */
    struct Step* clipBoard[16] = {NULL};

    /**
     * Flag to determine if the note editor is visible or not
     */
    bool isNoteEditorVisible = false;

    /**
     * Are we editing all notes of this step? Or just one (the currently selected note / channel)
     */
    bool isEditOnAllNotes = false;

    /**
     * Is the user allowed to select notes?
     */
    bool isNoteSelectionAllowed = true;

    /** 
     * Template note that is used to determine what note to place when the user puts down a new note
     */
    struct Note templateNote;

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
     * Get the maximum number of page banks
     */
    int getMaxPageBanks(const struct Track *track);

    /**
     * Set selected page, or queue, depending on page play mode
     */
    void setSelectedPage(struct Track *selectedTrack, int index);

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
     * Select all steps between the first and last selected step
     */
    void sanitizeSelection(int indexPressed);

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
    virtual void applyKeyToNoteInNoteEditor(struct Note *note, SDL_Scancode key);

    /**
     * Handle key input when shift 2 is down
     */
    virtual void handleKeyWithShift2Down(struct Track *track, int index);

    /**
     * Callback when the first pulse of a page/track is played
     * Note that this is AFTER the note has already played
     */
    virtual void isFirstPulseCallback();

    /**
     * Get track step index - this is the index in the steps-array on the track
     */
    int getTrackStepIndex(const uint64_t *ppqnCounter,  const struct Track *track, bool sendFirstPulseCallback);

    /**
     * Get notes at a given track index - this takes into account the polyphony sacrifice for more steps
     */
    void getNotesAtTrackStepIndex(int trackStepIndex, const struct Track *track, const struct Note **notes);

    /**
     * Process a single pulse - keeps track of things like trigg, nudge, length, speed, shuffle, etc.
     */
    void processPulse(const uint64_t *currentPulse, const struct Track *track);

    /**
     * Helper method to contain all the conditions that define if a note is played or not
     */
    bool isNotePlayed(const struct Note *note, const struct Track *track, int nudgeCheck);

    /**
     * Draw the page indicator
     */
    void drawPageIndicator(const struct Track *track, int playingPage);

    /**
     * Draw the template note
     */
    virtual void drawTemplateNote();
    
    /**
     * Draw the main sequencer
     */
    void drawSequencerMain(uint64_t *ppqnCounter, bool keyStates[SDL_NUM_SCANCODES], struct Track *selectedTrack);
    
    /**
     * Draw the main sequencer with shift 2 down (overlay)
     * @return bool to determine if the main sequencer should still be drawn or not
     */
    virtual bool drawSequencerMainWithShift2Down();

    /**
     * Draw an overlay on the step button (depends on sequencer type)
     */
    virtual void drawStepButtonOverlay(const int index, const struct Note *note, const struct Track *track) = 0;

    /**
     * Draw the ABCD buttons for the sequencer
     */
    void drawSequencerABCDButtons(bool keyStates[SDL_NUM_SCANCODES], const struct Track *track);

    /**
     * Draw the step editor
     */
    void drawStepEditor(struct Track *track);
private:

};    

#endif
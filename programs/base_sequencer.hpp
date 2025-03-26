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

/**
 * Base Sequencer class
 * This is the base sequencer, for all sequencer-like programs
 * This includes operations such as:
 *  - 
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
private:

public:


    // /**
    //  * Constructor 
    //  */
    // BaseSequencer();

    // /** 
    //  * Destructor
    //  */
    // virtual ~BaseSequencer();

    /**
     * Update method. 
     * This happens if there is input to update the parameters (such as a keydown event)
     */ 
    // virtual void update(struct Track *selectedTrack, bool keyStates[SDL_NUM_SCANCODES], SDL_Scancode key) = 0;

    // /**
    //  * Run this program, gets triggered every clock pulse
    //  */ 
    // virtual void run(PmStream *outputStream, const uint64_t *ppqnCounter, struct Track *selectedTrack) = 0;

    // /**
    //  * Draw this program, gets triggered every step
    //  */ 
    // virtual void draw(uint64_t *ppqnCounter, bool keyStates[SDL_NUM_SCANCODES], struct Track *track) = 0;
};    

#endif
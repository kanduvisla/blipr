#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <SDL.h>
#include <stdbool.h>
#include <portmidi.h>
#include <functional>
#include "base_sequencer.hpp"
#include "../project.h"

/**
 * Sequencer class
 */
class Sequencer : public BaseSequencer {
public:
    /**
     * Constructor 
     */
    Sequencer();

    /** 
     * Destructor
     */
    virtual ~Sequencer();

    // void update(struct Track *selectedTrack, bool keyStates[SDL_NUM_SCANCODES], SDL_Scancode key, bool isDrumkitSequencer);

    /**
     * Run this program, gets triggered every clock pulse
     */ 
    // void run(PmStream *outputStream, const uint64_t *ppqnCounter, struct Track *selectedTrack) override;
    
    /**
     * Draw this program, gets triggered every step
     */ 
    // void draw(uint64_t *ppqnCounter, bool keyStates[SDL_NUM_SCANCODES], struct Track *track) override;
    // void draw(uint64_t *ppqnCounter, bool keyStates[SDL_NUM_SCANCODES], struct Track *track, bool isDrumkitSequencer);
    
    // MARK: - Sequencer specific methods:

    /**
     * Determine if a note is trigged according to it's TRIG condition
     */
    // bool isNoteTrigged(int triggValue, int repeatCount);

    /**
     * Reset template note to default values
     */
    void resetTemplateNote() override;

protected:
    /**
     * Apply Key to Note in note editor
     */
    void applyKeyToNoteInNoteEditor(struct Note *note, SDL_Scancode key) override;

    /**
     * Toggle a step
     */
    void toggleStep(struct Step *step, int noteIndex) override;

    /**
     * Draw the template note
     */
    void drawTemplateNote() override;

    /**
     * Draw an overlay on the step button (depends on sequencer type)
     */
    void drawStepButtonOverlay(const int index, const struct Note *note, const struct Track *track) override;

private:
    // void setSelectedPage(struct Track *selectedTrack, int index);
    // void handleKey(struct Track *selectedTrack, SDL_Scancode key, bool isDrumkitSequencer);
    // void sanitizeSelection(int indexPressed);
    // int getMaxPageBanks(const struct Track *track);
    // bool isNoteTrigged(int triggValue, int repeatCount);
    // void setTriggText(int triggValue, char *text);
    // bool isNotePlayed(const struct Note *note, const struct Track *track, int nudgeCheck);
    // void isFirstPulseCallback();
    // void drawPageIndicator(const struct Track *track, int playingPage);
    // void drawTemplateNote();
    // void drawSequencerMain(uint64_t *ppqnCounter, bool keyStates[SDL_NUM_SCANCODES], struct Track *selectedTrack);
    // void drawStepEditor(struct Track *track);
};

#endif
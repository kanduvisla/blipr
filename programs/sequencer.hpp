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

    /**
     * Update method. 
     * This happens if there is input to update the parameters (such as a keydown event)
     */ 
    void update(struct Track *selectedTrack, bool keyStates[SDL_NUM_SCANCODES], SDL_Scancode key) override;
    // void update(struct Track *selectedTrack, bool keyStates[SDL_NUM_SCANCODES], SDL_Scancode key, bool isDrumkitSequencer);

    /**
     * Run this program, gets triggered every clock pulse
     */ 
    void run(PmStream *outputStream, const uint64_t *ppqnCounter, struct Track *selectedTrack) override;
    
    /**
     * Draw this program, gets triggered every step
     */ 
    void draw(uint64_t *ppqnCounter, bool keyStates[SDL_NUM_SCANCODES], struct Track *track) override;
    void draw(uint64_t *ppqnCounter, bool keyStates[SDL_NUM_SCANCODES], struct Track *track, bool isDrumkitSequencer);
    
    // MARK: - Sequencer specific methods:

    /**
     * Reset template note to default values
     */
    void resetTemplateNote();

    /**
     * Determine if a note is trigged according to it's TRIG condition
     */
    // bool isNoteTrigged(int triggValue, int repeatCount);

    /**
     * Get track step index - this is the index in the steps-array on the track
     */
    int getTrackStepIndex(
        const uint64_t *ppqnCounter, 
        const struct Track *track, 
        std::function<void()> isFirstPulseCallback
    );

    /**
     * Get notes at a given track index - this takes into account the polyphony sacrifice for more steps
     */
    void getNotesAtTrackStepIndex(int trackStepIndex, const struct Track *track, const struct Note **notes);

    /**
     * Process a single pulse - keeps track of things like trigg, nudge, length, speed, shuffle, etc.
     */
    void processPulse(
        const uint64_t *currentPulse,
        const struct Track *track,
        std::function<void()> firstPulseCallback,
        std::function<void(const struct Note*)> playNoteCallback
    );

    /**
     * Prepare the template note for the drumkit sequencer
     */
    void setTemplateNoteForDrumkitSequencer(const struct Track *track, int index);

    /**
     * Toggle a step
     */
    void toggleStep(struct Step *step, int noteIndex) override;

protected:
    /**
     * Apply Key to Note in note editor
     */
    void applyKeyToNoteInNoteEditor(struct Note *note, SDL_Scancode key) override;

private:
    void setSelectedPage(struct Track *selectedTrack, int index);
    // void handleKey(struct Track *selectedTrack, SDL_Scancode key, bool isDrumkitSequencer);
    void sanitizeSelection(int indexPressed);
    int getMaxPageBanks(const struct Track *track);
    bool isNoteTrigged(int triggValue, int repeatCount);
    void setTriggText(int triggValue, char *text);
    bool isNotePlayed(const struct Note *note, const struct Track *track, int nudgeCheck);
    void isFirstPulseCallback();
    void drawPageIndicator(const struct Track *track, int playingPage);
    void drawTemplateNote();
    void drawSequencerMain(
        uint64_t *ppqnCounter, 
        bool keyStates[SDL_NUM_SCANCODES],
        struct Track *selectedTrack,
        bool isDrumkitSequencer
    );
    void drawStepEditor(struct Track *track, bool isDrumkitSequencer);
};

#endif
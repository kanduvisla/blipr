#ifndef DRUMKIT_SEQUENCER_H
#define DRUMKIT_SEQUENCER_H

#include "base_sequencer.hpp"

/**
 * Drumkit Sequencer class
 */
class DrumkitSequencer : public BaseSequencer {
public:
    /**
     * Constructor 
     */
    DrumkitSequencer();

    /** 
     * Destructor
     */
    virtual ~DrumkitSequencer();

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
     * Toggle a step
     */
    void toggleStep(struct Step *step, int noteIndex) override;

    /**
     * Handle key input when shift 2 is down
     */
    void handleKeyWithShift2Down(struct Track *track, int index) override;
};

#endif
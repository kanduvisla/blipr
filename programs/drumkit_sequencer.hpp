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
     * Reset template note to default values
     */
    void resetTemplateNote() override;

protected:
    /**
     * Is the user allowed to select notes?
     */
    bool isNoteSelectionAllowed = false;

    /**
     * Toggle a step
     */
    void toggleStep(struct Step *step, int noteIndex) override;

    /**
     * Handle key input when shift 2 is down
     */
    void handleKeyWithShift2Down(struct Track *track, int index) override;

    /**
     * Draw an overlay on the step button (depends on sequencer type)
     */
    void drawStepButtonOverlay(const int index, const struct Note *note, const struct Step* step, const struct Track *track) override;

    /**
     * Draw the main sequencer with shift 2 down (overlay)
     * @return bool to determine if the main sequencer should still be drawn or not
     */
    bool drawSequencerMainWithShift2Down() override;
};

#endif
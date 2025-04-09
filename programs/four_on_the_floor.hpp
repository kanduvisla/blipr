#ifndef FOUR_ON_THE_FLOOR_H
#define FOUR_ON_THE_FLOOR_H

#include <SDL.h>
#include <portmidi.h>
#include "abstract_program.hpp"
#include "../project.h"

/**
 * Four on the floor
 * Basic generative program
 */
class FourOnTheFloor : public Program {
public:
    /**
     * Constructor 
     */
    FourOnTheFloor();

    /**
     * Destructor 
     */
    virtual ~FourOnTheFloor();

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
     * Reset FOTF
     */
    // void reset() override;

};

#endif
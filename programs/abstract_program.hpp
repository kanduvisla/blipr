#ifndef ABSTRACT_PROGRAM_H
#define ABSTRACT_PROGRAM_H

#include <SDL.h>
#include <portmidi.h>
#include "../project.h"

class Program {
public:
    // Constructor
    Program();

    // Destructor
    virtual ~Program();

    // Update method. This happens if there is input to update the parameters (such as a keydown event)
    virtual void update(struct Track *selectedTrack, bool keyStates[SDL_NUM_SCANCODES], SDL_Scancode key) = 0;

    // Run this program, gets triggered every clock pulse
    virtual void run(PmStream *outputStream, const uint64_t *ppqnCounter, struct Track *selectedTrack) = 0;
    
    // Draw this program, gets triggered every step
    virtual void draw(uint64_t *ppqnCounter, bool keyStates[SDL_NUM_SCANCODES], struct Track *track) = 0;
};

#endif

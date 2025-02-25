#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/resource.h>
#include "globals.h"
#include "colors.h"
#include "drawing_components.h"
#include "drawing.h"
#include "drawing_text.h"
#include "drawing_icons.h"
#include "constants.h"
#include "project.h"
#include "file_handling.h"
#include "programs/sequencer.h"
#include "programs/track_selection.h"
#include "programs/pattern_selection.h"
#include "programs/sequence_selection.h"
#include "programs/config_selection.h"
#include "programs/program_selection.h"
#include "programs/track_options.h"
#include "programs/four_on_the_floor.h"
#include "midi.h"

// Renderer:
SDL_Renderer *renderer = NULL;

// The initial BPM:
double bpm = 120.0;

// Used for delay calculations:
int64_t nanoSecondsPerPulse = 0;

// Project file
char *projectFile = "data.blipr";

#define INPUT_BUFFER_SIZE 100
#define OUTPUT_BUFFER_SIZE 100

// For Midi:
PmStream *input_stream;
PmStream *outputStreamA;
PmStream *outputStreamB;
PmStream *outputStreamC;
PmStream *outputStreamD;

void calculateMicroSecondsPerPulse() {
    double beatsPerSecond = bpm / 60.0;
    double secondsPerQuarterNote = 1.0 / beatsPerSecond;
    int64_t nanoSecondsPerQuarterNote = secondsPerQuarterNote * NANOS_PER_SEC;
    nanoSecondsPerPulse = nanoSecondsPerQuarterNote / PPQN_MULTIPLIED;
}

int64_t getTimespecDiffInNanoSeconds(struct timespec *start, struct timespec *end) {
    struct timespec temp;
    if ((end->tv_nsec - start->tv_nsec) < 0) {
        temp.tv_sec = end->tv_sec - start->tv_sec - 1;
        temp.tv_nsec = 1000000000 + end->tv_nsec - start->tv_nsec;
    } else {
        temp.tv_sec = end->tv_sec - start->tv_sec;
        temp.tv_nsec = end->tv_nsec - start->tv_nsec;
    }
    return temp.tv_sec * 1000000000LL + temp.tv_nsec;
}

/**
 * Main loop
 */
int main(int argc, char *argv[]) {
    #ifdef DEBUG
    printf("--- DEBUG BUILD ---\n");
    #endif

    listMidiDevices();

    SDL_Window      *win = NULL;
    SDL_Texture     *renderTarget = NULL;

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_CreateWindowAndRenderer(720, 720, 0, &win, &renderer);

    renderTarget = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        WINDOW_WIDTH / SCALE_FACTOR,
        WINDOW_HEIGHT / SCALE_FACTOR
    );

    // Get renderer info
    SDL_RendererInfo info;
    if (SDL_GetRendererInfo(renderer, &info) == 0) {
        printf("Renderer name: %s\n", info.name);
        printf("Renderer flags: ");

        if (info.flags & SDL_RENDERER_SOFTWARE) printf("Software ");
        if (info.flags & SDL_RENDERER_ACCELERATED) printf("Accelerated ");
        if (info.flags & SDL_RENDERER_PRESENTVSYNC) printf("VSync ");
        if (info.flags & SDL_RENDERER_TARGETTEXTURE) printf("TargetTexture ");

        printf("\n");

        if (info.flags & SDL_RENDERER_ACCELERATED) {
            printf("Hardware acceleration is enabled.\n");
        } else {
            printf("Hardware acceleration is not enabled.\n");
        }
    } else {
        printf("Couldn't get renderer info! SDL_Error: %s\n", SDL_GetError());
    }

    // Flag to determine if the app should quit:
    bool quit = false;
    calculateMicroSecondsPerPulse();

    // Clock:
    struct timespec prevTime, nowTime;
    clock_gettime(CLOCK_MONOTONIC, &nowTime);
    prevTime = nowTime;

    // Event handler
    SDL_Event e;

    // Triggs:
    bool isClockResetRequired = false;
    bool isRenderRequired = false;

    // Counters:
    int ppqnCounter = 0;

    // Array to keep track of key states
    bool keyStates[SDL_NUM_SCANCODES] = {false};
    
    // State:
    // [SHIFT3]
    bool isTrackOptionsActive = false;
    bool isProgramSelectionActive = false;
    bool isPatternAndSequenceOptionsActive = false;
    bool isUtilitiesActive = false;
    // [FUNC]
    bool isConfigurationModeActive = false;
    bool isTransportSelectionActive = false;
    bool isTrackSelectionActive = false;
    bool isPatternSelectionActive = false;
    bool isSequenceSelectionActive = false;
    
    int selectedMidiInstrument = 0;
    int selectedMidiChannel = 0;
    int selectedTrack = 0;
    int selectedPattern = 0;
    int selectedSequence = 0;

    // Project file:
    printf("Loading project file: %s\n", projectFile);
    struct Project *project = readProjectFile(projectFile);
    if (project == NULL) {
        printf("No project found, creating new project\n");
        project = malloc(PROJECT_BYTE_SIZE);
        if (project == NULL) {
            printf("Memory allocation failed\n");
            return 1;  // or handle the error appropriately
        } else {
            printf("Memory allocated succesfully\n");
        }        
        initializeProject(project);
        printf("Project initialized\n");
        writeProjectFile(project, projectFile);
        printf("Project saved\n");
    } else {
        printf("Loaded project: %s\n", project->name);
    }

    // Currently active track:
    struct Track* track = &project->sequences[selectedSequence].patterns[selectedPattern].tracks[selectedTrack];

    // Setup Midi:
    int midiDeviceIdA = getOutputDeviceIdByDeviceName(project->midiDeviceAName);
    if (midiDeviceIdA != -1) {
        printf("Midi output device A: %d\n", midiDeviceIdA);
        openMidiOutput(midiDeviceIdA, &outputStreamA);
    } else {
        
    }

    int midiDeviceIdB = getOutputDeviceIdByDeviceName(project->midiDeviceBName);
    printf("midi output device B: %d\n", midiDeviceIdB);
    int midiDeviceIdC = getOutputDeviceIdByDeviceName(project->midiDeviceCName);
    printf("midi output device C: %d\n", midiDeviceIdC);
    int midiDeviceIdD = getOutputDeviceIdByDeviceName(project->midiDeviceDName);
    printf("midi output device D: %d\n", midiDeviceIdD);

    // While application is running
    while(!quit) {
        // Handle events on queue
        while(SDL_PollEvent(&e) != 0 ) {
            //User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                SDL_Scancode scanCode = e.key.keysym.scancode;
                isRenderRequired = true;
                if (!keyStates[scanCode]) {
                    keyStates[scanCode] = true;
                    // Check if this is one of the global Func-options:
                    if (keyStates[BLIPR_KEY_FUNC]) {
                        // Only enable track selection on the Main Configuration screen:
                        if (!isPatternSelectionActive && 
                            !isSequenceSelectionActive && 
                            !isTransportSelectionActive &&
                            !isConfigurationModeActive
                        ) {
                            isTrackSelectionActive = true;
                        }

                        if (isTrackSelectionActive) {
                            // If in the main configuration screen ABCD are pressed, open sub-screen:
                            if (scanCode == BLIPR_KEY_A) {
                                isTrackSelectionActive = false;
                                isPatternSelectionActive = true;
                            } else if (scanCode == BLIPR_KEY_B) {
                                isTrackSelectionActive = false;
                                isSequenceSelectionActive = true;
                            } else if (scanCode == BLIPR_KEY_D) {
                                isTrackSelectionActive = false;
                                isConfigurationModeActive = true;
                            }
                        }

                        // Actions while the Func-key is down:
                        if (isTrackSelectionActive) {
                            updateTrackSelection(&selectedTrack, scanCode);
                            track = &project->sequences[selectedSequence].patterns[selectedPattern].tracks[selectedTrack];
                        } else if (isPatternSelectionActive) {
                            updateTrackSelection(&selectedPattern, scanCode);
                        } else if (isSequenceSelectionActive) {
                            updateSequenceSelection(&selectedSequence, scanCode);
                        } else if (isConfigurationModeActive) {
                            updateConfiguration(project, scanCode);
                        }
                    } else if (keyStates[BLIPR_KEY_SHIFT_3]) {
                        // Only enable track options on the Main screen:
                        if (!isProgramSelectionActive &&
                            !isPatternAndSequenceOptionsActive &&
                            !isUtilitiesActive
                        ) { 
                            isTrackOptionsActive = true;
                        }

                        if (scanCode == BLIPR_KEY_A) {
                            isTrackOptionsActive = true;
                            isProgramSelectionActive = false;
                            isPatternAndSequenceOptionsActive = false;
                            isUtilitiesActive = false;
                        } else if (scanCode == BLIPR_KEY_B) {
                            isTrackOptionsActive = false;
                            isProgramSelectionActive = true;
                            isPatternAndSequenceOptionsActive = false;
                            isUtilitiesActive = false;
                        } else if (scanCode == BLIPR_KEY_C) {
                            isTrackOptionsActive = false;
                            isProgramSelectionActive = false;
                            isPatternAndSequenceOptionsActive = true;
                            isUtilitiesActive = false;
                        } else if (scanCode == BLIPR_KEY_D) {
                            isTrackOptionsActive = false;
                            isProgramSelectionActive = false;
                            isPatternAndSequenceOptionsActive = false;
                            isUtilitiesActive = true;
                        }

                        if (isTrackOptionsActive) {
                            updateTrackOptions(track, scanCode);
                        } else if (isProgramSelectionActive) {
                            updateProgram(track, scanCode);
                        }
                    } else {
                        // Func-key is not down, so program of current track should be shown:
                        switch (track->program) {
                            case BLIPR_PROGRAM_SEQUENCER:
                                updateSequencer(project, keyStates, scanCode, selectedSequence, selectedPattern, selectedTrack);
                                break;
                        }
                    }

                    #ifdef DEBUG
                    printf("Key pressed: %s\n", SDL_GetKeyName(e.key.keysym.sym));
                    #endif

                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        quit = true;
                    }
                }
            } else if (e.type == SDL_KEYUP) {
                SDL_Scancode scanCode = e.key.keysym.scancode;
                isRenderRequired = true;
                keyStates[scanCode] = false;
                // Func-keyup always closes the entire program selection & func-menu
                if (scanCode == BLIPR_KEY_FUNC || scanCode == BLIPR_KEY_SHIFT_3) {
                    isTrackSelectionActive = false;
                    isPatternSelectionActive = false;
                    isSequenceSelectionActive = false;
                    isConfigurationModeActive = false;
                    isProgramSelectionActive = false;
                    isTrackOptionsActive = false;
                    isPatternAndSequenceOptionsActive = false;
                    isUtilitiesActive = false;
                    resetConfigurationScreen();
                }

                #ifdef DEBUG
                printf("Key released: %s (%d)\n", SDL_GetKeyName(e.key.keysym.sym), scanCode);
                #endif
            }
        }

        // Calculate with monotonic clock:
        clock_gettime(CLOCK_MONOTONIC, &nowTime);
        int64_t elapsedNs = getTimespecDiffInNanoSeconds(&prevTime, &nowTime);

        // Check if ppqn is trigged:
        if (elapsedNs > nanoSecondsPerPulse) {
            // Send Midi Clock:
            if (midiDeviceIdA != -1) {
                // Calculate back using the multiplier, otherwise the clock is too fast:
                if (ppqnCounter % PPQN_MULTIPLIER == 0) {
                    sendMidiClock(outputStreamA);
                } 
            }

            // Iterate over all tracks, and send proper midi signals
            for (int i=0; i<16; i++) {
                struct Track* iTrack = &project->sequences[selectedSequence].patterns[selectedPattern].tracks[i];
                // Run the program:
                switch (iTrack->program) {
                    case BLIPR_PROGRAM_SEQUENCER:
                        runSequencer(project, &ppqnCounter, selectedSequence, selectedPattern, iTrack);
                        break;
                    case BLIPR_PROGRAM_FOUR_ON_THE_FLOOR:
                        // Todo: select proper stream:
                        // printf("memory address of track: %d\n", &iTrack);
                        runFourOnTheFloor(outputStreamA, &ppqnCounter, iTrack);
                        break;
                }
            }

            isClockResetRequired = true;
            ppqnCounter += 1;
            if (ppqnCounter % PPQN == 0) {
                if (ppqnCounter >= MAX_PULSES) {
                    ppqnCounter = 0;
                }
                isRenderRequired = true;
            }
        }

        // Drawing magic:
        if (isRenderRequired) {
            // Set the render target to our texture
            SDL_SetRenderTarget(renderer, renderTarget);
            // Clear the render target
            SDL_SetRenderDrawColor(renderer, COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b, 255);
            SDL_RenderClear(renderer);

            // BPM Blinker:
            drawBPMBlinker(&ppqnCounter);

            // Sequence, pattern and track number (replace with real numbers):
            drawText(WIDTH - 54, 4, "S00P00T00", 60, COLOR_GRAY);

            // Basic Grid:
            drawBasicGrid(keyStates);

            // Configuration mode:
            if (isTrackSelectionActive) {
                drawTrackSelection(&selectedTrack);
            } else if (isPatternSelectionActive) {
                drawPatternSelection(&selectedPattern);
            } else if (isSequenceSelectionActive) {
                drawSequenceSelection(&selectedPattern);
            } else if (isConfigurationModeActive) {
                drawConfigSelection(project);
            } else if (isTrackOptionsActive) {
                drawTrackOptions(track);
            } else if (isProgramSelectionActive) {
                drawProgramSelection(track);
            } else if (isPatternAndSequenceOptionsActive) {
                drawCenteredLine(2, 61, "(PAT&SEQ OPTIONS)", TITLE_WIDTH, COLOR_WHITE);      
            } else if (isUtilitiesActive) {
                drawCenteredLine(2, 61, "(UTILITIES)", TITLE_WIDTH, COLOR_WHITE);      
            } else {
                // Draw the program associated with this Track:
                switch (track->program) {
                    case BLIPR_PROGRAM_NONE:
                        drawCenteredLine(2, 61, "(NO PROGRAM)", TITLE_WIDTH, COLOR_WHITE);      
                        break;
                    case BLIPR_PROGRAM_SEQUENCER:
                        drawSequencer(&ppqnCounter, track);
                        break;
                    case BLIPR_PROGRAM_FOUR_ON_THE_FLOOR:
                        drawFourOnTheFloor(&ppqnCounter, track);
                        break;
                }
            }

            // Clear the renderer:
            SDL_SetRenderTarget(renderer, NULL);

            // Draw the scaled-up texture to the screen
            SDL_Rect destRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderCopy(renderer, renderTarget, NULL, &destRect);

            // Render:
            SDL_RenderPresent(renderer);
            isRenderRequired = false;
        }

        // Re-calculate clock (if required):
        if (isClockResetRequired) {
            // Reset clock:
            isClockResetRequired = false;
            clock_gettime(CLOCK_MONOTONIC, &prevTime);
        }
    }

    SDL_DestroyTexture(renderTarget);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    writeProjectFile(project, projectFile);
    free(project);

    Pm_Close(outputStreamA);
    Pm_Close(outputStreamB);
    Pm_Close(outputStreamC);
    Pm_Close(outputStreamD);
    Pm_Terminate();

    return (0);
}

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

PmStream *input_stream;
PmStream *output_stream;

void calculateMicroSecondsPerPulse() {
    double beatsPerSecond = bpm / 60.0;
    double secondsPerBeat = 1.0 / beatsPerSecond;
    int64_t nanoSecondsPerBeat = secondsPerBeat * NANOS_PER_SEC;
    int64_t nanoSecondsPerNote = nanoSecondsPerBeat / LINES_PER_BAR;
    nanoSecondsPerPulse = nanoSecondsPerNote / PPQN;
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
    /* DEBUG INFO */
    printf("note byte size: %d / %d\n", sizeof(struct Note), NOTE_BYTE_SIZE);
    printf("step byte size: %d / %d\n", sizeof(struct Step), STEP_BYTE_SIZE);
    printf("track byte size: %d / %d\n", sizeof(struct Track), TRACK_BYTE_SIZE);
    printf("pattern byte size: %d / %d\n", sizeof(struct Pattern), PATTERN_BYTE_SIZE);
    printf("sequence byte size: %d / %d\n", sizeof(struct Sequence), SEQUENCE_BYTE_SIZE);
    printf("project byte size: %d / %d\n", sizeof(struct Project), PROJECT_BYTE_SIZE);
    
    struct rlimit rlim;
    getrlimit(RLIMIT_STACK, &rlim);
    printf("stack size: %llu\n", rlim.rlim_cur);

    /* END DEBUG INFO */

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
    bool isPpqnTrigged = false;
    bool isNoteTrigged = false;
    bool isBeatTrigged = false;
    bool isClockResetRequired = false;
    bool isRenderRequired = false;

    // Counters:
    int ppqnCounter = -1;
    int noteCounter = -1;
    int pageCounter = 0;

    // Array to keep track of key states
    bool keyStates[SDL_NUM_SCANCODES] = {false};
    
    // State:
    int selectedProgram = BLIPR_PROGRAM_SEQUENCER;
    bool isConfigurationModeActive = false;
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
    // exit(1);
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
                    
                    if (scanCode == BLIPR_KEY_FUNC) {
                        // Shift 3 also doubles as back button Back button:
                        // isConfigurationModeActive = false;
                        // Display track selection:
                        isTrackSelectionActive = true;
                    }

                    // Check if this is one of the global Func-options:
                    if (keyStates[BLIPR_KEY_FUNC]) {
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

                        // Actions while the Func-key is down:
                        if (isTrackSelectionActive) {
                            updateTrackSelection(&selectedTrack, scanCode);
                        } else if (isPatternSelectionActive) {
                            updateTrackSelection(&selectedPattern, scanCode);
                        } else if (isSequenceSelectionActive) {
                            updateSequenceSelection(&selectedSequence, scanCode);
                        }    
                    } else {
                        // Func-key is not down, so current program should be shown:
                        if (selectedProgram == BLIPR_PROGRAM_SEQUENCER) {
                            // Update the sequencer:
                            updateSequencer(project, keyStates, scanCode, selectedSequence, selectedPattern, selectedTrack);
                        }
                    }

                    printf("Key pressed: %s\n", SDL_GetKeyName(e.key.keysym.sym));
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        quit = true;
                    }
                }
            } else if (e.type == SDL_KEYUP) {
                SDL_Scancode scanCode = e.key.keysym.scancode;
                isRenderRequired = true;
                keyStates[scanCode] = false;
                // Func-keyup always closes the entire func-menu
                if (scanCode == BLIPR_KEY_FUNC) {
                    isTrackSelectionActive = false;
                    isPatternSelectionActive = false;
                    isSequenceSelectionActive = false;
                    isConfigurationModeActive = false;
                }
                printf("Key released: %s\n", SDL_GetKeyName(e.key.keysym.sym));
            }
        }

        // Calculate with monotonic clock:
        clock_gettime(CLOCK_MONOTONIC, &nowTime);
        int64_t elapsedNs = getTimespecDiffInNanoSeconds(&prevTime, &nowTime);

        if (elapsedNs > nanoSecondsPerPulse) {
            isPpqnTrigged = true;
        }

        // Check if ppqn is trigged:
        if (isPpqnTrigged) {
            // Do stuff on PPQN level

            // Run sequencer:
            // runSequencer(project, &ppqnCounter, selectedSequence, selectedPattern);

            // End
            isPpqnTrigged = false;
            isClockResetRequired = true;
            // isRenderRequired = true;
            ppqnCounter += 1;
            if (ppqnCounter % PPQN == 0) {
                ppqnCounter = 0;
                isNoteTrigged = true;
            }
        }

        // Check if note change is trigged:
        if (isNoteTrigged) {
            // Do stuff on Note level

            // End
            isNoteTrigged = false;
            isRenderRequired = true;
            noteCounter += 1;
            if (noteCounter % LINES_PER_BAR == 0) {
                isBeatTrigged = true;
                isNoteTrigged = false;
                if (noteCounter == PATTERN_LENGTH) {
                    noteCounter = 0;
                    pageCounter += 1;
                    if (pageCounter == 4) {
                        pageCounter = 0;
                    }
                }
            }
        }

        // Check if Bar (set of lines, the "B" in BPM) is trigged:
        if (isBeatTrigged) {
            // Do stuff on Beat level

            // End
            isBeatTrigged = false;
        }

        // Drawing magic:
        if (isRenderRequired) {
            // Set the render target to our texture
            SDL_SetRenderTarget(renderer, renderTarget);
            // Clear the render target
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // BPM Blinker:
            drawBPMBlinker(noteCounter, ppqnCounter);

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
                // TODO: Draw configuration
            } else {
                // Currently active program:
                if (selectedProgram == BLIPR_PROGRAM_SEQUENCER) {
                    // Draw the sequencer:
                    drawSequencer(project, &noteCounter, selectedSequence, selectedPattern, selectedTrack);
                }
            }

            // Test:
            // drawIcon(9, 9, BLIPR_ICON_CONFIG, COLOR_WHITE);

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

    return (0);
}

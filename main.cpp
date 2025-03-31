#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <unistd.h>
#include "globals.h"
#include "colors.h"
#include "drawing_components.h"
#include "drawing.h"
#include "drawing_text.h"
#include "drawing_icons.h"
#include "constants.h"
#include "project.h"
#include "file_handling.h"
#include "programs/sequencer.hpp"
#include "programs/track_selection.h"
#include "programs/pattern_selection.h"
#include "programs/sequence_selection.h"
#include "programs/config_selection.h"
#include "programs/program_selection.h"
#include "programs/track_options.h"
#include "programs/four_on_the_floor.hpp"
#include "programs/pattern_options.h"
#include "midi.h"
#include "print.h"

// Renderer:
SDL_Renderer *renderer = NULL;

// For debugging & monitoring:
bool isMidiDataLogged = false;
bool isTimeMeasured = false;

struct timespec seqStartTime, seqEndTime;   // To monitor sequencer performance
struct timespec renStartTime, renEndTime;   // To monitor renderer performance

// Project file
const char *projectFile = "data.blipr";

#define INPUT_BUFFER_SIZE 100
#define OUTPUT_BUFFER_SIZE 100

// Programs:
FourOnTheFloor progFourOnTheFloor;
Sequencer progSequencer;

/**
 * Calculate nano seconds per pulse for a given BPM
 */
uint64_t calculateNanoSecondsPerPulse(int bpm) {
    printLog("Calculating ns per pulse for %d BPM", bpm);
    double beatsPerSecond = bpm / 60.0;
    double secondsPerQuarterNote = 1.0 / beatsPerSecond;
    uint64_t nanoSecondsPerQuarterNote = secondsPerQuarterNote * NANOS_PER_SEC;
    uint64_t ns = nanoSecondsPerQuarterNote / PPQN_MULTIPLIED;
    printLog("ns per pulse: %d", ns);
    return ns;
}

/**
 * Get the time difference between 2 times
 */
uint64_t getTimespecDiffInNanoSeconds(struct timespec *start, struct timespec *end) {
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

uint64_t timespecToNs(struct timespec *ts) {
    return (uint64_t)ts->tv_sec * 1000000000LL + (uint64_t)ts->tv_nsec;
}

/**
 * Check for flags
 */
bool checkFlag(int argc, char *argv[], const char *flag) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], flag) == 0) {
            return true;
        }
    }
    return false;
}

// Shared data structure between threads
typedef struct {
    struct Project *project;
    struct Track* track;
    int unprocessedPulses;              // Pulses are count with unprocessed pulses in the clock thread.
    uint64_t ppqnCounter;               // The ppqn counter is kept in the sequencer track in conjunction with the unprocessedPulses counter. This way skipped pulses can be caught.
    bool isRenderRequired;
    bool keyStates[SDL_NUM_SCANCODES];
    
    bool isSetupMidiDevicesRequired;    // Boolean flag to determine if midi devices needs to be set-up (required after changing midi assignment)
    
    int programA;                       // Midi programs. Is 255 if no PC is required
    int programB;
    int programC;
    int programD;    
    int prevProgramA;                   // Previous Midi programs. Is used to detect changes
    int prevProgramB;
    int prevProgramC;
    int prevProgramD;    

    BliprScreen screen;
    bool quit;
    int bpm;    // shortcut to BPM, only used for displaying
    uint64_t nanoSecondsPerPulse;
    SDL_Scancode scanCodeKeyDown;
    SDL_Scancode scanCodeKeyUp;

    int selectedTrack;
    int selectedPattern;
    int queuedPattern;
    uint64_t patternStepCounter;       // Is in steps
    int selectedSequence;

    // For monitoring:
    double seqPerformance;
    double renPerformance;

    // Synchronization primitives
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} SharedState;

/**
 * Set the screen according to the current active track program
 */
void setScreenAccordingToActiveTrack(SharedState *state) {
    switch (state->track->program) {
        case BLIPR_PROGRAM_SEQUENCER:
            state->screen = BLIPR_SCREEN_SEQUENCER;
            break;
        case BLIPR_PROGRAM_DRUMKIT_SEQUENCER:
            state->screen = BLIPR_SCREEN_DRUMKIT_SEQUENCER;
            break;
        case BLIPR_PROGRAM_FOUR_ON_THE_FLOOR:
            state->screen = BLIPR_SCREEN_FOUR_ON_THE_FLOOR;
            break;
        default:
            state->screen = BLIPR_SCREEN_NO_PROGRAM;
            break;
    }
}

/**
 * Initialize shared state
 */
void initSharedState(SharedState* state) {
    state->unprocessedPulses = 0;
    state->ppqnCounter = 0;

    state->isRenderRequired = false;
    for (int i=0; i<SDL_NUM_SCANCODES; i++) {
        state->keyStates[i] = false;
    }
    state->isSetupMidiDevicesRequired = true;
    state->programA = 255;
    state->programB = 255;
    state->programC = 255;
    state->programD = 255;
    state->selectedTrack = 0;
    state->selectedPattern = 0;
    state->queuedPattern = 0;
    state->patternStepCounter = 0;
    state->selectedSequence = 0;
    state->quit = false;
    state->bpm = 0;
    state->scanCodeKeyDown = SDL_SCANCODE_UNKNOWN;
    state->scanCodeKeyUp = SDL_SCANCODE_UNKNOWN;
    state->seqPerformance = 0.0;
    state->renPerformance = 0.0;

    // Project file:
    print("Loading project file: %s", projectFile);
    state->project = readProjectFile(projectFile);
    if (state->project == NULL) {
        print("No project found, creating new project");
        state->project = (struct Project*)malloc(PROJECT_BYTE_SIZE);
        if (state->project == NULL) {
            printError("Memory allocation failed");
        } else {
            printLog("Memory allocated succesfully");
        }
        initializeProject(state->project);
        printLog("Project initialized");
        writeProjectFile(state->project, projectFile);
        printLog("Project saved");
    } else {
        printLog("Loaded project: %s", state->project->name);
    }
    
    // Get the BPM from the current pattern:
    state->bpm = state->project->sequences[0].patterns[0].bpm + 45;
    state->nanoSecondsPerPulse = calculateNanoSecondsPerPulse(state->bpm);
    state->track = &state->project->sequences[0].patterns[0].tracks[0];
    setScreenAccordingToActiveTrack(state);

    // Send proper PC to start with:
    state->programA = state->project->sequences[0].patterns[0].programA;
    state->programB = state->project->sequences[0].patterns[0].programB;
    state->programC = state->project->sequences[0].patterns[0].programC;
    state->programD = state->project->sequences[0].patterns[0].programD;

    pthread_mutex_init(&state->mutex, NULL);
    pthread_cond_init(&state->cond, NULL);
}

// Clean up shared state
void cleanupSharedState(SharedState* state) {
    pthread_mutex_destroy(&state->mutex);
    pthread_cond_destroy(&state->cond);
}

/**
 * Thread dedicated to timing
 */
void* timerThread(void *arg) {
    SharedState* state = (SharedState*)arg;

    // Timing for sequencer:
    struct timespec nowTime;
    uint64_t modulo, prevModulo = 0;
    clock_gettime(CLOCK_MONOTONIC, &nowTime);

    while (!state->quit) {
        // Calculate with monotonic clock:
        clock_gettime(CLOCK_MONOTONIC, &nowTime);
        modulo = timespecToNs(&nowTime) % state->nanoSecondsPerPulse;

        // if the module is smaller, we now we've passed a new pulse:
        if (modulo < prevModulo) {
            pthread_mutex_lock(&state->mutex);
            state->unprocessedPulses += 1;
            pthread_mutex_unlock(&state->mutex);
        }

        prevModulo = modulo;
    }
    
    return NULL;
}

/**
 * Sequencer thread
 */
void* sequencerThread(void* arg) {
    SharedState* state = (SharedState*)arg;

    // Setup Midi:
    PmStream *outputStream[4]; // 4 streams, for A, B, C and D
    int midiDevice[4];          // 4 midi devices, for A, B, C and D

    progSequencer.resetTemplateNote();
    //resetTemplateNote();

    // Sequencer loop:
    while (!state->quit) {
        if (state->isSetupMidiDevicesRequired) {
            // Setup Midi devices:
            for (int i=0; i<4; i++) {
                char* midiDeviceName;
                switch (i) {
                    case 0:
                        midiDeviceName = state->project->midiDeviceAName;
                        break;
                    case 1:
                        midiDeviceName = state->project->midiDeviceBName;
                        break;
                    case 2:
                        midiDeviceName = state->project->midiDeviceCName;
                        break;
                    case 3:
                        midiDeviceName = state->project->midiDeviceDName;
                        break;
                }

                if (strcmp(midiDeviceName, "") == 0) {
                    printLog("No midi output device set for slot %d", i);
                    continue;
                }

                midiDevice[i] = getOutputDeviceIdByDeviceName(midiDeviceName);
                if (midiDevice[i] != -1) {
                    printLog("Configured midi output device: %d", midiDevice[i]);
                    openMidiOutput(midiDevice[i], &outputStream[i]);
                    if (outputStream[i] == NULL) {
                        printError("Unable to open output stream for this device");
                    }
                } else {
                    printError("Midi device not found: %s", midiDeviceName);
                }
            }

            pthread_mutex_lock(&state->mutex);
            state->isSetupMidiDevicesRequired = false;
            pthread_mutex_unlock(&state->mutex);
        }

        if (state->programA != 255) {
            printLog("change program A to %d", state->programA);
            sendProgramChange(outputStream[0], state->project->midiDevicePcChannelA, state->programA);
            pthread_mutex_lock(&state->mutex);
            state->prevProgramA = state->programA;
            state->programA = 255;
            pthread_mutex_unlock(&state->mutex);
        } else if (state->programB != 255) {
            printLog("change program B to %d", state->programB);
            sendProgramChange(outputStream[1], state->project->midiDevicePcChannelB, state->programB);
            pthread_mutex_lock(&state->mutex);
            state->prevProgramB = state->programB;
            state->programB = 255;
            pthread_mutex_unlock(&state->mutex);
        } else if (state->programC != 255) {
            printLog("change program C to %d", state->programC);
            sendProgramChange(outputStream[2], state->project->midiDevicePcChannelC, state->programC);
            pthread_mutex_lock(&state->mutex);
            state->prevProgramC = state->programC;
            state->programC = 255;
            pthread_mutex_unlock(&state->mutex);
        } else if (state->programD != 255) {
            printLog("change program to %d", state->programD);
            sendProgramChange(outputStream[3], state->project->midiDevicePcChannelD, state->programD);
            pthread_mutex_lock(&state->mutex);
            state->prevProgramD = state->programD;
            state->programD = 255;
            pthread_mutex_unlock(&state->mutex);
        }

        if (state->unprocessedPulses > 0) {
            // Do the work!
            if (isTimeMeasured) {
                clock_gettime(CLOCK_MONOTONIC, &seqStartTime);
            }

            pthread_mutex_lock(&state->mutex);
            state->ppqnCounter += state->unprocessedPulses;
            int unprocessedPulses = state->unprocessedPulses;
            state->unprocessedPulses = 0;
            pthread_mutex_unlock(&state->mutex);

            // Send Midi Clock:
            for (int i=0; i<4; i++) { 
                if (outputStream[i] != NULL) {
                    // Calculate back using the multiplier, otherwise the clock is too fast
                    // If ppqnCounter = 24, and 24 % 24 = 0, then the midi clock will be sent.
                    // But if step 24 was skipped, and the inprocessedPulses was +2, then it could be that ppqnCounter % 24 == 1 and still a midi clock tick is required.
                    // So it should not be checked with 0, but with unprocessed pulses - 1, so if unprocessed pulses is 3 for example, the modulo check is (3-1) = 2.
                    if (state->ppqnCounter % PPQN_MULTIPLIER == unprocessedPulses - 1) {
                        sendMidiClock(outputStream[i]);
                    } 
                }
            }

            // Increase pattern steps:
            if (state->ppqnCounter % PP16N == 0) {
                state->patternStepCounter++;
                int length = (state->project->sequences[state->selectedSequence].patterns[state->selectedPattern].length + 1);
                if (state->patternStepCounter % length == 0) {
                    state->patternStepCounter = 0;
                    // This is the moment to switch from the queued pattern to the selected pattern
                    if (state->selectedPattern != state->queuedPattern) {
                        // Perform actions when switching pattern:
                        pthread_mutex_lock(&state->mutex);
                        state->selectedPattern = state->queuedPattern;
                        // Set proper track + reset repeat count for all track:
                        state->track = &state->project->sequences[state->selectedSequence]
                            .patterns[state->selectedPattern]
                            .tracks[state->selectedTrack];
                        for (int i=0; i<16; i++) {
                            state->project->sequences[state->selectedSequence].patterns[state->selectedPattern].tracks[i].repeatCount = 0;
                        }
                        // Trigger program change:
                        const struct Pattern *pattern = &state->project->sequences[state->selectedSequence].patterns[state->selectedPattern];
                        if (pattern->programA != state->prevProgramA) {
                            state->programA = pattern->programA;
                        }
                        if (pattern->programB != state->prevProgramB) {
                            state->programB = pattern->programB;
                        }
                        if (pattern->programC != state->prevProgramC) {
                            state->programC = pattern->programC;
                        }
                        if (pattern->programD != state->prevProgramD) {
                            state->programD = pattern->programD;
                        }
                        // Set proper BPM:
                        state->bpm = state->project->sequences[state->selectedSequence]
                            .patterns[state->selectedPattern].bpm + 45;
                        state->nanoSecondsPerPulse = calculateNanoSecondsPerPulse(state->bpm);
                        // Set proper screen:
                        setScreenAccordingToActiveTrack(state);
                        if (state->screen == BLIPR_SCREEN_DRUMKIT_SEQUENCER) {
                            // @TODO: Do this for drumkit sequencer prog, not sequencer prog
                            progSequencer.resetTemplateNote();
                        }
                        pthread_mutex_unlock(&state->mutex);
                    }                    
                }
            }

            // Iterate over all tracks, and send proper midi signals
            for (int i=0; i<16; i++) {
                struct Track* iTrack = &state->project->sequences[state->selectedSequence].patterns[state->selectedPattern].tracks[i];
                // bool isTrackKeyRepeatTriggered = false;
                
                // Run the program:
                switch (iTrack->program) {
                    case BLIPR_PROGRAM_SEQUENCER:
                    case BLIPR_PROGRAM_DRUMKIT_SEQUENCER:
                        progSequencer.run(outputStream[iTrack->midiDevice], &state->ppqnCounter, iTrack);
                        break;
                    case BLIPR_PROGRAM_FOUR_ON_THE_FLOOR:
                        progFourOnTheFloor.run(outputStream[iTrack->midiDevice], &state->ppqnCounter, iTrack);
                        break;
                }
            }

            // Decrease note-off counters:
            updateNotesAndSendOffs();

            // Check for render trigger (typically every step):
            if (state->ppqnCounter % PP16N == 0) {
                pthread_mutex_lock(&state->mutex);
                state->isRenderRequired = true;
                pthread_mutex_unlock(&state->mutex);
            }

            if (isTimeMeasured) {
                clock_gettime(CLOCK_MONOTONIC, &seqEndTime);
                int64_t elapsedNs = getTimespecDiffInNanoSeconds(&seqStartTime, &seqEndTime);
                double percentage = ((double)elapsedNs / state->nanoSecondsPerPulse) * 100.0;
                pthread_mutex_lock(&state->mutex);
                state->seqPerformance = percentage;
                pthread_mutex_unlock(&state->mutex);
                print("Sequencer took %dns to run (%.2f%%)", elapsedNs, percentage);
            }
        }
    }

    for (int i=0; i<4; i++) {
        Pm_Close(outputStream[i]);
    }

    Pm_Terminate();

    return NULL;
}

/**
 * Thread for keyboard input
 */
void* keyThread(void* arg) {
    SharedState* state = (SharedState*)arg;

    // Keydown logic:
    while (!state->quit) {
        // Perform key down actions:
        if(state->scanCodeKeyDown != SDL_SCANCODE_UNKNOWN) {
            // User requests quit
            if (state->scanCodeKeyDown == SDL_SCANCODE_ESCAPE) {
                pthread_mutex_lock(&state->mutex);
                state->quit = true;
                pthread_mutex_unlock(&state->mutex);
            }

            if (!state->keyStates[state->scanCodeKeyDown]) {
                pthread_mutex_lock(&state->mutex);
                state->keyStates[state->scanCodeKeyDown] = true;
                pthread_mutex_unlock(&state->mutex);
            }

            // Check if this is one of the global Func-options:
            if (state->keyStates[BLIPR_KEY_FUNC]) {
                // Fn-A = Pattern selector
                // Fn-B = Sequence selector
                // Fn-C = Configuration
                // Fn-D = Transport
                pthread_mutex_lock(&state->mutex);
                if (state->scanCodeKeyDown == BLIPR_KEY_FUNC || state->scanCodeKeyDown == BLIPR_KEY_A) {
                    state->screen = BLIPR_SCREEN_PATTERN_SELECTION;
                } else if (state->scanCodeKeyDown == BLIPR_KEY_B) {
                    state->screen = BLIPR_SCREEN_SEQUENCE_SELECTION;
                } else if (state->scanCodeKeyDown == BLIPR_KEY_C) {
                    state->screen = BLIPR_SCREEN_CONFIGURATION;
                } else if (state->scanCodeKeyDown == BLIPR_KEY_D) {
                    state->screen = BLIPR_SCREEN_TRANSPORT;
                }
                
                if (state->screen == BLIPR_SCREEN_PATTERN_SELECTION) {
                    updatePatternSelection(&state->queuedPattern, state->scanCodeKeyDown);
                } else if (state->screen == BLIPR_SCREEN_SEQUENCE_SELECTION) {
                    updateSequenceSelection(&state->selectedSequence, state->scanCodeKeyDown);
                    // Set proper pattern, track + reset repeat count
                    state->track = &state->project->sequences[state->selectedSequence]
                        .patterns[state->selectedPattern]
                        .tracks[state->selectedTrack];
                    state->track->repeatCount = 0;
                } else if (state->screen == BLIPR_SCREEN_CONFIGURATION) {
                    bool reloadMidi = false;
                    bool quit = false;
                    updateConfiguration(state->project, state->scanCodeKeyDown, &reloadMidi, &quit);
                    if (reloadMidi) { state->isSetupMidiDevicesRequired = true; }
                    if (quit) { state->quit = true; }
                } else if (state->screen == BLIPR_SCREEN_TRANSPORT) {
                    // TODO
                }
                pthread_mutex_unlock(&state->mutex);
            } else if (state->keyStates[BLIPR_KEY_SHIFT_3]) {
                // Shift 3 is Track & Pattern options
                // ^3-A = Track Selector
                // ^3-B = Track Options (1)
                // ^3-C = Program Selector
                // ^3-D = Pattern Options
                pthread_mutex_lock(&state->mutex);
                if (state->scanCodeKeyDown == BLIPR_KEY_SHIFT_3 || state->scanCodeKeyDown == BLIPR_KEY_A) {
                    state->screen = BLIPR_SCREEN_TRACK_SELECTION;
                } else if (state->scanCodeKeyDown == BLIPR_KEY_B) {
                    state->screen = BLIPR_SCREEN_TRACK_OPTIONS;
                } else if (state->scanCodeKeyDown == BLIPR_KEY_C) {
                    state->screen = BLIPR_SCREEN_PROGRAM_SELECTION;
                } else if (state->scanCodeKeyDown == BLIPR_KEY_D) {
                    state->screen = BLIPR_SCREEN_PATTERN_OPTIONS;
                }

                if (state->screen == BLIPR_SCREEN_TRACK_OPTIONS) {
                    updateTrackOptions(state->track, state->scanCodeKeyDown);
                } else if (state->screen == BLIPR_SCREEN_PROGRAM_SELECTION) {
                    updateProgram(state->track, state->scanCodeKeyDown);
                } else if (state->screen == BLIPR_SCREEN_TRACK_SELECTION) {
                    updateTrackSelection(&state->selectedTrack, state->scanCodeKeyDown);
                    state->track = &state->project->sequences[state->selectedSequence]
                        .patterns[state->selectedPattern]
                        .tracks[state->selectedTrack];
                    // Set selected note to 0:
                    progSequencer.resetSelectedNote();
                } else if (state->screen == BLIPR_SCREEN_PATTERN_OPTIONS) {
                    struct Sequence *sequence = &state->project->sequences[state->selectedSequence];
                    struct Pattern *pattern = &sequence->patterns[state->selectedPattern];
                    
                    int startBPM = pattern->bpm;
                    int startProgA = pattern->programA;
                    int startProgB = pattern->programB;
                    int startProgC = pattern->programC;
                    int startProgD = pattern->programD;

                    updatePatternOptions(
                        pattern, 
                        state->scanCodeKeyDown
                    );

                    if (startBPM != pattern->bpm) {
                        state->bpm = pattern->bpm + 45;
                        state->nanoSecondsPerPulse = calculateNanoSecondsPerPulse(state->bpm);
                    }

                    if (startProgA != pattern->programA) {
                        state->programA = pattern->programA;
                    }

                    if (startProgB != pattern->programB) {
                        state->programB = pattern->programB;
                    }

                    if (startProgC != pattern->programC) {
                        state->programC = pattern->programC;
                    }

                    if (startProgD != pattern->programD) {
                        state->programD = pattern->programD;
                    }
                }
                pthread_mutex_unlock(&state->mutex);
            } else {
                // No Fn or ^3 active, so handle the program of the current track:
                pthread_mutex_lock(&state->mutex);
                setScreenAccordingToActiveTrack(state);
                switch (state->track->program) {
                    case BLIPR_PROGRAM_SEQUENCER:
                    case BLIPR_PROGRAM_DRUMKIT_SEQUENCER:
                        // TODO: Set proper prog for drumkit sequencer
                        progSequencer.update(
                            state->track, 
                            state->keyStates, 
                            state->scanCodeKeyDown
                        );                        
                        break;
                }
                // Handle key:
                pthread_mutex_unlock(&state->mutex);
            }

            // Reset flag:
            pthread_mutex_lock(&state->mutex);
            state->scanCodeKeyDown = SDL_SCANCODE_UNKNOWN;
            state->isRenderRequired = true;
            pthread_mutex_unlock(&state->mutex);
        }

        // Perform key down actions:
        if(state->scanCodeKeyUp != SDL_SCANCODE_UNKNOWN) {
            // Func-keyup always closes the entire program selection & func-menu
            if (state->scanCodeKeyUp == BLIPR_KEY_FUNC || state->scanCodeKeyUp == BLIPR_KEY_SHIFT_3) {
                pthread_mutex_lock(&state->mutex);
                // Set screen to current running program:
                setScreenAccordingToActiveTrack(state);                
                pthread_mutex_unlock(&state->mutex);
                resetConfigurationScreen();
            } else if (state->scanCodeKeyUp == BLIPR_KEY_SHIFT_1) {
                progSequencer.resetSelectedSteps();
            }
            
            // Reset flag:
            pthread_mutex_lock(&state->mutex);
            state->keyStates[state->scanCodeKeyUp] = false;
            state->scanCodeKeyUp = SDL_SCANCODE_UNKNOWN;
            state->isRenderRequired = true;
            pthread_mutex_unlock(&state->mutex);
        }
    }

    return NULL;
}

/**
 * Main loop
 */
int main(int argc, char *argv[]) {
    bool isScreenRotated = checkFlag(argc, argv, "--rotate180");
    isMidiDataLogged = checkFlag(argc, argv, "--logMidiData");
    isTimeMeasured = checkFlag(argc, argv, "--measureTime");

    if (checkFlag(argc, argv, "--help") == true) {
        // Print Help:
        printf("Optional arguments:\n\n");
        printf("  --rotate180       Rotate the screen 180 degrees\n");
        printf("  --logMidiData     Log the MIDI data to the terminal\n");
        printf("  --measureTime     Measure time for actions");
    }

    printLog("Screen rotated: %s", isScreenRotated ? "true" : "false");

    #ifdef DEBUG
    printWarning("Running in debug mode");
    #endif

    listMidiDevices();

    SDL_Window      *win = NULL;
    SDL_Texture     *renderTarget = NULL;

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_CreateWindowAndRenderer(720, 720, 0, &win, &renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

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
        printLog("Renderer: %s", info.name);

        if (checkFlag(argc, argv, "--displayRendererFlags")) {
            printf("Renderer flags: ");
            if (info.flags & SDL_RENDERER_SOFTWARE) printf("Software ");
            if (info.flags & SDL_RENDERER_ACCELERATED) printf("Accelerated ");
            if (info.flags & SDL_RENDERER_PRESENTVSYNC) printf("VSync ");
            if (info.flags & SDL_RENDERER_TARGETTEXTURE) printf("TargetTexture ");
            printf("\n");
        }

        if (info.flags & SDL_RENDERER_ACCELERATED) {
            printLog("Hardware acceleration is enabled.");
        } else {
            printLog("Hardware acceleration is not enabled.");
        }
    } else {
        printError("Couldn't get renderer info! SDL_Error: %s", SDL_GetError());
    }

    initializeTextures();

    // Multi threading :-)
    SharedState state;
    initSharedState(&state);

    pthread_t timerThreadId, seqThreadId, keyThreadId;

    // Create threads for sequencer and key input
    pthread_create(&seqThreadId, NULL, sequencerThread, &state);
    pthread_create(&keyThreadId, NULL, keyThread, &state);
    pthread_create(&timerThreadId, NULL, timerThread, &state);  // start the timer thread last

    // Event handler
    SDL_Event e;

    // While application is running
    while(!state.quit) {
        // Delegate keyboard events to the right thread:
        while(SDL_PollEvent(&e) != 0 ) {
            if (e.type == SDL_KEYDOWN) {
                pthread_mutex_lock(&state.mutex);
                state.scanCodeKeyDown = e.key.keysym.scancode;
                pthread_mutex_unlock(&state.mutex);
            } else if (e.type == SDL_KEYUP) {
                pthread_mutex_lock(&state.mutex);
                state.scanCodeKeyUp = e.key.keysym.scancode;
                pthread_mutex_unlock(&state.mutex);
            }
        }

        // Determine if rendering should take place:
        if (state.isRenderRequired) {
            if (isTimeMeasured) {
                clock_gettime(CLOCK_MONOTONIC, &renStartTime);
            }

            // Set the render target to our texture
            SDL_SetRenderTarget(renderer, renderTarget);
            // Clear the render target
            SDL_SetRenderDrawColor(renderer, COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b, 255);
            SDL_RenderClear(renderer);

            // BPM Blinker:
            drawBPMBlinker(&state.ppqnCounter);

            // Sidebar:
            drawSideBar();
            drawCurrentTrackIndicator(
                state.selectedSequence + 1,
                state.selectedPattern + 1,
                state.selectedTrack + 1
            );
            drawBPMIndiciator(state.bpm);
            drawPatternLengthIndicator(
                state.patternStepCounter, 
                state.project->sequences[state.selectedSequence].patterns[state.selectedPattern].length
            );

            // Basic Grid:
            drawBasicGrid(state.keyStates);
            
            // Render proper screen / program / menu:
            switch (state.screen) {
                case BLIPR_SCREEN_TRACK_SELECTION:
                    drawTrackSelection(&state.selectedTrack);    
                    break;
                case BLIPR_SCREEN_PATTERN_SELECTION:
                    drawPatternSelection(&state.selectedPattern, &state.queuedPattern);
                    break;
                case BLIPR_SCREEN_SEQUENCE_SELECTION:
                    drawSequenceSelection(&state.selectedSequence);
                    break;
                case BLIPR_SCREEN_CONFIGURATION:
                    drawConfigSelection(state.project);    
                    break;
                case BLIPR_SCREEN_TRACK_OPTIONS:
                    drawTrackOptions(state.track);
                    break;
                case BLIPR_SCREEN_PROGRAM_SELECTION:
                    drawProgramSelection(state.track);
                    break;
                case BLIPR_SCREEN_PATTERN_OPTIONS:
                    // No sequence options required?
                    drawPatternOptions(&state.project->sequences[state.selectedSequence].patterns[state.selectedPattern]);
                    break;
                case BLIPR_SCREEN_UTILITIES:
                    // Is this used?
                    drawCenteredLine(2, 61, "(UTILITIES)", TITLE_WIDTH, COLOR_WHITE);
                    break;
                case BLIPR_SCREEN_TRANSPORT:
                    drawCenteredLine(2, 61, "(TRANSPORT)", TITLE_WIDTH, COLOR_WHITE);
                    break;
                case BLIPR_SCREEN_NO_PROGRAM:
                    drawCenteredLine(2, 61, "(NO PROGRAM)", TITLE_WIDTH, COLOR_WHITE);
                    break;
                case BLIPR_SCREEN_SEQUENCER: 
                case BLIPR_SCREEN_DRUMKIT_SEQUENCER:
                    // TODO: Set proper prog for drumkit sequencer
                    progSequencer.draw(
                        &state.ppqnCounter, 
                        state.keyStates, 
                        state.track
                    );
                    break;
                case BLIPR_SCREEN_FOUR_ON_THE_FLOOR:
                    progFourOnTheFloor.draw(&state.ppqnCounter, state.keyStates, state.track);
                    break;
                default:
                    // Should not happen, but just in case
                    drawCenteredLine(2, 61, "(NO SCREEN)", TITLE_WIDTH, COLOR_WHITE);
                    break;
            }

            if (isTimeMeasured) {
                // Render in bottom right:
                char seqText[10];
                snprintf(seqText, 10, "S:%.2f%%", state.seqPerformance);
                drawText(WIDTH - 45, HEIGHT - 12, seqText, 45, COLOR_YELLOW);
                char renText[10];
                snprintf(renText, 10, "R:%.2f%%", state.renPerformance);
                drawText(WIDTH - 45, HEIGHT - 6, renText, 45, COLOR_YELLOW);
            }

            // Clear the renderer:
            SDL_SetRenderTarget(renderer, NULL);

            // Draw the scaled-up texture to the screen
            SDL_Rect destRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            if (isScreenRotated) {
                SDL_RenderCopyEx(renderer, renderTarget, NULL, &destRect, 180.0, NULL, SDL_FLIP_NONE);
            } else {
                SDL_RenderCopy(renderer, renderTarget, NULL, &destRect);
            }

            // Render:
            SDL_RenderPresent(renderer);
            
            // Reset flag:
            pthread_mutex_lock(&state.mutex);
            state.isRenderRequired = false;
            pthread_mutex_unlock(&state.mutex);

            if (isTimeMeasured) {
                clock_gettime(CLOCK_MONOTONIC, &renEndTime);
                int64_t elapsedNs = getTimespecDiffInNanoSeconds(&renStartTime, &renEndTime);
                double percentage = ((double)elapsedNs / state.nanoSecondsPerPulse) * 100.0;
                pthread_mutex_lock(&state.mutex);
                state.renPerformance = percentage;
                pthread_mutex_unlock(&state.mutex);
                print("Renderer took %dns to run (%.2f%%)", elapsedNs, percentage);
            }
        }
    }

    cleanupTextures();
    SDL_DestroyTexture(renderTarget);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    writeProjectFile(state.project, projectFile);
    cleanupSharedState(&state);
    
    return 0;
}

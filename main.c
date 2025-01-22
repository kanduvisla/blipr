#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "globals.h"
#include "drawing_utils.h"
#include "drawing.h"

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 720
#define NANOS_PER_SEC 1000000000LL  // Number of nanoseconds in a second
#define SCALE_FACTOR 8 // scale ratio (720 / 8 = 90 x 90 px)
#define WIDTH (WINDOW_WIDTH / SCALE_FACTOR)
#define HEIGHT (WINDOW_HEIGHT / SCALE_FACTOR)
#define GRID_UNIT 1

// Pulse per quarter note
#define PPQN 24
#define LINES_PER_BAR 4             // Assuming we're always in 4/4
#define PATTERN_LENGTH 16

// MAX/MIN Macro
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

// Renderer:
SDL_Renderer *renderer = NULL;

// The initial BPM:
double bpm = 120.0;

// Used for delay calculations:
int64_t nanoSecondsPerPulse = 0;

// Colors:
SDL_Color colorRed = {255, 0, 0, 255};
SDL_Color colorGray = {96, 96, 96, 255};
SDL_Color colorLightGray = {192, 192, 192, 255};
SDL_Color colorWhite = {255, 255, 255, 255};

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
 * Draw a simple button
 */
void drawButton(int x, int y, int width, int height) {
    drawBeveledRect(x, y, width, height, colorLightGray);
}

/**
 * Draw sequencer button
 */
void drawSequencerButton(int x, int y, int width, int height, bool isActive) {
    drawBeveledRect(x, y, width, height, colorLightGray);
    drawBeveledRectOutline(x + (width / 2) - 3, y + height - 5, 6, 3, colorLightGray, true);
    SDL_Color indicatorColor = adjustColorBrightness(colorRed, isActive ? 0 : -0.75f);
    drawLine(
        x + (width / 2) - 2, 
        y + height - 4,
        x + (width / 2) + 1, 
        y + height - 4,
        indicatorColor
    );
}

/**
 * Main loop
 */
int main(void)
{
    SDL_Window      *win = NULL;
    SDL_Texture     *renderTarget = NULL;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(720, 720, 0, &win, &renderer);

    renderTarget = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        WINDOW_WIDTH / SCALE_FACTOR,
        WINDOW_HEIGHT / SCALE_FACTOR
    );

    // Flag to determine if the app should quit:
    bool quit = false;
    calculateMicroSecondsPerPulse();

    // Clock:
    struct timespec prevTime, nowTime, newTime;
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

    // While application is running
    while(!quit)
    {
        // Handle events on queue
        while(SDL_PollEvent(&e) != 0 ) // poll for event
        {
            //User requests quit
            if( e.type == SDL_QUIT ) // unless player manually quits
            {
                quit = true;
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
            isPpqnTrigged = false;
            isClockResetRequired = true;
            isRenderRequired = true;
            ppqnCounter += 1;
            if (ppqnCounter % PPQN == 0) {
                ppqnCounter = 0;
                isNoteTrigged = true;
            }
        }

        // Check if note change is trigged:
        if (isNoteTrigged) {
            isNoteTrigged = false;
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
            isBeatTrigged = false;
        }

        // Drawing magic:
        if (isRenderRequired) {
            // Set the render target to our texture
            SDL_SetRenderTarget(renderer, renderTarget);

            // BPM Blinker:
            float p = (float)((noteCounter % 4) * PPQN + ppqnCounter) / (float)(PPQN * 4);
            int r = 255 * (1-p);

            SDL_Color colorBlinker = {r, 0, 0, 255};
            drawRectOutline(0, 0, WIDTH, HEIGHT, GRID_UNIT, colorBlinker);

            // 16 dots on the bottom for the steps:
            for (int i = 0; i < PATTERN_LENGTH; ++i) {
                drawPixel(
                    2 + (i * 2),
                    HEIGHT - (GRID_UNIT * 3),
                    noteCounter == i ? colorRed : colorGray
                );
            }            

            // 4 dots on the bottom right to indicate page:
            for (int i = 0; i < 4; ++i) {
                drawPixel(
                    WIDTH - 9 + (i * 2),
                    HEIGHT - (GRID_UNIT * 3),
                    pageCounter == i ? colorRed : colorGray
                );
            }

            // Sequencer buttons:


            // Test area:
            // drawButton(GRID_UNIT * 2, GRID_UNIT * 2, 10, 10);
            drawSequencerButton(2, 2, 10, 10, true);
            drawSequencerButton(13, 2, 10, 10, false);

            // Clear the renderer:
            SDL_SetRenderTarget(renderer, NULL);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Set to black
            SDL_RenderClear(renderer);

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

    return (0);
}

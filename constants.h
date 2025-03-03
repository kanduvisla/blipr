#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL.h>

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 720
#define NANOS_PER_SEC 1000000000LL  // Number of nanoseconds in a second
#define SCALE_FACTOR 4              // Scale ratio, 4=180x180px
#define WIDTH (WINDOW_WIDTH / SCALE_FACTOR)
#define HEIGHT (WINDOW_HEIGHT / SCALE_FACTOR)
#define BUTTON_WIDTH (WIDTH / 6)
#define BUTTON_HEIGHT (HEIGHT / 6)
#define SMALL_BUTTON_HEIGHT (BUTTON_HEIGHT / 2)
#define TITLE_WIDTH ((BUTTON_WIDTH * 4) + 3)
#define GRID_UNIT 1

// Pulse per quarter note
#define PPQN 24                                     // Pulses per quarter note, so this is 1 - 5 - 9 - 13
#define PPQN_MULTIPLIER 1                           // Multiplier for PPQN, so the in-app pulse counter can have more steps
#define PPQN_MULTIPLIED (PPQN * PPQN_MULTIPLIER)    // Multiplied PPQN (higher resolution)
#define PP16N (PPQN_MULTIPLIED / 4)                 // Pulses per 16th note (1 step)
#define MAX_PULSES (PP16N * 512)                    // Max amount of steps in a track can be 512

// Macros
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

// blipr key mapping
#define BLIPR_KEY_1 SDL_SCANCODE_1
#define BLIPR_KEY_2 SDL_SCANCODE_2
#define BLIPR_KEY_3 SDL_SCANCODE_3
#define BLIPR_KEY_4 SDL_SCANCODE_4
#define BLIPR_KEY_5 SDL_SCANCODE_Q
#define BLIPR_KEY_6 SDL_SCANCODE_W
#define BLIPR_KEY_7 SDL_SCANCODE_E
#define BLIPR_KEY_8 SDL_SCANCODE_R
#define BLIPR_KEY_9 SDL_SCANCODE_A
#define BLIPR_KEY_10 SDL_SCANCODE_S
#define BLIPR_KEY_11 SDL_SCANCODE_D
#define BLIPR_KEY_12 SDL_SCANCODE_F
#define BLIPR_KEY_13 SDL_SCANCODE_Z
#define BLIPR_KEY_14 SDL_SCANCODE_X
#define BLIPR_KEY_15 SDL_SCANCODE_C
#define BLIPR_KEY_16 SDL_SCANCODE_V

#define BLIPR_KEY_A SDL_SCANCODE_7
#define BLIPR_KEY_B SDL_SCANCODE_8
#define BLIPR_KEY_C SDL_SCANCODE_9
#define BLIPR_KEY_D SDL_SCANCODE_0

#define BLIPR_KEY_SHIFT_1 SDL_SCANCODE_LSHIFT
#define BLIPR_KEY_SHIFT_2 SDL_SCANCODE_LCTRL
#define BLIPR_KEY_FUNC SDL_SCANCODE_RSHIFT
#define BLIPR_KEY_SHIFT_3 SDL_SCANCODE_RCTRL

// Midi devices
#define BLIPR_MIDI_DEVICE_A 0
#define BLIPR_MIDI_DEVICE_B 1
#define BLIPR_MIDI_DEVICE_C 2
#define BLIPR_MIDI_DEVICE_D 3
#define BLIPR_MIDI_BUFFER_SIZE 256

// blipr programs
#define BLIPR_PROGRAM_NONE 0
#define BLIPR_PROGRAM_SEQUENCER 1
#define BLIPR_PROGRAM_FOUR_ON_THE_FLOOR 2

#endif // CONSTANTS_H
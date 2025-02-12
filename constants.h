#ifndef CONSTANTS_H
#define CONSTANTS_H

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 720
#define NANOS_PER_SEC 1000000000LL  // Number of nanoseconds in a second
#define SCALE_FACTOR 4.5f           // Scale ratio
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

#endif // CONSTANTS_H
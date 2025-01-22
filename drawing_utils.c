#include <SDL.h>
#include "globals.h"
#include "drawing_utils.h"

/**
 * Adjust color brightness
 */
SDL_Color adjustColorBrightness(SDL_Color color, float percent) {
    SDL_Color adjusted;
    
    // Ensure percent is between -1.0 and 1.0
    percent = (percent < -1.0f) ? -1.0f : (percent > 1.0f) ? 1.0f : percent;
    
    if (percent < 0) {
        // Darken
        adjusted.r = (Uint8)(color.r * (1 + percent));
        adjusted.g = (Uint8)(color.g * (1 + percent));
        adjusted.b = (Uint8)(color.b * (1 + percent));
    } else {
        // Lighten
        adjusted.r = (Uint8)(color.r + (255 - color.r) * percent);
        adjusted.g = (Uint8)(color.g + (255 - color.g) * percent);
        adjusted.b = (Uint8)(color.b + (255 - color.b) * percent);
    }
    
    // Keep alpha the same
    adjusted.a = color.a;
    
    return adjusted;
}

/**
 * Set the renderer color
 */
void setColor(SDL_Color color) {
    SDL_SetRenderDrawColor(
        renderer, 
        color.r,
        color.g,
        color.b,
        color.a
    );
}

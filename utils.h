#ifndef UTILS_H
#define UTILS_H

#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Convert a scancode to a step index
 */
int scancodeToStep(SDL_Scancode key);

/**
 * Uppercase a string
 */
void upperCase(char *str);

/**
 * Create a byte that stores 2 boolean flags and a value of 0-63
 */
uint8_t create2FByte(bool flag1, bool flag2, uint8_t value);

/**
 * Get flag 1 from a byte with 2 flags
 */
bool get2FByteFlag1(uint8_t byte);

/**
 * Get flag 2 from a byte with 2 flags
 */
bool get2FByteFlag2(uint8_t byte);

/**
 * Get value from a byte with 2 flags
 */
uint8_t get2FByteValue(uint8_t byte);

unsigned char incrementHighNibble(unsigned char byte);
unsigned char decrementHighNibble(unsigned char byte);
unsigned char incrementLowNibble(unsigned char byte);
unsigned char decrementLowNibble(unsigned char byte);

#ifdef __cplusplus
}
#endif

#endif
#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include "utils.h"
#include "constants.h"

/**
 * Convert a scancode to a step index
 */
int scancodeToStep(SDL_Scancode key) {
    switch(key) {
        case BLIPR_KEY_1:
            return 0;
        case BLIPR_KEY_2:
            return 1;
        case BLIPR_KEY_3:
            return 2;
        case BLIPR_KEY_4:
            return 3;
        case BLIPR_KEY_5:
            return 4;
        case BLIPR_KEY_6:
            return 5;
        case BLIPR_KEY_7:
            return 6;
        case BLIPR_KEY_8:
            return 7;
        case BLIPR_KEY_9:
            return 8;
        case BLIPR_KEY_10:
            return 9;
        case BLIPR_KEY_11:
            return 10;
        case BLIPR_KEY_12:
            return 11;
        case BLIPR_KEY_13:
            return 12;
        case BLIPR_KEY_14:
            return 13;
        case BLIPR_KEY_15:
            return 14;
        case BLIPR_KEY_16:
            return 15;
        default:
            return -1;
    }
};

/**
 * Uppercase a string
 */
void upperCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char) str[i]);
    }
}

/**
 * Create a byte that stores 2 boolean flags and a value of 0-63
 */
uint8_t create2FByte(bool flag1, bool flag2, uint8_t value) {
    uint8_t result = 0;
    
    // Set flag1 at bit 0
    if (flag1) {
        result |= 0x01;  // Set bit 0
    }
    
    // Set flag2 at bit 1
    if (flag2) {
        result |= 0x02;  // Set bit 1
    }
    
    // Ensure value is in range 0-63 (6 bits)
    value &= 0x3F;
    
    // Shift value to bits 2-7 and combine with flags
    result |= (value << 2);
    
    return result;
}

/**
 * Get flag 1 from a byte with 2 flags
 */
bool get2FByteFlag1(uint8_t byte) {
    return (byte & 0x01) != 0;
}

/**
 * Get flag 2 from a byte with 2 flags
 */
bool get2FByteFlag2(uint8_t byte) {
    return (byte & 0x02) != 0;
}

/**
 * Get value from a byte with 2 flags
 */
uint8_t get2FByteValue(uint8_t byte) {
    return (byte >> 2) & 0x3F;
}

// Function to increment the high nibble (first hex digit)
unsigned char incrementHighNibble(unsigned char byte) {
    // Extract high nibble, increment it, handle overflow
    unsigned char highNibble = (byte >> 4) & 0x0F;
    highNibble = (highNibble + 1) & 0x0F;  // Increment and wrap around at 16
    
    // Clear the high nibble in original byte and set the new high nibble
    byte = (byte & 0x0F) | (highNibble << 4);
    
    return byte;
}

// Function to decrement the high nibble (first hex digit)
unsigned char decrementHighNibble(unsigned char byte) {
    // Extract high nibble, decrement it, handle underflow
    unsigned char highNibble = (byte >> 4) & 0x0F;
    highNibble = (highNibble - 1) & 0x0F;  // Decrement and wrap around at 0
    
    // Clear the high nibble in original byte and set the new high nibble
    byte = (byte & 0x0F) | (highNibble << 4);
    
    return byte;
}

// Function to increment the low nibble (second hex digit)
unsigned char incrementLowNibble(unsigned char byte) {
    // Extract low nibble, increment it, handle overflow
    unsigned char lowNibble = byte & 0x0F;
    lowNibble = (lowNibble + 1) & 0x0F;  // Increment and wrap around at 16
    
    // Clear the low nibble in original byte and set the new low nibble
    byte = (byte & 0xF0) | lowNibble;
    
    return byte;
}

// Function to decrement the low nibble (second hex digit)
unsigned char decrementLowNibble(unsigned char byte) {
    // Extract low nibble, decrement it, handle underflow
    unsigned char lowNibble = byte & 0x0F;
    lowNibble = (lowNibble - 1) & 0x0F;  // Decrement and wrap around at 0
    
    // Clear the low nibble in original byte and set the new low nibble
    byte = (byte & 0xF0) | lowNibble;
    
    return byte;
}
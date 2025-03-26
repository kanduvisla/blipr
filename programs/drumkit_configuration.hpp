#ifndef FOUR_ON_THE_FLOOR_H
#define FOUR_ON_THE_FLOOR_H

#include "../project.h"

#define DRUMKIT_NOTE_BYTE_SIZE 4
#define DRUMKIT_BYTE_SIZE (DRUMKIT_NOTE_BYTE_SIZE * 16) // 16 notes x 4

/**
 * A Drumkit Note, this is a variant on the default note, aimed at drumkits
 * The idea is that we can have different drum kit samples based on different Midi CC values (for Model:Cycles for example)
 */
struct DrumkkitNote {
    unsigned char note;         // byte representation of note C-5, D#3, etc.
    unsigned char velocity;     // 0-127
    unsigned char cc1Value;     // CC1 Value
    unsigned char cc2Value;     // CC2 Value
};

/**
 * Drumkit element
 */
typedef enum {
    DRUMKIT_KICK = 0,
    DRUMKIT_SNARE = 1,
    DRUMKIT_CLAP = 2,
    DRUMKIT_RIMSHOT = 3,
    DRUMKIT_CHAT_1 = 4,
    DRUMKIT_CHAT_2 = 5,
    DRUMKIT_OHAT = 6,
    DRUMKIT_RIDE = 7,
    DRUMKIT_CRASH = 8,
    DRUMKIT_LTOM = 9,
    DRUMKIT_MTOM = 10,
    DRUMKIT_HTOM = 11,
    DRUMKIT_COWBELL = 12,
    DRUMKIT_EXTRA_1 = 13,
    DRUMKIT_EXTRA_2 = 14,
    DRUMKIT_EXTRA_3 = 15
} DrumkitElement;

/**
 * Drumkit Configuration 
 * ---
 * This class represents the different midi notes used for a drumkit configuration.
 * So we can define a note for each drumkit element
 **/
class DrumkitConfiguration {
public:
    /**
     * Get the note for a given drumkit element
     */
    struct Note getNoteForDrumkitElement(DrumkitElement element);

    /**
     * Initialize this class from a byte array
     */
    void initFromByteArray(const unsigned char bytes[DRUMKIT_BYTE_SIZE]);

    /**
     * Populate a byte array from this class
     */
    void toByteArray(unsigned char bytes[DRUMKIT_BYTE_SIZE]);
};

#endif
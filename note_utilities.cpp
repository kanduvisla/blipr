#include "note_utilities.hpp"

unsigned char NoteUtilities::transpose(unsigned char midiNote, int steps)
{
    // Ensure the input is a valid MIDI note
    if (midiNote > 127) {
        return 0;  // Return 0 (lowest note) for invalid input
    }

    // Perform the transposition
    int transposed = (int)midiNote + steps;

    // Clamp the result to the valid MIDI note range (0-127)
    if (transposed < 0) {
        transposed = 0;
    } else if (transposed > 127) {
        transposed = 127;
    } 

    return (unsigned char)transposed;       
}

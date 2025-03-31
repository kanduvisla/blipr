#ifndef TRIGG_HELPER_H
#define TRIGG_HELPER_H

// Trig conditions:
// Byte structure
// Bit 1:   Enabled / Disabled
// Bit 2:   Inversed / Not inversed (for example, Trig 4-4 = ...x, inversed = xxx.)
// Bit 3-8: Mode (see list below, 64 modes in total)
#define TRIG_DISABLED 0
#define TRIG_1_2 1 // TRIG_2_2 is TRIG_1_2 inversed
#define TRIG_1_3 2
#define TRIG_2_3 3
#define TRIG_3_3 4
#define TRIG_1_4 5
#define TRIG_2_4 6
#define TRIG_3_4 7
#define TRIG_4_4 8
#define TRIG_1_5 9
#define TRIG_2_5 10
#define TRIG_3_5 11
#define TRIG_4_5 12
#define TRIG_5_5 13
#define TRIG_1_6 14
#define TRIG_2_6 15
#define TRIG_3_6 16
#define TRIG_4_6 17
#define TRIG_5_6 18
#define TRIG_6_6 19
#define TRIG_1_7 20
#define TRIG_2_7 21
#define TRIG_3_7 22
#define TRIG_4_7 23
#define TRIG_5_7 24
#define TRIG_6_7 25
#define TRIG_7_7 26
#define TRIG_1_8 27
#define TRIG_2_8 28
#define TRIG_3_8 29
#define TRIG_4_8 30
#define TRIG_5_8 31
#define TRIG_6_8 32
#define TRIG_7_8 33
#define TRIG_8_8 34
#define TRIG_1_PERCENT 35
#define TRIG_2_PERCENT 36
#define TRIG_5_PERCENT 37
#define TRIG_10_PERCENT 38
#define TRIG_25_PERCENT 39
#define TRIG_33_PERCENT 40
#define TRIG_50_PERCENT 41  // Higher than 50% are the inversed versions of these
#define TRIG_FILL 42        // When fill is trigged (TODO: determine how "FILL" is used)
#define TRIG_FIRST 43       // When first entering a pattern
#define TRIG_TRANSITION 44  // When transitioning to another pattern
#define TRIG_FIRST_PAGE 45          // When first entering a page
#define TRIG_TRANSITION_PAGE 46     // When transitioning to another page
#define TRIG_HIGHER_FIRST 47        // When first entering a higher pattern
#define TRIG_HIGHER_TRANSITION 48   // When transitioning to a higher pattern
#define TRIG_HIGHER_FIRST_PAGE 49       // When first entering a higher page
#define TRIG_HIGHER_TRANSITION_PAGE 50  // When transitioning to a higher page
#define TRIG_LOWER_FIRST 51             // When first entering a lower pattern
#define TRIG_LOWER_TRANSITION 52        // When transitioning to a lower pattern
#define TRIG_LOWER_FIRST_PAGE 53       // When first entering a lower page
#define TRIG_LOWER_TRANSITION_PAGE 54  // When transitioning to a lower page
#define TRIG_FIRST_PAGE_PLAY 55         // Trigged when the page is played the 1st time (does not re-trig when the page is played a second time when coming back from another page)
#define TRIG_FIRST_PATTERN_PLAY 56      // Trigged when the pattern is played the 1st time (does not re-trig when the pattern is played a second time when coming back from another pattern)
#define TRIG_TRANSITION_LAST_PAGE 57     // When transitioning to another page and the last repeat of transition repeats is played
#define TRIG_HIGHEST_VALUE 56         // Used internally for decision making, make sure to change this if you add new trigg conditions

class TriggHelper {
    /**
     * Check if the note is triggered according to the trigg condition
     * @param triggValue    The Trigg value
     * @param repeatCount   How many times this note has already been played (determined by tracklength or page size)
     */
    bool TriggHelper::isTrigged(int triggValue, int repeatCount);
};

#endif
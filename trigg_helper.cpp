#include "trigg_helper.hpp"
#include "utils.h"

/**
 * Check if the note is triggered according to the trigg condition
 * @param triggValue    The Trigg value
 * @param repeatCount   How many times this note has already been played (determined by tracklength or page size)
 */
bool TriggHelper::isTrigged(int triggValue, int repeatCount) {
    bool isEnabled = triggValue > 0; // get2FByteFlag1(triggValue);
    if (!isEnabled) {
        // If a trig condition is not enabled it will always pass:
        return true;
    }

    bool isTrigged = true;
    bool isInversed = get2FByteFlag2(triggValue);
    int value = get2FByteValue(triggValue);
    
    switch (value) {
        case TRIG_DISABLED:
            isTrigged = true;
            break;
        case TRIG_1_2:
            isTrigged = repeatCount % 2 == 0;
            break;
        case TRIG_1_3:
            isTrigged = repeatCount % 3 == 0;
            break;
        case TRIG_2_3:
            isTrigged = repeatCount % 3 == 1;
            break;
        case TRIG_3_3:
            isTrigged = repeatCount % 3 == 2;
            break;
        case TRIG_1_4:
            isTrigged = repeatCount % 4 == 0;
            break;
        case TRIG_2_4:
            isTrigged = repeatCount % 4 == 1;
            break;
        case TRIG_3_4:
            isTrigged = repeatCount % 4 == 2;
            break;
        case TRIG_4_4:
            isTrigged = repeatCount % 4 == 3;
            break;
        case TRIG_1_5:
            isTrigged = repeatCount % 5 == 0;
            break;
        case TRIG_2_5:
            isTrigged = repeatCount % 5 == 1;
            break;
        case TRIG_3_5:
            isTrigged = repeatCount % 5 == 2;
            break;
        case TRIG_4_5:
            isTrigged = repeatCount % 5 == 3;
            break;
        case TRIG_5_5:
            isTrigged = repeatCount % 5 == 4;
            break;
        case TRIG_1_6:
            isTrigged = repeatCount % 6 == 0;
            break;
        case TRIG_2_6:
            isTrigged = repeatCount % 6 == 1;
            break;
        case TRIG_3_6:
            isTrigged = repeatCount % 6 == 2;
            break;
        case TRIG_4_6:
            isTrigged = repeatCount % 6 == 3;
            break;
        case TRIG_5_6:
            isTrigged = repeatCount % 6 == 4;
            break;
        case TRIG_6_6:
            isTrigged = repeatCount % 6 == 5;
            break;
        case TRIG_1_7:
            isTrigged = repeatCount % 7 == 0;
            break;
        case TRIG_2_7:
            isTrigged = repeatCount % 7 == 1;
            break;
        case TRIG_3_7:
            isTrigged = repeatCount % 7 == 2;
            break;
        case TRIG_4_7:
            isTrigged = repeatCount % 7 == 3;
            break;
        case TRIG_5_7:
            isTrigged = repeatCount % 7 == 4;
            break;
        case TRIG_6_7:
            isTrigged = repeatCount % 7 == 5;
            break;
        case TRIG_7_7:
            isTrigged = repeatCount % 7 == 6;
            break;
        case TRIG_1_8:
            isTrigged = repeatCount % 8 == 0;
            break;
        case TRIG_2_8:
            isTrigged = repeatCount % 8 == 1;
            break;
        case TRIG_3_8:
            isTrigged = repeatCount % 8 == 2;
            break;
        case TRIG_4_8:
            isTrigged = repeatCount % 8 == 3;
            break;
        case TRIG_5_8:
            isTrigged = repeatCount % 8 == 4;
            break;
        case TRIG_6_8:
            isTrigged = repeatCount % 8 == 5;
            break;
        case TRIG_7_8:
            isTrigged = repeatCount % 8 == 6;
            break;
        case TRIG_8_8:
            isTrigged = repeatCount % 8 == 7;
            break;
        case TRIG_1_PERCENT:
            isTrigged = rand() % 100 == 0;
            break;
        case TRIG_2_PERCENT:
            isTrigged = rand() % 50 == 0;
            break;
        case TRIG_5_PERCENT:
            isTrigged = rand() % 20 == 0;
            break;
        case TRIG_10_PERCENT:
            isTrigged = rand() % 10 == 0;
            break;
        case TRIG_25_PERCENT:
            isTrigged = rand() % 4 == 0;
            break;
        case TRIG_33_PERCENT:
            isTrigged = rand() % 3 == 0;
            break;
        case TRIG_50_PERCENT:
            isTrigged = rand() % 2 == 0;
            break;
        case TRIG_FIRST:
            isTrigged = repeatCount == 0;
            break;
    }

    return isInversed ? !isTrigged : isTrigged;
}

/**
 * Set the trigg text to a given string
 */
void TriggHelper::setTriggText(int triggValue, char *text) {
    bool isEnabled = get2FByteFlag1(triggValue);
    if (!isEnabled) {
        snprintf(text, 4, "OFF");
        return;
    }

    bool isInversed = get2FByteFlag2(triggValue);
    int value = get2FByteValue(triggValue);

    switch (value) {
        case TRIG_DISABLED:
            snprintf(text, 4, "OFF");       
            case TRIG_1_2:
            snprintf(text, 4, "%s1:2", isInversed? "!" : "");
            break;
        case TRIG_1_3:
            snprintf(text, 4, "%s1:3", isInversed? "!" : "");
            break;
        case TRIG_2_3:
            snprintf(text, 4, "%s2:3", isInversed? "!" : "");
            break;
        case TRIG_3_3:
            snprintf(text, 4, "%s3:3", isInversed? "!" : "");
            break;
        case TRIG_1_4:
            snprintf(text, 4, "%s1:4", isInversed? "!" : "");
            break;
        case TRIG_2_4:
            snprintf(text, 4, "%s2:4", isInversed? "!" : "");
            break;
        case TRIG_3_4:
            snprintf(text, 4, "%s3:4", isInversed? "!" : "");
            break;
        case TRIG_4_4:
            snprintf(text, 4, "%s4:4", isInversed? "!" : "");
            break;
        case TRIG_1_5:
            snprintf(text, 4, "%s1:5", isInversed? "!" : "");
            break;
        case TRIG_2_5:
            snprintf(text, 4, "%s2:5", isInversed? "!" : "");
            break;
        case TRIG_3_5:
            snprintf(text, 4, "%s3:5", isInversed? "!" : "");
            break;
        case TRIG_4_5:
            snprintf(text, 4, "%s4:5", isInversed? "!" : "");
            break;
        case TRIG_5_5:
            snprintf(text, 4, "%s5:5", isInversed? "!" : "");
            break;
        case TRIG_1_6:
            snprintf(text, 4, "%s1:6", isInversed? "!" : "");
            break;
        case TRIG_2_6:
            snprintf(text, 4, "%s2:6", isInversed? "!" : "");
            break;
        case TRIG_3_6:
            snprintf(text, 4, "%s3:6", isInversed? "!" : "");
            break;
        case TRIG_4_6:
            snprintf(text, 4, "%s4:6", isInversed? "!" : "");
            break;
        case TRIG_5_6:
            snprintf(text, 4, "%s5:6", isInversed? "!" : "");
            break;
        case TRIG_6_6:
            snprintf(text, 4, "%s6:6", isInversed? "!" : "");
            break;
        case TRIG_1_7:
            snprintf(text, 4, "%s1:7", isInversed? "!" : "");
            break;
        case TRIG_2_7:
            snprintf(text, 4, "%s2:7", isInversed? "!" : "");
            break;
        case TRIG_3_7:
            snprintf(text, 4, "%s3:7", isInversed? "!" : "");
            break;
        case TRIG_4_7:
            snprintf(text, 4, "%s4:7", isInversed? "!" : "");
            break;
        case TRIG_5_7:
            snprintf(text, 4, "%s5:7", isInversed? "!" : "");
            break;
        case TRIG_6_7:
            snprintf(text, 4, "%s6:7", isInversed? "!" : "");
            break;
        case TRIG_7_7:
            snprintf(text, 4, "%s7:7", isInversed? "!" : "");
            break;
        case TRIG_1_8:
            snprintf(text, 4, "%s1:8", isInversed? "!" : "");
            break;
        case TRIG_2_8:
            snprintf(text, 4, "%s2:8", isInversed? "!" : "");
            break;
        case TRIG_3_8:
            snprintf(text, 4, "%s3:8", isInversed? "!" : "");
            break;
        case TRIG_4_8:
            snprintf(text, 4, "%s4:8", isInversed? "!" : "");
            break;
        case TRIG_5_8:
            snprintf(text, 4, "%s5:8", isInversed? "!" : "");
            break;
        case TRIG_6_8:
            snprintf(text, 4, "%s6:8", isInversed? "!" : "");
            break;
        case TRIG_7_8:
            snprintf(text, 4, "%s7:8", isInversed? "!" : "");
            break;
        case TRIG_8_8:
            snprintf(text, 4, "%s8:8", isInversed? "!" : "");
            break;
        case TRIG_1_PERCENT:
            strcpy(text, isInversed ? "99%" : "1%");
            break;
        case TRIG_2_PERCENT:
            strcpy(text, isInversed ? "98%" : "2%");
            break;
        case TRIG_5_PERCENT:
            strcpy(text, isInversed ? "95%" : "5%");
            break;
        case TRIG_10_PERCENT:
            strcpy(text, isInversed ? "90%" : "10%");
            break;
        case TRIG_25_PERCENT:
            strcpy(text, isInversed ? "75%" : "25%");
            break;
        case TRIG_33_PERCENT:
            strcpy(text, isInversed ? "66%" : "33%");
            break;
        case TRIG_50_PERCENT:
            strcpy(text, "50%");
            break;
        case TRIG_FILL:
            snprintf(text, 4, "%sFILL", isInversed? "!" : "");
            break;
        case TRIG_FIRST:
            snprintf(text, 4, "%s1ST", isInversed? "!" : "");
            break;
        case TRIG_TRANSITION:
            snprintf(text, 4, "%sTRANS", isInversed? "!" : "");
            break;
        case TRIG_FIRST_PAGE:
            snprintf(text, 4, "%s1ST.P", isInversed? "!" : "");
            break;
        case TRIG_TRANSITION_PAGE:
            snprintf(text, 4, "%sTRN.P", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_FIRST:
            snprintf(text, 4, "%s1ST.H", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_TRANSITION:
            snprintf(text, 4, "%sTRN.H", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_FIRST_PAGE:
            snprintf(text, 4, "%s1ST.HP", isInversed? "!" : "");
            break;
        case TRIG_HIGHER_TRANSITION_PAGE:
            snprintf(text, 4, "%sTRN.HP", isInversed? "!" : "");
            break;
        case TRIG_LOWER_FIRST:
            snprintf(text, 4, "%s1ST.L", isInversed? "!" : "");
            break;
        case TRIG_LOWER_TRANSITION:
            snprintf(text, 4, "%sTRN.L", isInversed? "!" : "");
            break;
        case TRIG_LOWER_FIRST_PAGE:
            snprintf(text, 4, "%s1ST.LP", isInversed? "!" : "");
            break;
        case TRIG_LOWER_TRANSITION_PAGE:
            snprintf(text, 4, "%sTRN.LP", isInversed? "!" : "");
            break;
        case TRIG_FIRST_PAGE_PLAY:
            snprintf(text, 4, "%s1ST.PG", isInversed? "!" : "");
            break;
        case TRIG_FIRST_PATTERN_PLAY:
            snprintf(text, 4, "%s1ST.PT", isInversed? "!" : "");
            break;
        default:
            strcpy(text, "---");
            break;
    }
}

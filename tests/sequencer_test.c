#include <stdbool.h>
#include "../programs/sequencer.h"

void testCalculateTrackStepIndex() {
    // Test cases, for manual testing:
    /*
    printf("Page size 12, total length 64:\n");
    for (int i = 0; i < 170; i++) {
        printf("PPQN Step: %d, Track Step Index: %d\n", i, calculateTrackStepIndex(i, 12, 64));
    }

    printf("\nPage size 4, total length 64:\n");
    for (int i = 0; i < 120; i++) {
        printf("PPQN Step: %d, Track Step Index: %d\n", i, calculateTrackStepIndex(i, 4, 64));
    }

    printf("\nPage size 12, total length 44:\n");
    for (int i = 0; i < 150; i++) {
        printf("PPQN Step: %d, Track Step Index: %d\n", i, calculateTrackStepIndex(i, 12, 44));
    }

    printf("\nPage size 16, total length 44:\n");
    for (int i = 0; i < 150; i++) {
        printf("PPQN Step: %d, Track Step Index: %d\n", i, calculateTrackStepIndex(i, 16, 44));
    }
    */

    // printf("\nPage size 16, total length 44:\n");
    // for (int i = 0; i < 150; i++) {
    //     printf("PPQN Step: %d, Track Step Index: %d\n", i, calculateTrackStepIndex(i, 16, 44));
    // }

    // printf("\nPage size 12, total length 44:\n");
    // for (int i = 0; i < 150; i++) {
    //     printf("PPQN Step: %d, Track Step Index: %d\n", i, calculateTrackStepIndex(i, 12, 44));
    // }

    // assert(calculateTrackStepIndex(36, 12, 44) == 0);
    // assert(calculateTrackStepIndex(37, 12, 44) == 1);
    // assert(calculateTrackStepIndex(48, 12, 44) == 16);
    // return;

    // Each track has a page size, this can be 1 to 16.
    // Pages are divided in the track.steps data in slots of 16.
    // So for example: if page size is 12, slot 1 will be [0-11], slot 2 will be [16-27], slot 3 will be [32-43], etc.
    assert(calculateTrackStepIndex(0, 12, 64, PAGE_PLAY_MODE_CONTINUOUS) == 0);
    assert(calculateTrackStepIndex(12, 12, 64, PAGE_PLAY_MODE_CONTINUOUS) == 16);
    assert(calculateTrackStepIndex(24, 12, 64, PAGE_PLAY_MODE_CONTINUOUS) == 32);
    assert(calculateTrackStepIndex(36, 12, 64, PAGE_PLAY_MODE_CONTINUOUS) == 48);
    assert(calculateTrackStepIndex(48, 12, 64, PAGE_PLAY_MODE_CONTINUOUS) == 0);
    // Meanwhile, the step calculated from the PPQN passed will be ever increased. So the challenge here will be to map this step to the index in the track.steps array
    // In the example above, if ppqnStep is 11, the trackStepIndex will be 11, but if ppqnStep is 12, the next slot is triggered and the trackStepIndex will be 16.
    // This can also be challenging for smaller page sizes. For example, with a page size of 4:
    assert(calculateTrackStepIndex(0, 4, 64, PAGE_PLAY_MODE_CONTINUOUS) == 0);
    assert(calculateTrackStepIndex(3, 4, 64, PAGE_PLAY_MODE_CONTINUOUS) == 3);
    assert(calculateTrackStepIndex(4, 4, 64, PAGE_PLAY_MODE_CONTINUOUS) == 16);
    assert(calculateTrackStepIndex(7, 4, 64, PAGE_PLAY_MODE_CONTINUOUS) == 19);
    assert(calculateTrackStepIndex(8, 4, 64, PAGE_PLAY_MODE_CONTINUOUS) == 32);
    assert(calculateTrackStepIndex(12, 4, 64, PAGE_PLAY_MODE_CONTINUOUS) == 48);
    assert(calculateTrackStepIndex(16, 4, 64, PAGE_PLAY_MODE_CONTINUOUS) == 0);
    // Also - to make things more difficult - the complete track can have a maximum length as well, and the trackStepIndex can never exceed that. If that happens, it needs to go back to the first slot.
    // For example, with a page size of 12, and a total track length of 44:
    assert(calculateTrackStepIndex(0, 12, 44, PAGE_PLAY_MODE_CONTINUOUS) == 0);
    assert(calculateTrackStepIndex(12, 12, 44, PAGE_PLAY_MODE_CONTINUOUS) == 16);
    assert(calculateTrackStepIndex(23, 12, 44, PAGE_PLAY_MODE_CONTINUOUS) == 27);
    assert(calculateTrackStepIndex(24, 12, 44, PAGE_PLAY_MODE_CONTINUOUS) == 32);
    assert(calculateTrackStepIndex(35, 12, 44, PAGE_PLAY_MODE_CONTINUOUS) == 43);
    assert(calculateTrackStepIndex(36, 12, 44, PAGE_PLAY_MODE_CONTINUOUS) == 0);
    assert(calculateTrackStepIndex(37, 12, 44, PAGE_PLAY_MODE_CONTINUOUS) == 1);
    assert(calculateTrackStepIndex(48, 12, 44, PAGE_PLAY_MODE_CONTINUOUS) == 16);
    // For example, with a page size of 16, and a total track length of 44:
    assert(calculateTrackStepIndex(0, 16, 44, PAGE_PLAY_MODE_CONTINUOUS) == 0);
    assert(calculateTrackStepIndex(12, 16, 44, PAGE_PLAY_MODE_CONTINUOUS) == 12);
    assert(calculateTrackStepIndex(23, 16, 44, PAGE_PLAY_MODE_CONTINUOUS) == 23);
    assert(calculateTrackStepIndex(24, 16, 44, PAGE_PLAY_MODE_CONTINUOUS) == 24);
    assert(calculateTrackStepIndex(35, 16, 44, PAGE_PLAY_MODE_CONTINUOUS) == 35);
    assert(calculateTrackStepIndex(36, 16, 44, PAGE_PLAY_MODE_CONTINUOUS) == 36);
    assert(calculateTrackStepIndex(37, 16, 44, PAGE_PLAY_MODE_CONTINUOUS) == 37);
    assert(calculateTrackStepIndex(44, 16, 44, PAGE_PLAY_MODE_CONTINUOUS) == 0);
    assert(calculateTrackStepIndex(48, 16, 44, PAGE_PLAY_MODE_CONTINUOUS) == 4);
    assert(calculateTrackStepIndex(52, 16, 44, PAGE_PLAY_MODE_CONTINUOUS) == 8);
}

void testSequencer() {
    testCalculateTrackStepIndex();
}
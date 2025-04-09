#include <stdio.h>
#include "../pattern_player.hpp"

void testPatternPlayer() {
    PatternPlayer subject = PatternPlayer();

    assert(subject.getQueuedPattern() == -1);
    assert(subject.getPlayingPattern() == -1);

    subject.queuePattern(0);
    assert(subject.getQueuedPattern() == -1);
    assert(subject.getPlayingPattern() == 0);

    subject.queuePattern(1);
    assert(subject.getQueuedPattern() == 1);
    assert(subject.getPlayingPattern() == 0);

    // Steps:
    assert(subject.getStep() == 0);
    subject.step();
    assert(subject.getStep() == 1);
    subject.resetStep();
    assert(subject.getStep() == 0);
}

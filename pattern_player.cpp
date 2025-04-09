#include "stddef.h"
#include "pattern_player.hpp"

void PatternPlayer::queuePattern(int index) {
    if (playingPattern == -1) {
        playingPattern = index;
    } else {
        queuedPattern = index;
    }
}

void PatternPlayer::step() {
    currentStepInPattern++;
}

void PatternPlayer::resetStep() {
    currentStepInPattern = 0;
}

bool PatternPlayer::isQueueEmpty() {
    if (queuedPattern == -1) {
        return true;
    }

    if (queuedPattern == playingPattern) {
        return true;
    }
}

void PatternPlayer::playPatternFromQueue() {
    if (!isQueueEmpty()) {
        playingPattern = queuedPattern;
        queuedPattern = -1;
        resetStep();
    }
}

int PatternPlayer::getPlayingPattern() {
    return playingPattern;
}

int PatternPlayer::getQueuedPattern() {
    return queuedPattern;
}

int PatternPlayer::getStep() {
    return currentStepInPattern;
}
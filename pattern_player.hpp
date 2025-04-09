#ifndef PATTERN_PLAYER_H
#define PATTERN_PLAYER_H

/// @brief The Pattern Player keeps overall track of playing the pattern
class PatternPlayer {
public:
    /// @brief Queue the next pattern for playing
    /// @param index 
    void queuePattern(int index);

    /// @brief Do a single step
    void step();

    /// @brief Reset the Pattern Player step
    void resetStep();

    /// @brief Is the queue empty?
    /// @return 
    bool isQueueEmpty();

    /// @brief Play the pattern from the queue
    void playPatternFromQueue();

    /// @brief Get the current playing pattern
    /// @return 
    int getPlayingPattern();

    /// @brief Get the queued pattern
    /// @return 
    int getQueuedPattern();
    
    /// @brief Get the current step
    /// @return 
    int getStep();

protected:

private:
    int playingPattern = -1;
    int queuedPattern = -1;
    int currentStepInPattern = 0;

};

#endif
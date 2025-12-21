#pragma once

#include <chrono>

class Clock {
private:
    using clock = std::chrono::high_resolution_clock;
    
    // defines the reference point for previousTime and currentTime
    std::chrono::time_point<clock> initialisationTime = clock::now(); 
    float previousTime = 0.0f;
    float currentTime = 0.0f;

public:
    /**
     * @Note: Should be called before entering a frame loop to clear any delta time
     *        accumulation from outside the frame loop
     */
    void update() {
        std::chrono::duration<float> elapsed = clock::now() - initialisationTime;
        previousTime = currentTime;
        currentTime = elapsed.count();
    }

    void reset() {
        initialisationTime = clock::now();
        previousTime = 0.0f;
        currentTime = 0.0f;
    }

    float getCurrentTime() const {
        return currentTime;
    }

    float getPreviousTime() const {
        return previousTime;
    }

    float getDeltaTime() const {
        return currentTime - previousTime;
    }
};

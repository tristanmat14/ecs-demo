#pragma once

#include <iostream>

#include "Clock.hpp"

class Application {
protected:
    Clock frameClock;
    bool running = true;

    /**
     * Called from the start method.
     * Perform OS-specific initialization here.
     * Return false to abort startup.
     */
    virtual bool onStart() { return true; }
    
    /**
     * Called from the end method.
     * Perform OS-specific cleanup here.
     */
    virtual void onEnd() {}

    /**
     * Called once before each frame.
     * Handle OS-specific events here.
     * Return false to stop the application.
     */
    virtual bool onPreFrame() { return true; }

    /**
     * Called once after each frame.
     * Handle OS-specific rendering/presentation here.
     */
    virtual void onPostFrame() {}

    /**
     * Core application logic.
     * OS-independent code should be defined in this class.
     */
    virtual void runFrame() {}

public:
    Application() = default; 
    virtual ~Application() = default;

    bool start() {
        running = onStart();

        // Reset frameClock after calling onStart() so the initial time is more accurate.
        frameClock.reset();

        return running;
    }
    
    void end() {
        onEnd();
    }
    
    /**
     * The Main loop - calls pre/post frame hooks and runs core logic.
     */
    void run() {
        float frameTimeSum = 0;
        int frameCount = 0;

        while (running && onPreFrame()) {
            frameClock.updateLap();
            frameTimeSum += frameClock.getDeltaTime();
            frameCount++;

            // FPS tracker
            if (frameCount >= 50) {
                float frameTimeAvg = frameTimeSum / frameCount;
                frameTimeSum = 0.0f;
                frameCount = 0;
                float FPS = 1.0f / frameTimeAvg;
                std::cout << "FPS: " << FPS << '\n';
            }
           
            runFrame();

            onPostFrame();
        }
    }
};

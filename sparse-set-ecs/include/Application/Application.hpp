#pragma once

#include <iostream>
#include <memory>

#include "IProgram.hpp"
#include "ECSProgram.hpp"
#include "Clock.hpp"

class Application {
private:
    std::unique_ptr<IProgram> program = std::make_unique<ECSProgram>();

    void trackFPS(float dt) {
        static float frameTimeSum = 0.0f;
        static int frameCount = 0;

        frameTimeSum += dt;
        frameCount++;

        if (frameCount >= 100) {
            float frameTimeAvg = frameTimeSum / frameCount;
            frameTimeSum = 0.0f;
            frameCount = 0;
            float FPS = 1.0f / frameTimeAvg;
            std::cout << "FPS: " << FPS << '\n';
        }
    }

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
     * TEMPORARY - REMOVE
     */
    virtual void onRender() {}

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
     * The Main loop - calls pre/post frame hooks and runs core program logic.
     */
    void run() {
        program->start();

        while (running && onPreFrame()) {
            frameClock.updateLap();
            float dt = frameClock.getDeltaTime();
          
            trackFPS(dt);

            program->update(dt);

            onRender();

            onPostFrame();
        }

        program->end();
    }
};

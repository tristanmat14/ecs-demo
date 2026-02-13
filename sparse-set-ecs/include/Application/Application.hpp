#pragma once

#include <iostream>
#include <memory>

#include "Clock.hpp"
#include "ICore.hpp"
#include "IRenderer.hpp"

class Application {
private:
    std::unique_ptr<ICore> m_core;
    Clock m_frameClock;
    bool m_running;

public:
    virtual ~Application() = default;
    explicit Application(std::unique_ptr<ICore> core)
        : m_core(std::move(core)) {}

    /**
     * Runs once in the Application::start() method. 
     * Used to initialize resources and application states.
     */
    virtual bool onStart() = 0;

    /**
     * Runs once in the Application::end() method.
     * Used to clean the application resources.
     */
    virtual void onEnd() = 0;

    /**
     * Runs once every frame.
     * Contains the core application logic.
     */
    virtual void onUpdate(float dt) = 0;

    /**
     * Runs once every frame after the onUpdate method.
     * Contains the rendering code.
     */
    virtual void onRender() = 0;

    /**
     * Called from the program entry point.
     */
    bool start() {
        if (!m_core->initialize()) {
            m_running = false;
            return false;
        }

        if (!onStart()) {
            m_running = false;
            return false;
        }

        // Reset frameClock after calling onStart() so the initial time is more accurate.
        m_frameClock.reset();

        m_running = true;
        return true;
    }
   
    /**
     * Called from the program entry point.
     */
    void end() {
        onEnd();
        m_core->shutdown();
    }
    
    /**
     * The main loop - calls pre/post frame hooks and runs the main application logic.
     * Called from the program entry point.
     */
    void run() {
        while (m_running && m_core->onPreFrame()) {
            m_frameClock.updateLap();
            float dt = std::clamp(m_frameClock.getDeltaTime(), 0.0f, 0.1f);

            onUpdate(dt);
            onRender();

            trackFPS(dt);

            m_core->onPostFrame();
        }
    }

    void stop() {
        m_running = false;
    }

    IRenderer& getRenderer() {
        return m_core->getRenderer();
    }

private:
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

};

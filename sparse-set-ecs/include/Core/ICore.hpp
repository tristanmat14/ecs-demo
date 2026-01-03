#pragma once

#include "IRenderer.hpp"

struct ICore {
    virtual ~ICore() = default;

    /**
     * Perform OS-specific initialization.
     * Returns false to abort startup.
     */
    virtual bool initialize() = 0; 
    
    /**
     * Perform OS-specific cleanup.
     */
    virtual void shutdown() = 0;

    /**
     * Called once before each frame.
     * Handle OS-specific events here.
     * Returns false to stop the application.
     */
    virtual bool onPreFrame() = 0;

    /**
     * Called once after each frame.
     * Handle OS-specific presentation here.
     */
    virtual void onPostFrame() = 0;

    virtual IRenderer& getRenderer() = 0;
};

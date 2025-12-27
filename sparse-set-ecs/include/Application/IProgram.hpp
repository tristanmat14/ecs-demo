#pragma once

class IProgram {
public:
    virtual ~IProgram() = default;
    //IProgram(const IProgram&) = delete;
    //IProgram& operator=(const IProgram&) = delete;

    /**
     * Called once when the program starts.
     * Initialize the program state here.
     */
    virtual void start() = 0;

    /**
     * Called every frame.
     * Update game logic, physics, etc.
     */
    virtual void update(float dt) = 0;

    /**
     * Called once when the main loop ends.
     * Clean up resources here.
     */
    virtual void end() = 0;
};

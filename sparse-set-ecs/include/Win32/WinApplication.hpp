#pragma once

#include <iostream>
#include <memory>
#include <cmath>

#include "Application.hpp"
#include "Clock.hpp"
#include "Window.hpp"
#include "Color.hpp"

// Temporary:
#include "EntityComponentManager.hpp"
#include "EntityWrapper.hpp"
#include "LifetimeSystem.hpp"
#include "MovementSystem.hpp"
#include "RenderSystem.hpp"

static inline void initEntities();

class WinApplication : public Application {
private:
    std::unique_ptr<Window> window;

public:
    WinApplication() = default; 
    WinApplication(const WinApplication&) = delete;
    WinApplication& operator=(const WinApplication&) = delete;

protected:    
    bool onStart() override {
        try {
            window = std::make_unique<Window>(800, 600, "C++ Window");
        } catch (const std::exception& e) {
            MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
            return false;
        }

        // setup ECS
        initEntities(); 

        return true; 
    }

    void onEnd() override {
        std::cout << "Ending program" << std::endl;
        window.reset();
    }
   
    bool onPreFrame() override {
        window->clearScreen(Color{30, 30, 30});
        return window->processMessages();
    }

    void onPostFrame() override {
        window->redraw();
    }

    void runFrame() {
        auto& ecm = EntityComponentManager::getInstance();
        
        auto& lifetimePool = ecm.getPool<LifetimeComponent>();
        auto& positionPool = ecm.getPool<PositionComponent>();
        auto& velocityPool = ecm.getPool<VelocityComponent>();

        // run systems
        lifetimeSystem(lifetimePool, ecm.entityRemover, frameClock.getDeltaTime());
        movementSystem(positionPool, velocityPool, frameClock.getDeltaTime());
        renderSystem(positionPool, *window);
        
        // deferred deletion of entities
        ecm.deleteEntities();
    }
};

static inline void initEntities() {
    auto& ecm = EntityComponentManager::getInstance();

    // static dot
    auto staticDot = EntityWrapper{ecm.createEntity()};
    staticDot.addComponent(PositionComponent{5.1f, 5.0f});
    staticDot.addComponent(LifetimeComponent{100.0f});

    // moving dot
    auto movingDot = EntityWrapper{ecm.createEntity()};
    movingDot.addComponent(PositionComponent{400.0f, 300.0f});
    movingDot.addComponent(VelocityComponent{0.0f, 1.0f});

    // slow moving dot
    auto slowMovingDot = EntityWrapper{ecm.createEntity()};
    slowMovingDot.addComponent(PositionComponent{0.0f, 800.0f});
    slowMovingDot.addComponent(VelocityComponent{3.0f, -3.0f});

    // fast moving dot
    auto fastMovingDot = EntityWrapper{ecm.createEntity()};
    fastMovingDot.addComponent(PositionComponent{600.0f, 800.0f});
    fastMovingDot.addComponent(VelocityComponent{-20.0f, -13.0f});

    // invisible mover
    //auto invisibleMover = EntityWrapper{ecm.createEntity()};
    //invisibleMover.addComponent(PositionComponent{2.3f, 3.2f});
    //invisibleMover.addComponent(VelocityComponent{1.0f, 1.0f});
}
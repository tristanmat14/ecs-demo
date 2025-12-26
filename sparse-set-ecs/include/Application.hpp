#pragma once

#include "EntityComponentManager.hpp"
#include "EntityWrapper.hpp"
#include "MovementSystem.hpp"
#include "TextGridRenderSystem.hpp"
#include "TextRenderSystem.hpp"
#include "LifetimeSystem.hpp"
#include "BoundedCollisionSystem.hpp"

#include "Clock.hpp"
#include "Utilities.hpp"

static inline void initEntities(EntityComponentManager& ecm);

class Application {
private:
    Clock frameClock;    

    EntityComponentManager& ecm;
   
    // -1 means first (0th) frame hasn't run yet.
    uint32_t frameCounter = -1; 

public:
    Application() : ecm(EntityComponentManager::getInstance()) {}

    void start() {
        initEntities(ecm);
        
        frameClock.reset();
    }

    void runFrame() {
        overwriteTerminal();

        ++frameCounter;
        frameClock.update();
        
        std::cout << "Frame number: " << frameCounter << " | Time (s): " << frameClock.getCurrentTime() << std::endl;

        auto& lifetimePool = ecm.getPool<LifetimeComponent>();
        auto& positionPool = ecm.getPool<PositionComponent>();
        auto& velocityPool = ecm.getPool<VelocityComponent>();
        auto& boundedCollisionPool = ecm.getPool<BoundedCollisionComponent>();
        auto& textRenderPool = ecm.getPool<TextRenderComponent>();

        // run systems
        lifetimeSystem(lifetimePool, ecm.entityRemover, frameClock.getDeltaTime());
        movementSystem(positionPool, velocityPool, frameClock.getDeltaTime());
        boundedCollisionSystem(boundedCollisionPool, positionPool, velocityPool);
        textGridRenderSystem(textRenderPool, positionPool);

        // deferred deletion of entities
        ecm.deleteEntities();

        delay(10);
    }

    void end() {

    }

    int getFrameCount() {
        return frameCounter;
    }
};

static inline void initEntities(EntityComponentManager& ecm) {
    // static dot
    auto staticDot = EntityWrapper{ecm.createEntity()};

    staticDot.addComponent(PositionComponent{5.1f, 5.0f});
    staticDot.addComponent(TextRenderComponent{'s'});
    staticDot.addComponent(LifetimeComponent{4.0f});

    auto oSymbol = TextRenderComponent{'o'};

    // moving dot
    auto movingDot = EntityWrapper{ecm.createEntity()};
    movingDot.addComponent(PositionComponent{0.0f, 0.0f});
    movingDot.addComponent(VelocityComponent{0.0f, 1.0f});
    movingDot.addComponent(oSymbol);
    movingDot.addComponent(BoundedCollisionComponent{-3.0f, -2.0f, -5.0f, 5.0f});

    // slow moving dot
    auto slowMovingDot = EntityWrapper{ecm.createEntity()};
    slowMovingDot.addComponent(PositionComponent{-11.0f, -11.0f});
    slowMovingDot.addComponent(VelocityComponent{0.5f, 0.5f});
    slowMovingDot.addComponent(oSymbol);

    // fast moving dot
    auto fastMovingDot = EntityWrapper{ecm.createEntity()};
    fastMovingDot.addComponent(PositionComponent{6.0f, 7.0f});
    fastMovingDot.addComponent(VelocityComponent{-2.0f, -3.0f});
    fastMovingDot.addComponent(oSymbol);
    fastMovingDot.addComponent(BoundedCollisionComponent{-10.0f, 10.0f, -10.0f, 10.0f});

    // invisible mover
    auto invisibleMover = EntityWrapper{ecm.createEntity()};
    invisibleMover.addComponent(PositionComponent{2.3f, 3.2f});
    invisibleMover.addComponent(VelocityComponent{1.0f, 1.0f});

    // renderable symbol with no position
    auto renderable = EntityWrapper{ecm.createEntity()};
    renderable.addComponent(TextRenderComponent{'r'});
    renderable.addComponent(LifetimeComponent{2.0f});
}

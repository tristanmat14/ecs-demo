#pragma once

#include "IProgram.hpp"

#include "EntityComponentManager.hpp"
#include "EntityWrapper.hpp"
#include "LifetimeSystem.hpp"
#include "MovementSystem.hpp"

class ECSProgram : public IProgram {
public:
    EntityComponentManager& ecm = EntityComponentManager::getInstance(); 

    void start() override {
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
        auto invisibleMover = EntityWrapper{ecm.createEntity()};
        invisibleMover.addComponent(PositionComponent{2.3f, 3.2f});
        invisibleMover.addComponent(VelocityComponent{1.0f, 1.0f});
    } 
    
    void update(float dt) override {
        auto& lifetimePool = ecm.getPool<LifetimeComponent>();
        auto& positionPool = ecm.getPool<PositionComponent>();
        auto& velocityPool = ecm.getPool<VelocityComponent>();

        // run systems
        lifetimeSystem(lifetimePool, ecm.entityRemover, dt);
        movementSystem(positionPool, velocityPool, dt);
        
        // deferred deletion of entities
        ecm.deleteEntities();
    }

    void end() override {}
};

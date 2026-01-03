#pragma once

#include "Application.hpp"

#include "EntityComponentManager.hpp"
#include "EntityWrapper.hpp"
#include "LifetimeSystem.hpp"
#include "MovementSystem.hpp"

class ECSApplication final : public Application {
public:
    using Application::Application;

    EntityComponentManager& ecm = EntityComponentManager::getInstance(); 

    bool onStart() override {
        getRenderer().setCameraSpace(10.0f, -10.0f, -10.0f, 10.0f);

        // static dot
        auto staticDot = EntityWrapper{ecm.createEntity()};
        staticDot.addComponent(PositionComponent{5.1f, 5.0f});
        staticDot.addComponent(LifetimeComponent{10.0f});

        // moving dot
        auto movingDot = EntityWrapper{ecm.createEntity()};
        movingDot.addComponent(PositionComponent{4.0f, -3.0f});
        movingDot.addComponent(VelocityComponent{0.0f, 1.0f});

        // slow moving dot
        auto slowMovingDot = EntityWrapper{ecm.createEntity()};
        slowMovingDot.addComponent(PositionComponent{-10.0f, 8.0f});
        slowMovingDot.addComponent(VelocityComponent{0.3f, -0.3f});

        // fast moving dot
        auto fastMovingDot = EntityWrapper{ecm.createEntity()};
        fastMovingDot.addComponent(PositionComponent{10.0f, 10.0f});
        fastMovingDot.addComponent(VelocityComponent{-1.6f, -0.3f});
    
        return true;
    } 
    
    void onUpdate(float dt) override {
        auto& lifetimePool = ecm.getPool<LifetimeComponent>();
        auto& positionPool = ecm.getPool<PositionComponent>();
        auto& velocityPool = ecm.getPool<VelocityComponent>();

        // run systems
        lifetimeSystem(lifetimePool, ecm.entityRemover, dt);
        movementSystem(positionPool, velocityPool, dt);
        
        // deferred deletion of entities
        ecm.deleteEntities();
    }

    void onRender() override {
        auto& renderer = getRenderer();

        renderer.clearScreen(Color{30, 30, 30});

        auto& positionPool = ecm.getPool<PositionComponent>();
        auto& velocityPool = ecm.getPool<VelocityComponent>();

        for (std::size_t i = 0; i < positionPool.getSize(); ++i) {
            Entity e = positionPool.entities[i];
            auto& position = positionPool.data[i];
            
            renderer.drawCircle(position.x, position.y, 5.0f, Color{0, 100, 250});            

            if (velocityPool.has(e)) {
                auto& velocity = velocityPool.get(e);
                
                renderer.drawLine(
                    position.x,
                    position.y,
                    position.x + velocity.x,
                    position.y + velocity.y,
                    Color{255, 0, 0}
                );
            }
        }
    }

    void onEnd() override {}
};

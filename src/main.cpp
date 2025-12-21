#include <unordered_map>
#include <vector>
#include <thread>
#include <chrono>

#include "EntityComponentManager.hpp"

#include "MovementSystem.hpp"
#include "TextGridRenderSystem.hpp"
#include "TextRenderSystem.hpp"
#include "LifetimeSystem.hpp"
#include "DebugSystem.hpp"

#include "Clock.hpp"
#include "Utilities.hpp"

static constexpr float SECONDS_PER_FRAME = 0.1f;

int main(int argc, char* argv[]) {
    Clock frameClock;
    Clock displayClock; 
    auto& ecm = EntityComponentManager::getInstance();

    ComponentPool<PositionComponent> test;

    // static dot
    auto newEntity = ecm.createEntity();
    ecm.positionPool.add(newEntity, PositionComponent{5.0f, 5.0f});
    ecm.textRenderPool.add(newEntity, TextRenderComponent{'o'});
    ecm.lifetimePool.add(newEntity, LifetimeComponent{4.0f});

    // moving dot
    newEntity = ecm.createEntity();
    ecm.positionPool.add(newEntity, PositionComponent{0.0f, 0.0f});
    ecm.velocityPool.add(newEntity, VelocityComponent{0.0f, 1.0f});
    ecm.textRenderPool.add(newEntity, TextRenderComponent{'o'});

    // slow moving dot
    newEntity = ecm.createEntity();
    ecm.positionPool.add(newEntity, PositionComponent{-11.0f, -11.0f});
    ecm.velocityPool.add(newEntity, VelocityComponent{0.5f, 0.5f});
    ecm.textRenderPool.add(newEntity, TextRenderComponent{'o'});

    // fast moving dot
    newEntity = ecm.createEntity();
    ecm.positionPool.add(newEntity, PositionComponent{6.0f, 7.0f});
    ecm.velocityPool.add(newEntity, VelocityComponent{-1.0f, -2.0f});
    ecm.textRenderPool.add(newEntity, TextRenderComponent{'o'});

    // invisible mover
    newEntity = ecm.createEntity();
    ecm.positionPool.add(newEntity, PositionComponent{2.3f, 3.2f});
    ecm.velocityPool.add(newEntity, VelocityComponent{1.0f, 1.0f});
  
    // renderable symbol with no position
    newEntity = ecm.createEntity();
    ecm.textRenderPool.add(newEntity, TextRenderComponent{'r'});
    ecm.lifetimePool.add(newEntity, LifetimeComponent{2.0f});

    clearTerminal();
    frameClock.reset();
    displayClock.reset();

    // engine loop
    for (int frame = 0; frame < 100000; frame++) {
        frameClock.update();
        displayClock.update();
        
        // run systems
        lifetimeSystem(ecm.lifetimePool, ecm.entityRemover, frameClock.getDeltaTime());
        movementSystem(ecm.positionPool, ecm.velocityPool, frameClock.getDeltaTime());

        if (displayClock.getCurrentTime() >= SECONDS_PER_FRAME) {
            displayClock.reset(); 
            overwriteTerminal();
            
            std::cout << "Frame number: " << frame << " | Time (s): " << frameClock.getCurrentTime() << std::endl;

            //textRenderSystem(ecm.textRenderPool, ecm.positionPool);
            textGridRenderSystem(ecm.textRenderPool, ecm.positionPool);
        }

        // deferred deletion of entities
        ecm.deleteEntities();

        delay(10);
    }

    return 0;
}

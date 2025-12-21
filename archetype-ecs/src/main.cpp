#include <unordered_map>
#include <vector>

#include "EntityComponentManager.hpp"

#include "MovementSystem.hpp"
#include "TextRenderSystem.hpp"
#include "LifetimeSystem.hpp"
#include "DebugSystem.hpp"

int main(int argc, char* argv[]) {
    auto& ecm = EntityComponentManager::getInstance();

    // static dot
    auto newEntity = ecm.createEntity();
    ecm.positionPool.add(newEntity, PositionComponent{5.0f, 5.0f});
    ecm.textRenderPool.add(newEntity, TextRenderComponent{'s'});
    ecm.lifetimePool.add(newEntity, LifetimeComponent{4});

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
    ecm.velocityPool.add(newEntity, VelocityComponent{-2.0f, -3.0f});
    ecm.textRenderPool.add(newEntity, TextRenderComponent{'o'});

    // invisible mover
    newEntity = ecm.createEntity();
    ecm.positionPool.add(newEntity, PositionComponent{2.3f, 3.2f});
    ecm.velocityPool.add(newEntity, VelocityComponent{1.0f, 1.0f});
  
    // renderable symbol with no position
    newEntity = ecm.createEntity();
    ecm.textRenderPool.add(newEntity, TextRenderComponent{'r'});
    ecm.lifetimePool.add(newEntity, LifetimeComponent{2});

    // engine loop
    for (int frame = 0; frame < 3; frame++) {
        std::cout << "Frame number: " << frame << std::endl; 
        
        // run systems
        lifetimeSystem(ecm.lifetimePool, ecm.entityRemover);
        movementSystem(ecm.positionPool, ecm.velocityPool);
        textRenderSystem(ecm.textRenderPool, ecm.positionPool);

        // deferred deletion of entities
        ecm.deleteEntities();
    }

    return 0;
}

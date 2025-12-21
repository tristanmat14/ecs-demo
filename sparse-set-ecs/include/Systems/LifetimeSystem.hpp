#pragma once

#include "ComponentPool.hpp"
#include "LifetimeComponent.hpp"

void lifetimeSystem(
    ComponentPool<LifetimeComponent>& lifetimePool,
    EntityComponentManager::EntityRemover& entityRemover,
    float deltaTime
) {
    for (std::size_t i = 0; i < lifetimePool.getSize(); ++i) {
        Entity e = lifetimePool.entities[i];
        
        auto& lifetime = lifetimePool.data[i];
        lifetime.seconds_left -= deltaTime;

        if (lifetime.seconds_left <= 0.0f) {
            entityRemover.add(e);
        }
    }
}
#pragma once

#include "ComponentPool.hpp"
#include "LifetimeComponent.hpp"

void lifetimeSystem(
    ComponentPool<LifetimeComponent>& lifetimePool,
    EntityComponentManager::EntityRemover& entityRemover
) {
    for (std::size_t i = 0; i < lifetimePool.getSize(); ++i) {
        Entity e = lifetimePool.entities[i];
        
        auto& lifetime = lifetimePool.data[i];
        lifetime.frames_left -= 1;

        if (lifetime.frames_left <= 0) {
            entityRemover.add(e);
        }
    }
}
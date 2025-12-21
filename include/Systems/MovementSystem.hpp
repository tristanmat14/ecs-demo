#pragma once

#include "ComponentPool.hpp"
#include "PositionComponent.hpp"
#include "VelocityComponent.hpp"

void movementSystem(
    ComponentPool<PositionComponent>& positionPool,
    ComponentPool<VelocityComponent>& velocityPool,
    float deltaTime
) {
    for (std::size_t i = 0; i < velocityPool.getSize(); ++i) {
        Entity e = velocityPool.entities[i];

        if (!positionPool.has(e)) continue;

        auto& position = positionPool.get(e);
        auto& velocity = velocityPool.data[i];

        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
    }
}
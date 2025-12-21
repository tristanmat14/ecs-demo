#pragma once

#include "ComponentPool.hpp"
#include "BoundedCollisionComponent.hpp"
#include "PositionComponent.hpp"
#include "VelocityComponent.hpp"

void boundedCollisionSystem(
    ComponentPool<BoundedCollisionComponent>& collisionPool,
    ComponentPool<PositionComponent>& positionPool,
    ComponentPool<VelocityComponent>& velocityPool
) {
    for (std::size_t i = 0; i < collisionPool.getSize(); ++i) {
        Entity e = collisionPool.entities[i];

        if (!positionPool.has(e)) continue;

        auto& bounds = collisionPool.data[i];
        auto& position = positionPool.get(e);
        
        bool x_out_of_bounds = true;
        bool y_out_of_bounds = true;

        if (position.x < bounds.xMin) {
            position.x = bounds.xMin;
        } else if (position.x > bounds.xMax) {
            position.x = bounds.xMax;
        } else {
            x_out_of_bounds = false;
        }

        if (position.y < bounds.yMin) {
            position.y = bounds.yMin;
        } else if (position.y > bounds.yMax) {
            position.y = bounds.yMax;
        } else {
            y_out_of_bounds = false;
        }

        if (!velocityPool.has(e)) continue;

        auto& velocity = velocityPool.get(e);
        if (x_out_of_bounds) {
            velocity.x *= -1.0f;
        }
        if (y_out_of_bounds) {
            velocity.y *= -1.0f;
        }
    }
}
#pragma once

#include "ComponentPool.hpp"
#include "PositionComponent.hpp"
#include "Window.hpp"

void renderSystem(
    ComponentPool<PositionComponent>& positionPool,
    Window& window
) { 
    for (std::size_t i = 0; i < positionPool.getSize(); ++i) {
        Entity e = positionPool.entities[i];
        auto& position = positionPool.get(e);
        
        window.drawCircle(position.x, position.y, 10.0f, Color{112, 224, 155});
    }
}
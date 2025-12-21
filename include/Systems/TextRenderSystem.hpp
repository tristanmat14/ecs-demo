#pragma once

#include <iostream>

#include "ComponentPool.hpp"
#include "TextRenderComponent.hpp"
#include "PositionComponent.hpp"

void textRenderSystem(
    ComponentPool<TextRenderComponent>& renderPool,
    ComponentPool<PositionComponent>& positionPool
) {
    for (std::size_t i = 0; i < renderPool.getSize(); ++i) {
        Entity e = renderPool.entities[i];

        auto& render = renderPool.data[i];

        std::cout << render << ' ';

        if (positionPool.has(e)) {
            auto& position = positionPool.get(e);
            std::cout << position;
        }

        std::cout << '\n';
    }
}
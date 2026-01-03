#pragma once

#include <iostream>

#include "IComponent.hpp"
#include "ComponentID.hpp"

struct PositionComponent final : public IComponent<PositionComponent> {
    static ComponentID typeId() {
        return ComponentID::Position;
    }

    float x, y;

    PositionComponent(float x_, float y_) : x(x_), y(y_) {}
};
    
std::ostream& operator<<(std::ostream& os, const PositionComponent& c) {
    os << '(' << c.x << ',' << c.y << ')';
    return os;
}

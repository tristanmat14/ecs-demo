#pragma once

#include <iostream>

#include "IComponent.hpp"
#include "ComponentID.hpp"

struct VelocityComponent final : IComponent<VelocityComponent> {
    static ComponentID typeId() {
        return ComponentID::Velocity;
    }

    float x, y;

    VelocityComponent(float x_, float y_) : x(x_), y(y_) {}
};

std::ostream& operator<<(std::ostream& os, const VelocityComponent& c) {
    os << '(' << c.x << ',' << c.y << ')';
    return os;
}

#pragma once

#include <iostream>

#include "IComponent.hpp"
#include "ComponentID.hpp"

struct LifetimeComponent : public IComponent<LifetimeComponent> {
    static ComponentID typeId() {
        return ComponentID::Lifetime;
    }

    float seconds_left;

    LifetimeComponent(float seconds_left_) : seconds_left(seconds_left_) {}
};

std::ostream& operator<<(std::ostream& os, const LifetimeComponent c) {
    os << c.seconds_left;
    return os;
}

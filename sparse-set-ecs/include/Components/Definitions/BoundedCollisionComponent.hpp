#pragma once

#include <iostream>

#include "IComponent.hpp"
#include "ComponentID.hpp"

struct BoundedCollisionComponent : public IComponent<BoundedCollisionComponent> {
    static ComponentID typeId() {
        return ComponentID::BoundedCollision;
    }

    float xMin;
    float xMax;
    float yMin;
    float yMax;

    BoundedCollisionComponent(float xMin_, float xMax_, float yMin_, float yMax_)
        : xMin(xMin_), xMax(xMax_), yMin(yMin_), yMax(yMax_) {}
};

std::ostream& operator<<(std::ostream& os, const BoundedCollisionComponent& c) {
    os << "(xLimits[" << c.xMin << ',' << c.xMax << "], yLimits[" << c.yMin << ',' << c.yMax << "])";
    return os;
}
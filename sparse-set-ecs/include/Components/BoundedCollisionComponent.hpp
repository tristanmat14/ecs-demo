#pragma once

#include <iostream>

struct BoundedCollisionComponent {
    float xMin;
    float xMax;
    float yMin;
    float yMax;
};

std::ostream& operator<<(std::ostream& os, const BoundedCollisionComponent& c) {
    os << "(xLimits[" << c.xMin << ',' << c.xMax << "], yLimits[" << c.yMin << ',' << c.yMax << "])";
    return os;
}
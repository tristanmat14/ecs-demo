#pragma once

#include <iostream>

struct VelocityComponent {
    float x, y;
};

std::ostream& operator<<(std::ostream& os, const VelocityComponent& c) {
    os << '(' << c.x << ',' << c.y << ')';
    return os;
}

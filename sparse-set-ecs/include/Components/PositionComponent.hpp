#pragma once

#include <iostream>

struct PositionComponent {
    float x, y;
};
    
std::ostream& operator<<(std::ostream& os, const PositionComponent& c) {
    os << '(' << c.x << ',' << c.y << ')';
    return os;
}

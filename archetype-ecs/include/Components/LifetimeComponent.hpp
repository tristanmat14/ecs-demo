#pragma once

#include <iostream>

struct LifetimeComponent {
    int frames_left;
};

std::ostream& operator<<(std::ostream& os, const LifetimeComponent c) {
    os << c.frames_left;
    return os;
}

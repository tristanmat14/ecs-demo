#pragma once

#include <iostream>

struct LifetimeComponent {
    float seconds_left;
};

std::ostream& operator<<(std::ostream& os, const LifetimeComponent c) {
    os << c.seconds_left;
    return os;
}

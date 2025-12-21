#pragma once

#include <iostream>

struct TextRenderComponent {
    char symbol;
};

std::ostream& operator<<(std::ostream& os, const TextRenderComponent c) {
    os << c.symbol;
    return os;
}

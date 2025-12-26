#pragma once

#include <iostream>

#include "IComponent.hpp"
#include "ComponentID.hpp"

struct TextRenderComponent : public IComponent<TextRenderComponent> {
    static ComponentID typeId() {
        return ComponentID::TextRender;
    }

    char symbol;

    TextRenderComponent(char symbol_) : symbol(symbol_) {}
};

std::ostream& operator<<(std::ostream& os, const TextRenderComponent c) {
    os << c.symbol;
    return os;
}

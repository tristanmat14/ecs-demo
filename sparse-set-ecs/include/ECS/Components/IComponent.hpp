#pragma once

#include <concepts>

#include "ComponentID.hpp"

template<typename Derived>
struct IComponent {
    // This follows the Curiously Repeating Template Pattern (CRTP).
    // This allows the use of compile time polymorphism.
    static ComponentID typeId() {
        return Derived::typeId();
    }
};

template<typename T>
concept ComponentConcept = std::derived_from<T, IComponent<T>>;

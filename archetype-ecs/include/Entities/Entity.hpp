#pragma once

#include <cstdint>

struct Entity {
    uint32_t id;

    bool operator==(const Entity& other) const {
        return id == other.id;
    }
};

namespace std {
    template<>
    struct hash<Entity> {
        std::size_t operator()(const Entity& e) const noexcept {
            return std::hash<std::uint32_t>{}(e.id);
        }
    };
}

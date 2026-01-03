#pragma once

#include <cstdint>

#include "IComponent.hpp"

using EntityID = uint32_t;

class Entity {
private:
    EntityID id;

public:
    Entity(EntityID id_) : id(id_) {}

    EntityID getId() const {
        return id;
    }

    bool operator==(const Entity& other) const {
        return id == other.id;
    }
};

template<>
struct std::hash<Entity> {
    std::size_t operator()(const Entity& e) const noexcept {
        return std::hash<EntityID>{}(e.getId());
    }
};

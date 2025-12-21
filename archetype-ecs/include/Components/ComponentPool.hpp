#pragma once

#include <vector>
#include <unordered_map>

#include "Entity.hpp"

template<typename Component>
struct ComponentPool {
    std::vector<Component> data;
    std::vector<Entity> entities;
    std::unordered_map<Entity, std::size_t> lookup;

    std::size_t getSize() const {
        return this->entities.size();
    }

    bool has(Entity e) const {
        return this->lookup.contains(e);
    }

    void add(Entity e, Component c) {
        if (this->has(e)) {
            return;
        }

        std::size_t index = this->getSize();
        this->lookup[e] = index;
        this->data.push_back(std::move(c));
        this->entities.push_back(e);
    }

    void remove(Entity e) {
        if (!this->has(e)) {
            return;
        }

        std::size_t index = this->lookup[e];
        std::size_t last_index = this->getSize() - 1;

        Entity last_entity = this->entities[last_index];

        // Move last element into removed slot
        this->entities[index] = last_entity; 
        this->data[index] = std::move(this->data[last_index]);

        // Update lookup map
        this->lookup[last_entity] = index;
        this->lookup.erase(e);

        // Remove tail 
        this->data.pop_back();
        this->entities.pop_back();
    }

    /**
     * @precondition: the entity exists in the pool and has been checked with the .has(Entity) method
     */
    Component& get(Entity e) {
        return this->data[this->lookup[e]];
    }
};

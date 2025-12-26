#pragma once

#include <vector>
#include <unordered_map>
#include <concepts>
#include <cassert>

#include "EntityComponentManager.hpp"
#include "IComponent.hpp"
#include "Entity.hpp"

/**
 * Stores a list of entities and components such that they correspond via a common index.
 * 
 * e.g.
 * entities -> [0, 1, 4, 7, 3, 9]
 * components -> [a, c, d, k, l, p]
 * 
 * entity 0 has component a, entity 1 has component c, etc.
 *
 * If an entity doesn't have a component, then it isn't in the corresponding component pool.
 * If an entity has a component, then it is in the corresponding component pool.
 */
template<ComponentConcept Component>
struct ComponentPool : public IComponentPool {
    std::vector<Component> data;
    std::vector<Entity> entities;
    std::unordered_map<Entity, std::size_t> lookup;

    ComponentPool(std::size_t reserve = 0) {
        data.reserve(reserve);
        entities.reserve(reserve);
    }

    std::size_t getSize() const {
        return this->entities.size();
    }

    bool has(Entity e) const {
        return this->lookup.contains(e);
    }
    
    /**
     * @precondition: the entity has the relavent component (i.e. its in the component pool).
     *                This can be checked with the .has(Entity) method.
     */
    Component& get(Entity e) {
        assert(this->has(e) && "ComponentPool.get(Entity) precondition violated");
        return this->data[this->lookup[e]];
    }

    // TODO: remove the add() and remove() methods from the public interface.
    //       This is because a higher level interface should be in charge of adding
    //       and removing components.
    void add(Entity e, Component& c) {
        if (this->has(e)) {
            return;
        }

        std::size_t index = this->getSize();
        this->lookup[e] = index;
        this->data.push_back(std::move(c));
        this->entities.push_back(e);
    }

    void remove(Entity e) {
        auto& it = this->lookup.find(e);
        if (it == this->lookup.end()) return;

        std::size_t entity_index = it->second;
        std::size_t last_index = this->getSize() - 1;

        Entity last_entity = this->entities[last_index];

        // Move last element into removed slot
        this->entities[entity_index] = last_entity; 
        this->data[entity_index] = std::move(this->data[last_index]);

        // Update lookup map
        this->lookup[last_entity] = entity_index;
        this->lookup.erase(e);

        // Remove tail 
        this->data.pop_back();
        this->entities.pop_back();
    }
};

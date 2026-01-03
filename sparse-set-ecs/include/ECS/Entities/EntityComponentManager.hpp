#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>

#include "IComponent.hpp"
#include "IComponentPool.hpp"
#include "ComponentPool.hpp"
#include "Entity.hpp"

constexpr std::size_t DEFAULT_CAPACITY = 20;

class EntityComponentManager {
public:
    class EntityRemover {
    private:
        std::queue<Entity> deleteQueue;
        EntityRemover() = default;
        friend class EntityComponentManager;
    public:
        void add(Entity e) {
            deleteQueue.push(e);
        }
    };

    /*
    class EntityCreator {
    private:
        std::queue<Entity> createQueue;
        EntityCreator() = default;
        friend class EntityComponentManager;
    public:
        // takes a list of components
        // make sure the lists are copied so we fully own them
        void add() {
            // adds entity with the components
        }
    };
    */

public:
    EntityRemover entityRemover;

private:
    EntityID nextId = 0;
    std::vector<Entity> entities;
    std::unordered_map<ComponentID, std::unique_ptr<IComponentPool>> componentPools;

public:
    static EntityComponentManager& getInstance() {
        static EntityComponentManager instance;
        return instance;
    }
    
    // ---------------------------------------------------
    // Entity Management
    // ---------------------------------------------------
    Entity createEntity() {
        auto entity = Entity{nextId++};
        entities.push_back(entity);
        return entity;
    }

    void deleteEntity(Entity e) {
        for (auto& [_, componentPool] : componentPools) {
            componentPool->remove(e);
        }
        
        auto it = std::find(entities.begin(), entities.end(), e);
        if (it != entities.end()) {
            *it = std::move(entities.back());
            entities.pop_back();
        }
    }

    void deleteEntities() {
        while (!entityRemover.deleteQueue.empty()) {
            auto e = entityRemover.deleteQueue.front();
            entityRemover.deleteQueue.pop();
            deleteEntity(e);
        }
    }

    const std::vector<Entity>& getEntities() const {
        return entities;
    }

    // ---------------------------------------------------
    // Component Management
    // ---------------------------------------------------
    template<ComponentConcept Component>
    void addComponent(Entity e, Component c) {
        ComponentPool<Component>& pool = this->getPool<Component>();
        pool.add(e, c);
    }

    template<ComponentConcept Component>
    void removeComponent(Entity e) {
        ComponentPool<Component>& pool = this->getPool<Component>();
        pool.remove(e);
    }

    template<ComponentConcept Component>
    ComponentPool<Component>& getPool() {
        auto typeId = Component::typeId();

        if (!componentPools.contains(typeId)) {
            componentPools[typeId] = std::make_unique<ComponentPool<Component>>(DEFAULT_CAPACITY);
        }

        return *static_cast<ComponentPool<Component>*>(componentPools[typeId].get());
    }

private:
    EntityComponentManager() : entityRemover{} {};

    EntityComponentManager(const EntityComponentManager&) = delete;
    EntityComponentManager& operator=(const EntityComponentManager&) = delete;
    EntityComponentManager(EntityComponentManager&&) = delete;
    EntityComponentManager& operator=(const EntityComponentManager&&) = delete;
};

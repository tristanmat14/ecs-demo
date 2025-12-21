#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <queue>

#include "Entity.hpp"
#include "ComponentPool.hpp"
#include "PositionComponent.hpp"
#include "TextRenderComponent.hpp"
#include "VelocityComponent.hpp"
#include "LifetimeComponent.hpp"

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

private:
    EntityComponentManager() : entityRemover{} {};
    
    EntityComponentManager(const EntityComponentManager&) = delete;
    EntityComponentManager& operator=(const EntityComponentManager&) = delete;
    EntityComponentManager(EntityComponentManager&&) = delete;
    EntityComponentManager& operator=(EntityComponentManager&&) = delete;
    
    uint32_t nextId = 0;

    std::vector<Entity> entities;

    void deleteEntity(Entity e) {
        positionPool.remove(e);
        velocityPool.remove(e);
        textRenderPool.remove(e);
        lifetimePool.remove(e);
        std::erase(entities, e);
    }

public:
    static EntityComponentManager& getInstance() {
        static EntityComponentManager instance;
        return instance;
    }
    
    Entity createEntity() {
        auto e = Entity{nextId++};
        entities.push_back(e);
        return e;
    }

    void deleteEntities() {
        while (!entityRemover.deleteQueue.empty()) {
            auto e = entityRemover.deleteQueue.front();
            entityRemover.deleteQueue.pop();
            deleteEntity(e);
        }
    }

    std::vector<Entity> const& getEntities() {
        return entities;
    }

    EntityRemover entityRemover;

    ComponentPool<PositionComponent> positionPool;
    ComponentPool<VelocityComponent> velocityPool;
    ComponentPool<TextRenderComponent> textRenderPool;
    ComponentPool<LifetimeComponent> lifetimePool;
};


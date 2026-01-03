#pragma once

#include "Entity.hpp"
#include "EntityComponentManager.hpp"
#include "IComponent.hpp"

class EntityWrapper {
private:
    Entity e;

public:
    explicit EntityWrapper(Entity e_) : e(e_) {}

    EntityID getId() {
        return e.getId();
    }
    
    template<ComponentConcept Component>
    void addComponent(Component c) {
        EntityComponentManager::getInstance().addComponent(this->e, c);
    }

    template<ComponentConcept Component>
    void removeComponent() {
        EntityComponentManager::getInstance().removeComponent<Component>(this->e);
    }
};
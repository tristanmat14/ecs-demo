#pragma once

#include "Entity.hpp"

/**
 * TODO: fix this interface such that it has public methods, and make remove and add private methods.
 * This is because external code should not remove or add directly, they need to use the ECM.
 * Make the ECM a friend
 */

struct IComponentPool {
    virtual ~IComponentPool() = default;
    virtual void remove(Entity e) = 0;
};

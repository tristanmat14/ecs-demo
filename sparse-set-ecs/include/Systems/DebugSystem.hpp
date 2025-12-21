#pragma once

#include <iostream>
#include <sstream>

#include "EntityComponentManager.hpp"

void debugSystem(
    EntityComponentManager& ecm,
    std::ostream& os = std::cerr
) {
    std::ostringstream out;

    out << "-----------------------------" << '\n';

    for (auto e : ecm.getEntities()) {
        out << "Entity ID: " << e.id << '\n';
        
        if (ecm.textRenderPool.has(e)) {
            auto& container = ecm.textRenderPool.get(e);
            out << "\tRender = " << container << '\n';
        }

        if (ecm.positionPool.has(e)) {
            auto& container = ecm.positionPool.get(e);
            out << "\tPosition = " << container << '\n';
        }

        if (ecm.velocityPool.has(e)) {
            auto& container = ecm.velocityPool.get(e);
            out << "\tVelocity = " << container << '\n';
        }

        if (ecm.lifetimePool.has(e)) {
            auto& container = ecm.lifetimePool.get(e);
            out << "\tLifetime = " << container << '\n';
        }

        out << '\n';
    }

    os << out.str() << "-----------------------------" << '\n';
}
#pragma once

#include <iostream>
#include <memory>
#include <cmath>

#include "Application.hpp"
#include "Clock.hpp"
#include "Window.hpp"
#include "Color.hpp"

class WinApplication : public Application {
private:
    std::unique_ptr<Window> window;

public:
    WinApplication() = default; 
    WinApplication(const WinApplication&) = delete;
    WinApplication& operator=(const WinApplication&) = delete;

protected:    
    bool onStart() override {
        try {
            window = std::make_unique<Window>(800, 600, "C++ Window");
        } catch (const std::exception& e) {
            MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
            return false;
        }

        return true; 
    }

    void onEnd() override {
        std::cout << "Ending program" << std::endl;
        window.reset();
    }
   
    bool onPreFrame() override {
        window->clearScreen(Color{30, 30, 30});
        return window->processMessages();
    }

    void onPostFrame() override {
        window->redraw();
    }

    void onRender() override {
        auto& ecm = EntityComponentManager::getInstance();
        auto& positionPool = ecm.getPool<PositionComponent>();

        for (std::size_t i = 0; i < positionPool.getSize(); ++i) {
            auto& position = positionPool.data[i];

            window->drawCircle(position.x, position.y, 5.0f, Color{206, 122, 230});
        }
    }
};

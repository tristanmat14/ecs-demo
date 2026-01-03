#pragma once

#include <iostream>
#include <memory>
#include <cmath>
#include <cassert>

#include "ICore.hpp"
#include "IRenderer.hpp"
#include "Clock.hpp"
#include "Window.hpp"
#include "Color.hpp"

class Win32Core final : public ICore {
private:
    class Renderer final : public IRenderer {
    private:
        std::unique_ptr<Window> m_window = nullptr;

        float m_top = 1.0f; 
        float m_bottom = -1.0f; 
        float m_left = -1.0f; 
        float m_right = 1.0f;

        void addWindow(std::unique_ptr<Window> window) {
            m_window = std::move(window);
        }
        
        int convertToPixelX(float x) {
            assert(m_window != nullptr);

            float width = m_window->getWidth();

            float gradient = width / (m_right - m_left);
            float offset = -gradient * m_left;
            float pixelX = gradient * x + offset;

            return static_cast<int>(pixelX);
        }

        int convertToPixelY(float y) {
            assert(m_window != nullptr);

            float height = m_window->getHeight();
            
            float gradient = -height / (m_top - m_bottom);
            float offset = -gradient * m_top;
            float pixelY = gradient * y + offset;

            return static_cast<int>(pixelY);
        }

        friend class Win32Core;

    public:
        Renderer() = default;

        void setCameraSpace(float top, float bottom, float left, float right) override {
            assert(m_window != nullptr);
            
            assert(top != bottom);
            assert(right != left);

            m_top = top;
            m_bottom = bottom;
            m_left = left;
            m_right = right;
        }

        void clearScreen(Color color) override {
            assert(m_window != nullptr);

            m_window->clearScreen(color);
        }
        
        void drawCircle(float centerX, float centerY, float radius, Color color) override {
            assert(m_window != nullptr);

            int pixelCenterX = convertToPixelX(centerX);
            int pixelCenterY = convertToPixelY(centerY);

            m_window->drawCircle(pixelCenterX, pixelCenterY, radius, color);
        }

        void drawRectangle(float minX, float maxX, float minY, float maxY, Color color) override {
            assert(m_window != nullptr);

            int pixelMinX = convertToPixelX(minX);
            int pixelMaxX = convertToPixelX(maxX);
            int pixelMinY = convertToPixelY(minY);
            int pixelMaxY = convertToPixelY(maxY);

            m_window->drawRectangle(pixelMinX, pixelMaxX, pixelMinY, pixelMaxY, color);
        }

        void drawLine(float x1, float y1, float x2, float y2, Color color) override {
            assert(m_window != nullptr);

            int pixelX1 = convertToPixelX(x1);
            int pixelY1 = convertToPixelY(y1);
            int pixelX2 = convertToPixelX(x2);
            int pixelY2 = convertToPixelY(y2);

            m_window->drawLine(pixelX1, pixelY1, pixelX2, pixelY2, color);
        }
    };
    
    Renderer m_renderer; 

public:
    Win32Core() = default; 
    Win32Core(const Win32Core&) = delete;
    Win32Core& operator=(const Win32Core&) = delete;

public:    
    bool initialize() override {
        try {
            auto window = std::make_unique<Window>(800, 600, "C++ Window");
            m_renderer.addWindow(std::move(window));
        } catch (const std::exception& e) {
            MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
            return false;
        }

        return true; 
    }

    void shutdown() override {
        std::cout << "Ending program" << std::endl;
        m_renderer.m_window.reset();
    }
   
    bool onPreFrame() override {
        return m_renderer.m_window->processMessages();
    }

    void onPostFrame() override {
        m_renderer.m_window->redraw();
    }

    IRenderer& getRenderer() {
        return m_renderer;
    }
};

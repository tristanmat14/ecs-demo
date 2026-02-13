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
#include "Vector2.hpp"

class Win32Core final : public ICore {
private:
    class Renderer final : public IRenderer {
    private:
        std::unique_ptr<Window> m_window = nullptr;

        float m_top = 1.0f; 
        float m_bottom = -1.0f; 
        float m_left = -1.0f; 
        float m_right = 1.0f;
       
        float m_cameraAspectRatio = (m_right - m_left) / (m_top - m_bottom);

        void addWindow(std::unique_ptr<Window> window) {
            m_window = std::move(window);
        }

        Vector2 calculateAdjustedWindowSize() {
            float width = m_window->getWidth();
            float height = m_window->getHeight();
            float windowAspectRatio = width / height; 

            float adjustedWindowWidth = width;
            float adjustedWindowHeight = height;

            if (windowAspectRatio > m_cameraAspectRatio) {
                // window too wide
                adjustedWindowWidth = height * m_cameraAspectRatio;
            } else if (windowAspectRatio < m_cameraAspectRatio) {
                // window too tall 
                adjustedWindowHeight = width / m_cameraAspectRatio; 
            }

            return Vector2{adjustedWindowWidth, adjustedWindowHeight};
        }

        Vector2 convertToPixelSpace(Vector2 cameraSpace) {
            assert(m_window != nullptr);

            Vector2 adjustedWindowSize = calculateAdjustedWindowSize();

            float width = m_window->getWidth();
            float height = m_window->getHeight();

            Vector2 windowOffset = (Vector2{width, height} - adjustedWindowSize) / 2.0f;

            Vector2 gradient = Vector2{
                adjustedWindowSize.x / (m_right - m_left),
                -adjustedWindowSize.y / (m_top - m_bottom)
            };
            Vector2 offset = -gradient * Vector2(m_left, m_top);
            Vector2 pixelSpace = Vector2{gradient * cameraSpace + offset};

            return pixelSpace + windowOffset;
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

            m_cameraAspectRatio = (m_right - m_left) / (m_top - m_bottom);
        }

        void clearScreen(Color color) override {
            clearScreen(color, Color{0, 0, 0});
        }

        void clearScreen(Color color, Color blankingColor) override {
            assert(m_window != nullptr);

            m_window->clearScreen(blankingColor);

            float height = m_window->getHeight();
            float width = m_window->getWidth();
            
            Vector2 adjustedWindowSize = calculateAdjustedWindowSize();

            float widthOffset = (width - adjustedWindowSize.x) / 2.0f;
            float heightOffset = (height - adjustedWindowSize.y) / 2.0f;

            m_window->drawRectangle(
                widthOffset, width - widthOffset,
                heightOffset, height - heightOffset,
                color
            );
        }
       
        void drawCircle(Vector2 center, float radius, Color color) override {
            assert(m_window != nullptr);

            Vector2 pixelSpace = convertToPixelSpace(center);

            m_window->drawCircle(
                static_cast<int>(pixelSpace.x),
                static_cast<int>(pixelSpace.y),
                radius, color
            ); 
        }

        void drawRectangle(Vector2 p1, Vector2 p2, Color color) override {
            assert(m_window != nullptr);

            std::cout << "INPUT: " << p1.x << "," << p1.y << " - " << p2.x << "," << p2.y << std::endl;

            Vector2 pixelP1 = convertToPixelSpace(p1);
            Vector2 pixelP2 = convertToPixelSpace(p2);

            std::cout << "OUTPUT: " << pixelP1.x << "," << pixelP1.y << " - " << pixelP2.x << "," << pixelP2.y << std::endl;

            m_window->drawRectangle(
                pixelP1.x, pixelP2.x,
                pixelP1.y, pixelP2.y, 
                color
            );
        }

        void drawLine(Vector2 p1, Vector2 p2, Color color) override {
            assert(m_window != nullptr);

            Vector2 pixelP1 = convertToPixelSpace(p1);
            Vector2 pixelP2 = convertToPixelSpace(p2);

            m_window->drawLine(pixelP1.x, pixelP1.y, pixelP2.x, pixelP2.y, color);
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

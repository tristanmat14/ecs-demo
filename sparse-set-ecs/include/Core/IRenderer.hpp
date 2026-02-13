#pragma once

#include "Color.hpp"
#include "Vector2.hpp"

struct IRenderer {
    virtual ~IRenderer() = default;

    virtual void setCameraSpace(float top, float bottom, float left, float right) = 0;
    virtual void clearScreen(Color color) = 0;
    virtual void clearScreen(Color color, Color blankingColor) = 0;
    virtual void drawCircle(Vector2 center, float radius, Color color) = 0; 
    virtual void drawRectangle(Vector2 p1, Vector2 p2, Color color) = 0;
    virtual void drawLine(Vector2 p1, Vector2 p2, Color color) = 0; 
};

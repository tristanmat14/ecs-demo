#pragma once

#include "Color.hpp"

struct IRenderer {
    virtual ~IRenderer() = default;

    virtual void setCameraSpace(float top, float bottom, float left, float right) = 0;
    virtual void clearScreen(Color color) = 0;
    virtual void drawCircle(float centerX, float centerY, float radius, Color color) = 0;
    virtual void drawRectangle(float minX, float maxX, float minY, float maxY, Color color) = 0;
    virtual void drawLine(float x1, float y1, float x2, float y2, Color color) = 0;
};

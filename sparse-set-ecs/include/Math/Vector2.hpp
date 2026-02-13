#pragma once

#include <cmath>

struct Vector2 {
    float x, y;

    Vector2(float x_, float y_) : x(x_), y(y_) {}
    Vector2() : x(0), y(0) {}
    Vector2(const Vector2& other) : x(other.x), y(other.y) {}

    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2 operator-() const {
        return {-x, -y}; 
    }

    Vector2 operator+(const Vector2& other) const {
        return {x + other.x, y + other.y};
    }

    Vector2 operator-(const Vector2& other) const {
        return {x - other.x, y - other.y}; 
    }

    // component wise multiplication
    Vector2 operator*(const Vector2& other) const {
        return {x * other.x, y * other.y};
    }
    
    // component wise division 
    Vector2 operator/(const Vector2& other) const {
        return {x / other.x, y / other.y};
    }

    Vector2 operator*(const float scalar) const {
        return {x * scalar, y * scalar};
    }

    Vector2 operator/(const float scalar) const {
        return {x / scalar, y / scalar};
    }

    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        
        return *this;
    }
    
    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        
        return *this;
    }

    Vector2& operator*=(const float scalar) {
        x *= scalar;
        y *= scalar;
        
        return *this;
    }

    Vector2& operator/=(const float scalar) {
        x /= scalar;
        y /= scalar;

        return *this;
    }

    bool operator==(const Vector2& other) const {
        return (x == other.x && y == other.y);
    }

    bool operator!=(const Vector2& other) const {
        return !(operator==(other));
    }
};

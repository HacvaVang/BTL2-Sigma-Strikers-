#pragma once

#include <cmath>

// Simple 2D vector class used for positions, velocities, etc.
// All units are floating point (metres or metres/second as appropriate).
class Vector {
public:
    float x;
    float y;

    // constructors
    Vector(float x_ = 0.0f, float y_ = 0.0f) : x(x_), y(y_) {}

    // basic arithmetic operators
    Vector operator+(const Vector& other) const { return Vector(x + other.x, y + other.y); }
    Vector& operator+=(const Vector& other) { x += other.x; y += other.y; return *this; }

    Vector operator-(const Vector& other) const { return Vector(x - other.x, y - other.y); }
    Vector& operator-=(const Vector& other) { x -= other.x; y -= other.y; return *this; }

    Vector operator*(float scalar) const { return Vector(x * scalar, y * scalar); }
    Vector& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }

    Vector operator/(float scalar) const { return Vector(x / scalar, y / scalar); }
    Vector& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

    // vector utilities
    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSquared() const { return x * x + y * y; }
    float dot(const Vector& other) const { return x * other.x + y * other.y; }

    Vector normalized() const {
        float len = length();
        if (len < 0.0001f) return Vector(0,0);
        return Vector(x / len, y / len);
    }

    float distanceTo(const Vector& other) const {
        return (*this - other).length();
    }
};

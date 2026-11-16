#include "mathExt.h"

float clamp(const float x, const float min, const float max) {
    if (x < min)
        return min;
    else if (x > max)
        return max;
    return x;
}

Vector2Float vec2f(const float x, const float y) {
    return (Vector2Float) {
        .x = x,
        .y = y,
    };
}

Vector2Float Vec2Add(const Vector2Float a, const Vector2Float b) {
    return (Vector2Float) {
        .x = a.x + b.x,
        .y = a.y + b.y,
    };
}

Vector2Float Vec2Sub(const Vector2Float a, const Vector2Float b) {
    return (Vector2Float) {
        .x = a.x - b.x,
        .y = a.y - b.y,
    };
}

Vector2Float Vec2Mult(const Vector2Float a, const Vector2Float b) {
    return (Vector2Float) {
        .x = a.x * b.x,
        .y = a.y * b.y,
    };
}

Vector2Float Vec2Div(const Vector2Float a, const Vector2Float b) {
    return (Vector2Float) {
        .x = a.x / b.x,
        .y = a.y / b.y,
    };
}
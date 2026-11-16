#ifndef MATH_EXT_H_
#define MATH_EXT_H_

float clamp(float x, float min, float max);

typedef struct {
    float x, y;
} Vector2Float;

Vector2Float vec2f(const float x, const float y);
Vector2Float Vec2Add(const Vector2Float a, const Vector2Float b);
Vector2Float Vec2Sub(const Vector2Float a, const Vector2Float b);
Vector2Float Vec2Mult(const Vector2Float a, const Vector2Float b);
Vector2Float Vec2Div(const Vector2Float a, const Vector2Float b);

#endif // MATH_EXT_H_
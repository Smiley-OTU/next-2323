#pragma once
#include "App/AppSettings.h"
#include <cmath>
#undef near
#undef far
#include "Math2.h"

constexpr float TWO_PI  = PI * 2.0f;

// Part of Math2.h
//constexpr float DEG2RAD = PI / 180.0f;
//constexpr float RAD2DEG = 180.0f / PI;

struct vec2
{
	float x = 0.0f;
	float y = 0.0f;
};

constexpr vec2 v2zero = { 0.0f, 0.0f };

inline vec2 operator+(const vec2& a, const vec2& b)
{
	return { a.x + b.x, a.y + b.y };
}

inline vec2 operator-(const vec2& a, const vec2& b)
{
	return { a.x - b.x, a.y - b.y };
}

inline vec2 operator*(const vec2& a, float b)
{
	return { a.x * b, a.y * b };
}

inline vec2 operator/(const vec2& a, float b)
{
	return { a.x / b, a.y / b };
}

inline float Length(const vec2& v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

inline float LengthSqr(const vec2& v)
{
	return v.x * v.x + v.y * v.y;
}

inline vec2 Normalize(const vec2& v)
{
	float length = Length(v);
	return length > 0.0f ? v / length : v2zero;
}

inline float Dot(const vec2& a, const vec2& b)
{
	return { a.x * b.x + a.y * b.y };
}

inline vec2 Project(const vec2& a, const vec2& b)
{
	return b * (Dot(a, b) / LengthSqr(b));
}

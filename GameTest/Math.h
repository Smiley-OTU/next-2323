#pragma once
#include "App/AppSettings.h"
#include <cmath>
#undef near
#undef far
#include "Math2.h"

constexpr float TWO_PI  = PI * 2.0f;

struct Color
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

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

// Scalar overlap test
inline bool Overlaps(float min1, float max1, float min2, float max2)
{
	return !((max1 < min2) || (max2 < min1));
}

// Scalar overlap difference
inline float Overlap(float min1, float max1, float min2, float max2)
{
	return fminf(max1, max2) - fmaxf(min1, min2);
}

// Things we use that are part of Math2 (Math2 is most likely not allowed in Ubisoft Next 2023):
//constexpr float DEG2RAD = PI / 180.0f;
//constexpr float RAD2DEG = 180.0f / PI;

//inline float Clamp(float value, float min, float max)
//{
//	return (fmaxf(max, fminf(value, min)));
//}

vec2 Clamp(vec2 v, vec2 min, vec2 max)
{
	return { fminf(max.x, fmaxf(min.x, v.x)), fminf(max.y, fmaxf(min.y, v.y)) };
}

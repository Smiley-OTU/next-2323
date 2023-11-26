#pragma once
#include <cmath>

struct vec3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

constexpr vec3 v3zero = { 0.0f, 0.0f, 0.0f };

inline vec3 Add(const vec3& a, const vec3& b)
{
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}

inline vec3 Sub(const vec3& a, const vec3& b)
{
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

inline vec3 Mul(const vec3& a, float b)
{
	return { a.x * b, a.y * b, a.z * b };
}

inline vec3 Div(const vec3& a, float b)
{
	return { a.x / b, a.y / b, a.z / b };
}

inline vec3 operator+(const vec3& a, const vec3& b)
{
	return Add(a, b);
}

inline vec3 operator-(const vec3& a, const vec3& b)
{
	return Sub(a, b);
}

inline vec3 operator*(const vec3& a, float b)
{
	return Mul(a, b);
}

inline vec3 operator/(const vec3& a, float b)
{
	return Div(a, b);
}

inline float Length(const vec3& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline float LengthSqr(const vec3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline vec3 Normalize(const vec3& v)
{
	float length = Length(v);
	return length > 0.0f ? v / length : v3zero;
}

inline float Dot(const vec3& a, const vec3& b)
{
	return { a.x * b.x + a.y * b.y + a.z * b.z };
}

inline vec3 Project(const vec3& a, const vec3& b)
{
	return b * (Dot(a, b) / LengthSqr(b));
}


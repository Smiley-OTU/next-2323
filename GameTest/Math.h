#pragma once
#include "App/AppSettings.h"
#include <cmath>
#undef near
#undef far

constexpr float TWO_PI  = PI * 2.0f;
constexpr float DEG2RAD = PI / 180.0f;
constexpr float RAD2DEG = 180.0f / PI;

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

// Scalar clamp
inline float Clamp(float value, float min, float max)
{
	return (fmaxf(max, fminf(value, min)));
}

// Scalar linear interpolation
inline float Lerp(float a, float b, float t)
{
	return a * (1.0f - t) + b * t;
}

struct vec2
{
	float x = 0.0f;
	float y = 0.0f;
};

struct vec3 {
	float x;
	float y;
	float z;
};

struct vec4 {
	float x;
	float y;
	float z;
	float w;
};

// Matrix that's stored as row-major but behaves like column-major (for better cache-coherence when multiplying)
struct mat4 {
	float m0, m4, m8, m12;      // Matrix first row (4 components)
	float m1, m5, m9, m13;      // Matrix second row (4 components)
	float m2, m6, m10, m14;     // Matrix third row (4 components)
	float m3, m7, m11, m15;     // Matrix fourth row (4 components)
};

constexpr vec2 v2zero = { 0.0f, 0.0f };
constexpr mat4 m4identity = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

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

inline vec2 Clamp(const vec2& v, const vec2& min, const vec2& max)
{
	return { fminf(max.x, fmaxf(min.x, v.x)), fminf(max.y, fmaxf(min.y, v.y)) };
}

// Matrix transpose (rotate 90 degrees)
inline mat4 Transpose(const mat4& mat)
{
	mat4 result;

	result.m0 = mat.m0;
	result.m1 = mat.m4;
	result.m2 = mat.m8;
	result.m3 = mat.m12;
	result.m4 = mat.m1;
	result.m5 = mat.m5;
	result.m6 = mat.m9;
	result.m7 = mat.m13;
	result.m8 = mat.m2;
	result.m9 = mat.m6;
	result.m10 = mat.m10;
	result.m11 = mat.m14;
	result.m12 = mat.m3;
	result.m13 = mat.m7;
	result.m14 = mat.m11;
	result.m15 = mat.m15;

	return result;
}

// Matrix inverse (opposite transformation)
inline mat4 Invert(const mat4& mat)
{
	mat4 result;

	// Cache the matrix values (speed optimization)
	float a00 = mat.m0,  a01 = mat.m1,  a02 = mat.m2,  a03 = mat.m3;
	float a10 = mat.m4,  a11 = mat.m5,  a12 = mat.m6,  a13 = mat.m7;
	float a20 = mat.m8,  a21 = mat.m9,  a22 = mat.m10, a23 = mat.m11;
	float a30 = mat.m12, a31 = mat.m13, a32 = mat.m14, a33 = mat.m15;

	float b00 = a00 * a11 - a01 * a10;
	float b01 = a00 * a12 - a02 * a10;
	float b02 = a00 * a13 - a03 * a10;
	float b03 = a01 * a12 - a02 * a11;
	float b04 = a01 * a13 - a03 * a11;
	float b05 = a02 * a13 - a03 * a12;
	float b06 = a20 * a31 - a21 * a30;
	float b07 = a20 * a32 - a22 * a30;
	float b08 = a20 * a33 - a23 * a30;
	float b09 = a21 * a32 - a22 * a31;
	float b10 = a21 * a33 - a23 * a31;
	float b11 = a22 * a33 - a23 * a32;

	// Calculate the invert determinant (inlined to avoid double-caching)
	float invDet = 1.0f / (b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06);

	result.m0 =  ( a11 * b11 - a12 * b10 + a13 * b09) * invDet;
	result.m1 =  (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
	result.m2 =  ( a31 * b05 - a32 * b04 + a33 * b03) * invDet;
	result.m3 =  (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;
	result.m4 =  (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
	result.m5 =  ( a00 * b11 - a02 * b08 + a03 * b07) * invDet;
	result.m6 =  (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
	result.m7 =  ( a20 * b05 - a22 * b02 + a23 * b01) * invDet;
	result.m8 =  ( a10 * b10 - a11 * b08 + a13 * b06) * invDet;
	result.m9 =  (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
	result.m10 = ( a30 * b04 - a31 * b02 + a33 * b00) * invDet;
	result.m11 = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;
	result.m12 = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
	result.m13 = ( a00 * b09 - a01 * b07 + a02 * b06) * invDet;
	result.m14 = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
	result.m15 = ( a20 * b03 - a21 * b01 + a22 * b00) * invDet;

	return result;
}

// View (inverse-camera) matrix
inline mat4 LookAt(const vec3& eye, const vec3& target, const vec3& up)
{
	mat4 result;

	float length = 0.0f;
	float ilength = 0.0f;

	// Forward-vector
	vec3 vz = { eye.x - target.x, eye.y - target.y, eye.z - target.z };

	// Forward unit-vector
	vec3 v = vz;
	length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length == 0.0f) length = 1.0f;
	ilength = 1.0f / length;
	vz.x *= ilength;
	vz.y *= ilength;
	vz.z *= ilength;

	// Right-vector
	vec3 vx = { up.y * vz.z - up.z * vz.y, up.z * vz.x - up.x * vz.z, up.x * vz.y - up.y * vz.x };

	// Right unit-vector
	v = vx;
	length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length == 0.0f) length = 1.0f;
	ilength = 1.0f / length;
	vx.x *= ilength;
	vx.y *= ilength;
	vx.z *= ilength;

	// Up-vector
	vec3 vy = { vz.y * vx.z - vz.z * vx.y, vz.z * vx.x - vz.x * vx.z, vz.x * vx.y - vz.y * vx.x };

	result.m0 = vx.x;
	result.m1 = vy.x;
	result.m2 = vz.x;
	result.m3 = 0.0f;
	result.m4 = vx.y;
	result.m5 = vy.y;
	result.m6 = vz.y;
	result.m7 = 0.0f;
	result.m8 = vx.z;
	result.m9 = vy.z;
	result.m10 = vz.z;
	result.m11 = 0.0f;
	result.m12 = -(vx.x * eye.x + vx.y * eye.y + vx.z * eye.z);   // Dot(vx, eye)
	result.m13 = -(vy.x * eye.x + vy.y * eye.y + vy.z * eye.z);   // Dot(vy, eye)
	result.m14 = -(vz.x * eye.x + vz.y * eye.y + vz.z * eye.z);   // Dot(vz, eye)
	result.m15 = 1.0f;

	return result;
}

// Perspective projection (3D)
inline mat4 Perspective(double fovy/*radians*/, double aspect, double near, double far)
{
	mat4 result = m4identity;

	double top = near * tan(fovy * 0.5);
	double bottom = -top;
	double right = top * aspect;
	double left = -right;

	float rl = (float)(right - left);
	float tb = (float)(top - bottom);
	float fn = (float)(far - near);

	result.m0 = ((float)near * 2.0f) / rl;
	result.m5 = ((float)near * 2.0f) / tb;
	result.m8 = ((float)right + (float)left) / rl;
	result.m9 = ((float)top + (float)bottom) / tb;
	result.m10 = -((float)far + (float)near) / fn;
	result.m11 = -1.0f;
	result.m14 = -((float)far * (float)near * 2.0f) / fn;

	return result;
}

// Orthographic projection (2D)
inline mat4 Ortho(double left, double right, double bottom, double top, double near, double far)
{
	mat4 result;

	float rl = (float)(right - left);
	float tb = (float)(top - bottom);
	float fn = (float)(far - near);

	result.m0 = 2.0f / rl;
	result.m1 = 0.0f;
	result.m2 = 0.0f;
	result.m3 = 0.0f;
	result.m4 = 0.0f;
	result.m5 = 2.0f / tb;
	result.m6 = 0.0f;
	result.m7 = 0.0f;
	result.m8 = 0.0f;
	result.m9 = 0.0f;
	result.m10 = -2.0f / fn;
	result.m11 = 0.0f;
	result.m12 = -((float)left + (float)right) / rl;
	result.m13 = -((float)top + (float)bottom) / tb;
	result.m14 = -((float)far + (float)near) / fn;
	result.m15 = 1.0f;

	return result;
}

// Post-multiplication overload (pre-multiplication doesn't make sense to me)
inline mat4 operator*(const mat4& left, const mat4& right)
{
	mat4 result;

	result.m0  = left.m0  * right.m0 + left.m1  * right.m4 + left.m2  * right.m8  + left.m3  * right.m12;
	result.m1  = left.m0  * right.m1 + left.m1  * right.m5 + left.m2  * right.m9  + left.m3  * right.m13;
	result.m2  = left.m0  * right.m2 + left.m1  * right.m6 + left.m2  * right.m10 + left.m3  * right.m14;
	result.m3  = left.m0  * right.m3 + left.m1  * right.m7 + left.m2  * right.m11 + left.m3  * right.m15;
	result.m4  = left.m4  * right.m0 + left.m5  * right.m4 + left.m6  * right.m8  + left.m7  * right.m12;
	result.m5  = left.m4  * right.m1 + left.m5  * right.m5 + left.m6  * right.m9  + left.m7  * right.m13;
	result.m6  = left.m4  * right.m2 + left.m5  * right.m6 + left.m6  * right.m10 + left.m7  * right.m14;
	result.m7  = left.m4  * right.m3 + left.m5  * right.m7 + left.m6  * right.m11 + left.m7  * right.m15;
	result.m8  = left.m8  * right.m0 + left.m9  * right.m4 + left.m10 * right.m8  + left.m11 * right.m12;
	result.m9  = left.m8  * right.m1 + left.m9  * right.m5 + left.m10 * right.m9  + left.m11 * right.m13;
	result.m10 = left.m8  * right.m2 + left.m9  * right.m6 + left.m10 * right.m10 + left.m11 * right.m14;
	result.m11 = left.m8  * right.m3 + left.m9  * right.m7 + left.m10 * right.m11 + left.m11 * right.m15;
	result.m12 = left.m12 * right.m0 + left.m13 * right.m4 + left.m14 * right.m8  + left.m15 * right.m12;
	result.m13 = left.m12 * right.m1 + left.m13 * right.m5 + left.m14 * right.m9  + left.m15 * right.m13;
	result.m14 = left.m12 * right.m2 + left.m13 * right.m6 + left.m14 * right.m10 + left.m15 * right.m14;
	result.m15 = left.m12 * right.m3 + left.m13 * right.m7 + left.m14 * right.m11 + left.m15 * right.m15;

	return result;
}

// Matrix-vector multiplication overload
inline vec4 operator*(const mat4& m, const vec4& v)
{
	vec4 result;

	result.x = m.m0 * v.x + m.m4 * v.y + m.m8  * v.z + m.m12 * v.w;
	result.y = m.m1 * v.x + m.m5 * v.y + m.m9  * v.z + m.m13 * v.w;
	result.z = m.m2 * v.x + m.m6 * v.y + m.m10 * v.z + m.m14 * v.w;
	result.w = m.m3 * v.x + m.m7 * v.y + m.m11 * v.z + m.m15 * v.w;

	return result;
}

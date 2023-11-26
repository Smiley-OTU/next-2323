#pragma once
#include "Vec3.h"

struct Entity
{
	size_t id = 0;
	vec3 pos;
};

inline Entity Create()
{
	static size_t id = 0;
	return { ++id };
}

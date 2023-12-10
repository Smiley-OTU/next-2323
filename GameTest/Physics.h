#pragma once
#include "Collision.h"

class Physics
{
public:
    void Update(float dt, Entities& entities);
    void Render(const Entities& entities);

	float timestep = 1.0f / 50.0f;
	vec2 gravity = { 0.0f, -9.8f };

private:
    float prevTime = 0.0f;
    float currTime = 0.0f;

    void Step(float dt, Entities& entities);
};

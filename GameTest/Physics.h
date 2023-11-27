#pragma once
#include "Collision.h"
#include <vector>

class PhysicsWorld
{
public:
    void Update(float dt);
    void Render();

	float timestep = 1.0f / 50.0f;
	vec2 gravity = { 0.0f, -9.8f };

    std::vector<Particle> particles;
    const std::vector<Manifold>& Collisions() { return mCollisions; }
private:
    std::vector<Manifold> mCollisions;

    float prevTime = 0.0f;
    float currTime = 0.0f;

    void Step(float dt);
};
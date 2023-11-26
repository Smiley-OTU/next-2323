#pragma once
#include "Entity.h"
#include <vector>
#include <unordered_map>

struct Collider
{
    enum { PLANE, CIRCLE } shape;
    union
    {
        vec3 normal{};
        float radius;
    };
    bool dynamic = false;
};

struct Particle : Entity
{
    vec3 vel;
    vec3 acc;

    float gravityScale = 1.0f;
    float mass = 1.0f;
    float friction = 0.0f;
    float restitution = 1.0f;

    Collider collider;
};

class PhysicsWorld
{
public:
	vec3 gravity = { 0.0f, -9.8f, 0.0f };
	float timestep = 1.0f / 50.0f;
	float prevTime = 0.0f;
	float currTime = 0.0f;

private:
    std::vector<Particle> mParticles;
};
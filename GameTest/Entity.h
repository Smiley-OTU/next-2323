#pragma once
#include "Math.h"
#include <vector>

struct Entity;
using OnCollision = void(*)(Entity& entity);

enum Shape
{
    CIRCLE,
    PLANE,
    AABB
};

enum Tag
{
    PLAYER,
    BRICK,
    BALL,
    WALL,
    INVALID
};

struct Collider
{
    union
    {
        vec2 normal{};
        vec2 extents;
        float radius;
    };

    Shape shape = CIRCLE;
};

// Making an ECS is overkill so I'm storing everything in this Entity structure
struct Entity
{
    vec2 pos;
    vec2 vel;
    vec2 acc;
    vec2 force;

    float gravityScale = 1.0f;
    float invMass = 1.0f;
    float friction = 0.0f;
    float restitution = 1.0f;

    Collider collider;
    OnCollision onCollision = nullptr;
    Tag tag = INVALID;
    bool disabled = false;
};

using Entities = std::vector<Entity>;
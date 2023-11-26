#pragma once
#include "Vec3.h"
#include <vector>

enum Shape
{
    CIRCLE,
    PLANE
};

struct Collider
{
    Shape shape;
    union
    {
        vec3 normal{};
        float radius;
    };
    bool dynamic = false;
};

struct Particle
{
    vec3 pos;
    vec3 vel;
    vec3 acc;
    vec3 force;

    float gravityScale = 1.0f;
    float invMass = 1.0f;
    float friction = 0.0f;
    float restitution = 1.0f;

    Collider collider;
};

struct Manifold
{
    Particle* a = nullptr;
    Particle* b = nullptr;
    vec3 mtv{};
};

// mtv points from 2 to 1
inline bool SphereSphere(vec3 pos1, float radius1, vec3 pos2, float radius2, vec3* mtv = nullptr)
{
    vec3 direction = pos1 - pos2;
    float distance = Length(direction);
    float radiiSum = radius1 + radius2;
    bool collision = distance <= radiiSum;
    if (collision && mtv != nullptr)
        *mtv = Normalize(direction) * (radiiSum - distance);
    return collision;
}

// mtv points from plane to sphere
inline bool SpherePlane(vec3 circle, float radius, vec3 plane, vec3 normal, vec3* mtv = nullptr)
{
    float distance = Dot(circle - plane, normal);
    bool collision = distance <= radius;
    if (collision && mtv != nullptr)
        *mtv = normal * (radius - distance);
    return collision;
}

inline bool HitTest(vec3 pos1, vec3 pos2, Collider col1, Collider col2, vec3* mtv = nullptr)
{
    if (col1.shape == CIRCLE && col2.shape == CIRCLE)
        return SphereSphere(pos1, col1.radius, pos2, col2.radius, mtv);

    if (col1.shape == CIRCLE && col2.shape == PLANE)
        return SpherePlane(pos1, col1.radius, pos2, col2.normal, mtv);

    if (col1.shape == PLANE && col2.shape == CIRCLE)
        return SpherePlane(pos2, col2.radius, pos1, col1.normal, mtv);

    return false;
}

inline std::vector<Manifold> HitTest(const std::vector<Particle>& particles)
{
    std::vector<Manifold> collisions;
    for (size_t i = 0; i < particles.size(); i++)
    {
        for (size_t j = i + 1; j < particles.size(); j++)
        {
            Manifold manifold;
            const Particle& a = particles[i];
            const Particle& b = particles[j];
            if (HitTest(a.pos, b.pos, a.collider, b.collider, &manifold.mtv))
            {
                if (!a.collider.dynamic && b.collider.dynamic)
                {
                    manifold.mtv = manifold.mtv * -1.0f;
                    manifold.a = (Particle*)&b, manifold.b = (Particle*)&a;
                }
                else
                {
                    manifold.a = (Particle*)&a, manifold.b = (Particle*)&b;
                }
                collisions.push_back(manifold);
            }
        }
    }
    return collisions;
}

inline void ResolveVelocity(Manifold collision)
{
    Particle& a = *collision.a;
    Particle& b = *collision.b;
}

inline void ResolvePosition(Manifold collision)
{
    Particle& a = *collision.a;
    Particle& b = *collision.b;
    if (b.collider.dynamic)
    {
        a.pos = a.pos + collision.mtv * 0.5f;
        b.pos = b.pos - collision.mtv * 0.5f;
    }
    else
        a.pos = a.pos + collision.mtv;
}
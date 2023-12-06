#pragma once
#include "Math.h"
#include <vector>

enum Shape
{
    CIRCLE,
    PLANE,
    AABB
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
    bool dynamic = false;
};

struct Particle
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
};

struct Manifold
{
    Particle* a = nullptr;
    Particle* b = nullptr;
    vec2 mtv{};
};

// mtv points from 2 to 1
inline bool CircleCircle(vec2 pos1, float radius1, vec2 pos2, float radius2, vec2* mtv = nullptr)
{
    vec2 direction = pos1 - pos2;
    float distance = Length(direction);
    float radiiSum = radius1 + radius2;
    bool collision = distance <= radiiSum;
    if (collision && mtv != nullptr)
        *mtv = Normalize(direction) * (radiiSum - distance);
    return collision;
}

// TODO -- fix this and make it look nice
// mtv points from box to circle
//inline bool CircleRect(vec2 circleCenter, float radius, vec2 rectCenter, vec2 halfExtents, vec2* mtv = nullptr)
//{
//    vec2 nearest;
//    // We need to change how these are calculated
//    float xMin = rectCenter.x - halfExtents.x;
//    float xMax = rectCenter.x + halfExtents.x;
//    float yMin = rectCenter.y - halfExtents.y;
//    float yMax = rectCenter.y + halfExtents.y;
//    nearest.x = circleCenter.x < xMin ? xMin : xMax;
//    nearest.y = circleCenter.y < yMin ? yMin : yMax;
//
//    float distance = Length(circleCenter - nearest);
//    bool collision = distance <= radius;
//    if (collision && mtv != nullptr)
//        *mtv = Normalize(circleCenter - rectCenter) * (radius - distance);
//    return collision;
//}

inline bool CircleRect2(float cx, float cy, float radius, float rx, float ry, float rw, float rh, vec2* mtv = nullptr) {

    // temporary variables to set edges for testing
    float testX = cx;
    float testY = cy;

    // which edge is closest?
    if (cx < rx)         testX = rx;      // test left edge
    else if (cx > rx + rw) testX = rx + rw;   // right edge
    if (cy < ry)         testY = ry;      // top edge
    else if (cy > ry + rh) testY = ry + rh;   // bottom edge

    // get distance from closest edges
    float distX = cx - testX;
    float distY = cy - testY;
    float distance = sqrt((distX * distX) + (distY * distY));

    // if the distance is less than the radius, collision!
    bool collision = distance <= radius;
    if (collision && mtv != nullptr) {
        // Calculate rectangle centre
        vec2 c = { rx + rw * 0.5f, ry + rh * 0.5f };
        *mtv = Normalize(vec2{ cx, cy } - c) * (radius - distance);
    }
    return collision;
}

// mtv points from plane to sphere
inline bool CirclePlane(vec2 circle, float radius, vec2 plane, vec2 normal, vec2* mtv = nullptr)
{
    float distance = Dot(circle - plane, normal);
    bool collision = distance <= radius;
    if (collision && mtv != nullptr)
        *mtv = normal * (radius - distance);
    return collision;
}

inline bool HitTest(vec2 pos1, vec2 pos2, Collider col1, Collider col2, vec2* mtv = nullptr)
{
    if (col1.shape == CIRCLE && col2.shape == CIRCLE)
        return CircleCircle(pos1, col1.radius, pos2, col2.radius, mtv);

    if (col1.shape == CIRCLE && col2.shape == PLANE)
        return CirclePlane(pos1, col1.radius, pos2, col2.normal, mtv);

    if (col1.shape == PLANE && col2.shape == CIRCLE)
        return CirclePlane(pos2, col2.radius, pos1, col1.normal, mtv);

    if (col1.shape == CIRCLE && col2.shape == AABB)
        //return CircleRect(pos1, col1.radius, pos2, col2.extents, mtv);
        return CircleRect2(pos1.x, pos1.y, col1.radius, pos2.x - col2.extents.x, pos2.y - col2.extents.y,
            col2.extents.x * 2.0f, col2.extents.y * 2.0f, mtv);

    if (col1.shape == AABB && col2.shape == CIRCLE)
        //return CircleRect(pos2, col2.radius, pos1, col1.extents, mtv);
        return CircleRect2(pos2.x, pos2.y, col2.radius, pos1.x - col1.extents.x, pos1.y - col1.extents.y,
            col1.extents.x * 2.0f, col1.extents.y * 2.0f, mtv);

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
    // e = restitution, j = impulse, jt = tangent impulse (friciton), mu = friction coefficient
    Particle& a = *collision.a;
    Particle& b = *collision.b;

    // Exit if objects are separating or both have infinite masses
    vec2 normal = Normalize(collision.mtv);
    vec2 velBA = a.vel - b.vel;
    float t = Dot(velBA, normal);
    if (t > 0.0f) return;
    if ((a.invMass + b.invMass) <= FLT_MIN) return;

    // Restitution
    float e = fminf(a.restitution, b.restitution);
    float j = -(1.0f + e) * t / (a.invMass + b.invMass);
    a.vel = a.vel + normal * j * a.invMass;
    b.vel = b.vel - normal * j * b.invMass;

    // Friction
    vec2 tangent = Normalize(velBA - normal * t);
    float jt = -Dot(velBA, tangent) / (a.invMass + b.invMass);
    float mu = sqrtf(a.friction * b.friction);
    jt = Clamp(jt, -j * mu, j * mu);
    a.vel = a.vel + tangent * jt * a.invMass;
    b.vel = b.vel + tangent * jt * b.invMass;
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

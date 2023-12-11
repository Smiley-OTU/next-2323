#pragma once
#include "Entity.h"

struct Manifold
{
    Entity* a = nullptr;
    Entity* b = nullptr;
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

// mtv points from plane to sphere
inline bool CirclePlane(vec2 circle, float radius, vec2 plane, vec2 normal, vec2* mtv = nullptr)
{
    float distance = Dot(circle - plane, normal);
    bool collision = distance <= radius;
    if (collision && mtv != nullptr)
        *mtv = normal * (radius - distance);
    return collision;
}

// mtv points from rect to circle
inline bool CircleRect(vec2 circle, float radius, vec2 rect, vec2 extents, vec2* mtv = nullptr)
{
    vec2 nearest = {
        Clamp(circle.x, rect.x - extents.x, rect.x + extents.x),
        Clamp(circle.y, rect.y - extents.y, rect.y + extents.y),
    };

    vec2 normal = Normalize(circle - nearest);
    return CirclePlane(circle, radius, nearest, normal, mtv);
}

inline bool RectRect(vec2 rect1, vec2 extents1, vec2 rect2, vec2 extents2, vec2* mtv = nullptr)
{
    float xMin1 = rect1.x - extents1.x;
    float xMax1 = rect1.x + extents1.x;
    float yMin1 = rect1.y - extents1.y;
    float yMax1 = rect1.y + extents1.y;

    float xMin2 = rect2.x - extents2.x;
    float xMax2 = rect2.x + extents2.x;
    float yMin2 = rect2.y - extents2.y;
    float yMax2 = rect2.y + extents2.y;

    return Overlaps(xMin1, xMax1, xMin2, xMax2) && Overlaps(yMin1, yMax1, yMin2, yMax2);
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
        return CircleRect(pos1, col1.radius, pos2, col2.extents, mtv);

    if (col1.shape == AABB && col2.shape == CIRCLE)
        return CircleRect(pos2, col2.radius, pos1, col1.extents, mtv);

    return false;
}

inline std::vector<Manifold> HitTest(const std::vector<Entity>& entities)
{
    std::vector<Manifold> collisions;
    for (size_t i = 0; i < entities.size(); i++)
    {
        for (size_t j = i + 1; j < entities.size(); j++)
        {
            Manifold manifold;
            const Entity& a = entities[i];
            const Entity& b = entities[j];
            if (a.disabled || b.disabled) continue;
            if (HitTest(a.pos, b.pos, a.collider, b.collider, &manifold.mtv))
            {
                // Ensure A is always dynamic and B is either static or dynamic
                if (!a.invMass > 0.0f && b.invMass > 0.0f)
                    manifold.a = (Entity*)&b, manifold.b = (Entity*)&a;
                else
                    manifold.a = (Entity*)&a, manifold.b = (Entity*)&b;

                // Ensure the mtv always points from B to A
                //if (Dot(manifold.a->pos - manifold.b->pos, manifold.mtv) < 0.0f)
                //    manifold.mtv = manifold.mtv * -1.0f;
                // This is somehow breaking things...

                collisions.push_back(manifold);
            }
        }
    }
    return collisions;
}

inline void ResolveVelocity(Manifold collision)
{
    // e = restitution, j = impulse, jt = tangent impulse (friciton), mu = friction coefficient
    Entity& a = *collision.a;
    Entity& b = *collision.b;

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
    //vec2 tangent = Normalize(velBA - normal * t);
    //float jt = -Dot(velBA, tangent) / (a.invMass + b.invMass);
    //float mu = sqrtf(a.friction * b.friction);
    //jt = Clamp(jt, -j * mu, j * mu);
    //a.vel = a.vel + tangent * jt * a.invMass;
    //b.vel = b.vel + tangent * jt * b.invMass;
}

inline void ResolvePosition(Manifold collision)
{
    Entity& a = *collision.a;
    Entity& b = *collision.b;
    if (b.invMass > 0.0f)
    {
        a.pos = a.pos + collision.mtv * 0.5f;
        b.pos = b.pos - collision.mtv * 0.5f;
    }
    else
        a.pos = a.pos + collision.mtv;
}

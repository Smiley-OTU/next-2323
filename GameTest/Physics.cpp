#include "Physics.h"
#include "Graphics.h"

void Physics::Update(float dt, Entities& entities)
{
    while (prevTime < currTime)
    {
        prevTime += timestep;
        Step(timestep, entities);
    }
    currTime += dt;
}

void Physics::Step(float dt, Entities& entities)
{
    for (size_t i = 0; i < entities.size(); i++)
    {
        Entity& p = entities[i];

        vec2 fa = p.force * p.invMass;
        vec2 fg = gravity * p.gravityScale;
        p.acc = fa + fg;

        p.vel = p.vel + p.acc * dt;
        p.pos = p.pos + p.vel * dt;

        p.force = v2zero;
    }

    std::vector<Manifold> collisions = HitTest(entities);
    for (const Manifold& collision : collisions)
    {
        ResolveVelocity(collision);
        ResolvePosition(collision);

        Entity& a = *collision.a;
        Entity& b = *collision.b;
        if (a.onCollision != nullptr)
            a.onCollision(b);
        if (b.onCollision != nullptr)
            b.onCollision(a);
    }
}

void Physics::Render(const Entities& entities)
{
    std::vector<bool> collisions(entities.size());
    for (size_t i = 0; i < entities.size(); i++)
    {
        for (size_t j = i + 1; j < entities.size(); j++)
        {
            const Entity& a = entities[i];
            const Entity& b = entities[j];
            collisions[i] = collisions[i] || HitTest(a.pos, b.pos, a.collider, b.collider);
        }
    }
    
    for (size_t i = 0; i < entities.size(); i++)
    {
        Color color = collisions[i] ? Color{ 1.0f, 0.0f, 0.0f } : Color{ 0.0f, 1.0f, 0.0f };
        const Entity& p = entities[i];
        switch (p.collider.shape)
        {
        case CIRCLE:
            DrawCircle(p.pos, p.collider.radius, color);
            break;

        case AABB:
            DrawRect(p.pos, p.collider.extents.x * 2.0f, p.collider.extents.y * 2.0f, color);
            break;

        case PLANE:
            break;
        }
    }
}
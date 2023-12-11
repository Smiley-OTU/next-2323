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
    for (Entity& entity : entities)
    {
        vec2 fa = entity.force * entity.invMass;
        vec2 fg = gravity * entity.gravityScale;
        entity.acc = fa + fg;

        entity.vel = entity.vel + entity.acc * dt;
        entity.pos = entity.pos + entity.vel * dt;

        entity.force = v2zero;
    }

    std::vector<Manifold> collisions = HitTest(entities);
    for (const Manifold& collision : collisions)
    {
        ResolveVelocity(collision);
        ResolvePosition(collision);

        Entity& a = *collision.a;
        Entity& b = *collision.b;
        if (a.onCollision != nullptr)
            a.onCollision(a, b);
        if (b.onCollision != nullptr)
            b.onCollision(b, a);
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
        const Entity& entity = entities[i];
        switch (entity.collider.shape)
        {
        case CIRCLE:
            DrawCircle(entity.pos, entity.collider.radius, color);
            break;

        case AABB:
            DrawRect(entity.pos, entity.collider.extents.x * 2.0f, entity.collider.extents.y * 2.0f, color);
            break;

        case PLANE:
            break;
        }
    }
}
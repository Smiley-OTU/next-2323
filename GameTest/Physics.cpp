#include "Physics.h"
#include "Graphics.h"

void PhysicsWorld::Update(float dt)
{
    while (prevTime < currTime)
    {
        prevTime += timestep;
        Step(timestep);
    }
    currTime += dt;
}

void PhysicsWorld::Step(float dt)
{
    for (size_t i = 0; i < particles.size(); i++)
    {
        Particle& p = particles[i];

        vec2 fa = p.force * p.invMass;
        vec2 fg = gravity * p.gravityScale;
        p.acc = fa + fg;

        p.vel = p.vel + p.acc * dt;
        p.pos = p.pos + p.vel * dt;

        p.force = v2zero;
    }

    mCollisions = HitTest(particles);
    for (const Manifold& collision : mCollisions)
    {
        ResolveVelocity(collision);
        ResolvePosition(collision);
    }
}

void PhysicsWorld::Render()
{
    std::vector<bool> collisions(particles.size());
    for (size_t i = 0; i < particles.size(); i++)
    {
        for (size_t j = i + 1; j < particles.size(); j++)
        {
            const Particle& a = particles[i];
            const Particle& b = particles[j];
            collisions[i] = collisions[i] || HitTest(a.pos, b.pos, a.collider, b.collider);
        }
    }
    
    for (size_t i = 0; i < particles.size(); i++)
    {
        Color color = collisions[i] ? Color{ 1.0f, 0.0f, 0.0f } : Color{ 0.0f, 1.0f, 0.0f };
        const Particle& p = particles[i];
        switch (p.collider.shape)
        {
        case CIRCLE:
            DrawCircle(p.pos, p.collider.radius, color);
            break;

        case PLANE:
            break;
        }
    }
}
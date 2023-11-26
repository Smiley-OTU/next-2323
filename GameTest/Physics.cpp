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

        vec3 fa = p.force * p.invMass;
        vec3 fg = gravity * p.gravityScale;
        p.acc = fa + fg;

        p.vel = p.vel + p.acc * dt;
        p.pos = p.pos + p.vel * dt;

        p.force = v3zero;
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
    for (const Particle& p : particles)
    {
        switch (p.collider.shape)
        {
        case CIRCLE:
            DrawCircle(p.pos, p.collider.radius, { 1.0f, 0.0f, 0.f });
            break;

        case PLANE:
            break;
        }
    }
}
#include "Physics.h"

void PhysicsWorld::Step(float dt)
{
    while (prevTime < currTime)
    {
        prevTime += timestep;
        Simulate(timestep);
    }
    currTime += dt;
}

void PhysicsWorld::Simulate(float dt)
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
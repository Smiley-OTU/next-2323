//------------------------------------------------------------------------
// GameTest.cpp
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <windows.h> 
#include <math.h>  
//------------------------------------------------------------------------
#include "app\app.h"
//------------------------------------------------------------------------
#include "Physics.h"
#include "Graphics.h"

// (Note that this is in screen-space instead of world-space, so motion appears different)
constexpr vec3 CENTER{ APP_VIRTUAL_WIDTH * 0.5f, APP_VIRTUAL_HEIGHT * 0.5f };

// TODO -- turn this into brick break with students?
// TODO -- make an ECS? Physics is a solved problems... Now we need a video game!!!
PhysicsWorld world;

void Init()
{
	Particle circle1;
	circle1.collider.shape = CIRCLE;
	circle1.collider.radius = 25.0f;
	circle1.collider.dynamic = true;

	circle1.pos = CENTER;
	circle1.friction = 0.5f;
	circle1.restitution = 0.5f;

	Particle circle2;
	circle2.collider.shape = CIRCLE;
	circle2.collider.radius = 25.0f;
	circle2.collider.dynamic = true;

	circle2.pos = v3zero;
	circle2.vel = { 25.0f, 0.0f, 0.0f };
	circle2.friction = 0.5f;
	circle2.restitution = 0.5f;

	Particle plane;
	plane.collider.shape = PLANE;
	plane.collider.normal = vec3{ 0.0f, 1.0f, 0.0f };

	plane.gravityScale = 0.0f;
	plane.invMass = 0.0f;
	plane.friction = 1.0f;
	plane.restitution = 1.0f;

	world.particles.push_back(circle1);
	world.particles.push_back(circle2);
	world.particles.push_back(plane);
}

void Update(float deltaTime)
{
	// Time in seconds
	float dt = deltaTime / 1000.0f;
	world.Update(dt);
}

void Render()
{
	// White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	world.Render();
}

void Shutdown()
{
	world.particles.resize(0);
}

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

#undef near
#undef far
#include "Math2.h"

// (Note that this is in screen-space instead of world-space, so motion appears different)
constexpr vec2 CENTER{ APP_VIRTUAL_WIDTH * 0.5f, APP_VIRTUAL_HEIGHT * 0.5f };

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

	circle2.pos = v2zero;
	circle2.vel = { 25.0f, 0.0f };
	circle2.friction = 0.5f;
	circle2.restitution = 0.5f;

	Particle plane;
	plane.collider.shape = PLANE;
	plane.collider.normal = vec2{ 0.0f, 1.0f };

	plane.gravityScale = 0.0f;
	plane.invMass = 0.0f;
	plane.friction = 1.0f;
	plane.restitution = 1.0f;

	world.particles.push_back(circle1);
	world.particles.push_back(circle2);
	world.particles.push_back(plane);

	glMatrixMode(GL_PROJECTION);
	glOrtho(-100.0f, 100.0f, -100.0f, 100.0f, -1.0f, 1.0f);

	Vector3 eye{ 0.0f, 0.0f, 0.5f };
	Vector3 target{ 0.0f, 0.0f, 0.0f };
	Vector3 up{ 0.0f, 1.0f, 0.0f };
	Matrix view = Transpose(LookAt(eye, target, up));

	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf((float*)&view);
}

void Update(float dt)
{
	dt /= 1000.0f;
	world.Update(dt);
}

void Render()
{
	// White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//world.Render();

	DrawCircleWorld({}, 25.0f);
	// TODO -- use actual world-screen pipeline for matrix stack
	// (Currently, this API does screen to NDC conversion before drawing)
	// (Pre-multiplication, so translate * scale will scale then translate)
	//DrawCircleNDC();
	//DrawCircleWorld(CENTER, 50.0f);
}

void Shutdown()
{
	world.particles.resize(0);
}

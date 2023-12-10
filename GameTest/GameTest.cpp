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

// TODO -- turn this into brick break with students?
// TODO -- make an ECS? Physics is a solved problems... Now we need a video game!!!
PhysicsWorld world;
Particle* player;
float playerWidth = 2.0f;
float playerHeight = 1.0f;
Vector2 mouse{};

struct Renderer
{
	Matrix proj;
	Matrix view;
} renderer;

Vector2 ScreenToWorld(Matrix view, Matrix proj, Vector2 screen)
{
	APP_VIRTUAL_TO_NATIVE_COORDS(screen.x, screen.y);
	Vector4 clip{ screen.x, screen.y, 0.0f, 1.0f };
	Matrix inv = Invert(proj * view);
	Vector4 world = Multiply(clip, inv);
	world.x *= world.w;	
	world.y *= world.w;	
	world.z *= world.w;
	return { world.x, world.y };
}

Particle CreateWall(vec2 position, vec2 normal)
{
	Particle plane;
	plane.collider.shape = PLANE;
	plane.collider.normal = normal;
	plane.collider.dynamic = false;

	plane.pos = position;
	plane.gravityScale = 0.0f;
	plane.invMass = 0.0f;
	plane.friction = 1.0f;
	plane.restitution = 1.0f;

	return plane;
}

void Init()
{
	Particle circle1;
	circle1.collider.shape = CIRCLE;
	circle1.collider.radius = 0.5f;
	circle1.collider.dynamic = true;
	
	circle1.pos = { 5.0f, 5.0f };
	circle1.vel = { -5.0f, 0.0f };
	circle1.friction = 0.0f;
	circle1.restitution = 1.0f;

	Particle circle2;
	circle2.collider.shape = CIRCLE;
	circle2.collider.radius = 0.5f;
	circle2.collider.dynamic = true;

	circle2.pos = { -5.0f, -5.0f };
	circle2.vel = {  5.0f, 0.0f };
	circle2.friction = 0.0f;
	circle2.restitution = 1.0f;

	world.particles.push_back(circle1);
	world.particles.push_back(circle2);

	world.particles.push_back(CreateWall({ 0.0f, 10.0f }, { 0.0f, -1.0f }));	// top
	world.particles.push_back(CreateWall({ 0.0f, -10.0f }, { 0.0f, 1.0f }));	// bottom
	world.particles.push_back(CreateWall({ -10.0f, 0.0f }, { 1.0f, 0.0f }));	// left
	world.particles.push_back(CreateWall({ 10.0f, 0.0f }, { -1.0f, 0.0f }));	// right

	// Hack so the player's memory address doesn't change
	world.particles.push_back({});
	player = &world.particles.back();
	player->collider.shape = AABB;
	player->collider.extents = { playerWidth * 0.5f, playerHeight * 0.5f };
	player->collider.dynamic = false;

	player->pos = { 0.0f, -9.0f };
	player->invMass = 0.0f;
	player->gravityScale = 0.0f;

	glMatrixMode(GL_PROJECTION);
	renderer.proj = Transpose(Ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f));
	glMultMatrixf((float*)&renderer.proj);

	glMatrixMode(GL_MODELVIEW);
	renderer.view = Transpose(LookAt({ 0.0f, 0.0f, 0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }));
	glMultMatrixf((float*)&renderer.view);
}

void Update(float dt)
{
	App::GetMousePos(mouse.x, mouse.y);
	mouse = ScreenToWorld(renderer.view, renderer.proj, mouse);
	dt /= 1000.0f;
	world.Update(dt);

	const CController& controller = CSimpleControllers::GetInstance().GetController(0);
	float speed = 5.0f;
	vec2 direction = {};

	if (controller.GetLeftThumbStickX() < -0.5f)
		direction.x -= 1.0f;
	else if (controller.GetLeftThumbStickX() > 0.5f)
		direction.x += 1.0f;
	if (controller.GetLeftThumbStickY() < -0.5f)
		direction.y -= 1.0f;
	else if (controller.GetLeftThumbStickY() > 0.5f)
		direction.y += 1.0f;
	player->vel = direction * speed;
}

void Render()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	world.Render();
	DrawRect(player->pos, playerWidth, playerHeight, { 1.0f, 0.0f, 0.0f }, true);
	DrawRect({ mouse.x, mouse.y }, 1.0f, 1.0f, { 1.0f, 0.0f, 1.0f });
}

void Shutdown()
{
	world.particles.resize(0);
}

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

Matrix proj;
Matrix view;
vec2 mouse{};

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
	proj = Transpose(Ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f));
	glMultMatrixf((float*)&proj);

	glMatrixMode(GL_MODELVIEW);
	view = Transpose(LookAt({ 0.0f, 0.0f, 0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }));
	glMultMatrixf((float*)&view);
}

void Update(float dt)
{
	float mx, my;
	App::GetMousePos(mx, my);
	mouse = ScreenToWorld(view, proj, { mx, my });

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
	DrawRect(mouse, 1.0f, 1.0f, { 1.0f, 0.0f, 1.0f });

	vec2 cursor = WorldToScreen({ mouse.x, mouse.y }, view, proj);
	char buffer[64];
	sprintf(buffer, "x: %f, y: %f", cursor.x, cursor.y);
	DrawText({ -9.9f, 9.5f }, buffer, { 1.0f, 0.0f, 1.0f });
}

void Shutdown()
{
	world.particles.resize(0);
}

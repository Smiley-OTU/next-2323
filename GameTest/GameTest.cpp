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
Entities entities;
Physics physics;

Entity* player;
float playerWidth = 2.0f;
float playerHeight = 1.0f;

Matrix proj;
Matrix view;
vec2 mouse{};

Entity CreateWall(vec2 position, vec2 normal)
{
	Entity plane;
	plane.collider.shape = PLANE;
	plane.collider.normal = normal;

	plane.pos = position;
	plane.gravityScale = 0.0f;
	plane.invMass = 0.0f;
	plane.friction = 1.0f;
	plane.restitution = 1.0f;

	plane.tag = WALL;
	return plane;
}

Entity CreateBrick(vec2 position, float w, float h)
{
	Entity brick;
	brick.collider.shape = AABB;
	brick.collider.extents = { w * 0.5f, h * 0.5f };

	brick.pos = position;
	brick.gravityScale = 0.0f;
	brick.invMass = 0.0f;
	brick.friction = 1.0f;
	brick.restitution = 1.0f;

	brick.fgColor = { 0.0f, 0.0f, 1.0f };
	brick.bgColor = { 0.0f, 1.0f, 1.0f };

	brick.tag = BRICK;
	return brick;
}

Entity CreateBall(vec2 position, float r)
{
	Entity ball;
	ball.collider.shape = CIRCLE;
	ball.collider.radius = r;

	ball.pos = position;
	ball.gravityScale = 1.0f;
	ball.invMass = 1.0f;
	ball.friction = 0.0f;
	ball.restitution = 1.0f;

	ball.fgColor = { 0.5f, 0.5f, 0.5f };
	ball.tag = BALL;
	return ball;
}

void Init()
{
	entities.reserve(1024);

	// Player is always entities.front()
	entities.push_back({});
	player = &entities.front();
	player->collider.shape = AABB;
	player->collider.extents = { playerWidth * 0.5f, playerHeight * 0.5f };

	player->pos = { 0.0f, -9.0f };
	player->invMass = 0.0f;
	player->gravityScale = 0.0f;
	player->tag = PLAYER;

	player->bgColor = { 1.0f, 0.0f, 0.0f };
	player->fgColor = { 0.0f, 1.0f, 0.0f };
	
	// Walls will never be deleted, so they come next
	entities.push_back(CreateWall({ 0.0f, 10.0f }, { 0.0f, -1.0f }));	// top
	entities.push_back(CreateWall({ 0.0f, -10.0f }, { 0.0f, 1.0f }));	// bottom
	entities.push_back(CreateWall({ -10.0f, 0.0f }, { 1.0f, 0.0f }));	// left
	entities.push_back(CreateWall({ 10.0f, 0.0f }, { -1.0f, 0.0f }));	// right

	// Balls will also persist
	entities.push_back(CreateBall({ -9.0f, 0.0f }, 0.5f));
	entities.push_back(CreateBall({ 9.0f, 0.0f }, 0.5f));

	// Finally, the bricks!
	size_t rowCount = 8;
	size_t colCount = 8;
	float brickWidth = 20.0f / (colCount + 2);
	float brickHeight = 20.0f / (rowCount + 4);
	float x, y = -10.0f + brickHeight * 3.5f;
	for (size_t i = 0; i < rowCount; i++)
	{
		x = -10.0f + brickWidth * 1.5f;
		for (size_t j = 0; j < colCount; j++)
		{
			entities.push_back(CreateBrick({ x, y }, brickWidth, brickHeight));
			x += brickWidth;
		}
		y += brickHeight;
	}
	
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
	physics.Update(dt, entities);

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

//void DrawBricks()
//{
//	for (size_t i = 0; i < entities.size(); i++)
//	{
//		const Entity& p = entities[i];
//		if (p.tag != BRICK || p.disabled) continue;
//		DrawRect(p.pos, p.collider.extents.x * 2.0f, p.collider.extents.y * 2.0f, { 0.0f, 0.0f, 1.0f });
//		DrawRect(p.pos, p.collider.extents.x * 2.0f, p.collider.extents.y * 2.0f, { 0.0f, 1.0f, 1.0f }, true);
//	}
//}
//
//void DrawBall()
//{
//
//}
//
//void DrawPlayer()
//{
//	DrawRect(player->pos, playerWidth, playerHeight, { 1.0f, 0.0f, 0.0f }, true);
//}

void DrawEntities()
{
	for (size_t i = 0; i < entities.size(); i++)
	{
		const Entity& p = entities[i];
		switch (p.tag)
		{
		case BALL:
			DrawCircle(p.pos, p.collider.radius, p.fgColor);
			break;

		case BRICK:
		case PLAYER:
			DrawRect(p.pos, p.collider.extents.x * 2.0f, p.collider.extents.y * 2.0f, p.fgColor);
			DrawRect(p.pos, p.collider.extents.x * 2.0f, p.collider.extents.y * 2.0f, p.bgColor, true);
			break;
		}
	}
}

void Render()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//physics.Render(entities);
	DrawEntities();
	
	DrawRect(mouse, 1.0f, 1.0f, { 1.0f, 0.0f, 1.0f });
	vec2 cursor = WorldToScreen({ mouse.x, mouse.y }, view, proj);
	char buffer[64];
	sprintf(buffer, "x: %f, y: %f", cursor.x, cursor.y);
	DrawText({ -9.9f, 9.5f }, buffer, { 1.0f, 0.0f, 1.0f });
}

void Shutdown()
{
	entities.resize(0);
}

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

Entities entities;
Physics physics;

Matrix proj;
Matrix view;
vec2 mouse{};

Entity* player;
constexpr float PLAYER_WIDTH = 2.0f;
constexpr float PLAYER_HEIGHT = 1.0f;
constexpr float BALL_RADIUS = 0.5f;

constexpr float LEFT = -10.0f;
constexpr float RIGHT = 10.0f;
constexpr float TOP = 10.0f;
constexpr float BOTTOM = -10.0f;

constexpr size_t BRICK_ROWS = 8;
constexpr size_t BRICK_COLS = 8;
constexpr float BRICK_WIDTH = (RIGHT - LEFT) / (BRICK_COLS + 2);
constexpr float BRICK_HEIGHT = (TOP - BOTTOM) / (BRICK_ROWS + 4);

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

Entity CreateBrick(vec2 position)
{
	Entity brick;
	brick.collider.shape = AABB;
	brick.collider.extents = { BRICK_WIDTH * 0.5f, BRICK_HEIGHT * 0.5f };

	brick.pos = position;
	brick.gravityScale = 0.0f;
	brick.invMass = 0.0f;
	brick.friction = 1.0f;
	brick.restitution = 1.0f;

	brick.tag = BRICK;
	return brick;
}

void BallCollisionHandler(Entity& entity);
Entity CreateBall(vec2 position)
{
	Entity ball;
	ball.collider.shape = CIRCLE;
	ball.collider.radius = BALL_RADIUS;

	ball.pos = position;
	ball.vel = { 0.0f, -10.0f };
	ball.gravityScale = 0.0f;
	ball.invMass = 1.0f;
	ball.friction = 0.0f;
	ball.restitution = 1.0f;

	ball.onCollision = BallCollisionHandler;
	ball.tag = BALL;
	return ball;
}

void Init()
{
	entities.reserve(1024);

	entities.push_back({});
	player = &entities.front();
	player->collider.shape = AABB;
	player->collider.extents = { PLAYER_WIDTH * 0.5f, PLAYER_HEIGHT * 0.5f };

	player->pos = { 0.0f, -9.0f };
	player->invMass = 0.0f;
	player->gravityScale = 0.0f;
	player->tag = PLAYER;
	
	entities.push_back(CreateWall({ 0.0f, 10.0f }, { 0.0f, -1.0f }));	// top
	entities.push_back(CreateWall({ 0.0f, -10.0f }, { 0.0f, 1.0f }));	// bottom
	entities.push_back(CreateWall({ -10.0f, 0.0f }, { 1.0f, 0.0f }));	// left
	entities.push_back(CreateWall({ 10.0f, 0.0f }, { -1.0f, 0.0f }));	// right

	entities.push_back(CreateBall({ -9.0f, 0.0f }));
	entities.push_back(CreateBall({ 9.0f, 0.0f }));

	float x, y = BOTTOM + BRICK_HEIGHT * 3.5f;
	for (size_t i = 0; i < BRICK_ROWS; i++)
	{
		x = LEFT + BRICK_WIDTH * 1.5f;
		for (size_t j = 0; j < BRICK_COLS; j++)
		{
			entities.push_back(CreateBrick({ x, y }));
			x += BRICK_WIDTH;
		}
		y += BRICK_HEIGHT;
	}
	
	glMatrixMode(GL_PROJECTION);
	proj = Transpose(Ortho(LEFT, RIGHT, BOTTOM, TOP, -1.0f, 1.0f));
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
	//if (controller.GetLeftThumbStickY() < -0.5f)
	//	direction.y -= 1.0f;
	//else if (controller.GetLeftThumbStickY() > 0.5f)
	//	direction.y += 1.0f;
	player->vel = direction * speed;
}

void DrawEntities()
{
	for (const Entity& entity : entities)
	{
		if (entity.disabled) continue;
		switch (entity.tag)
		{
		case BALL:
			DrawCircle(entity.pos, entity.collider.radius, { 0.5f, 0.5f, 0.5f });
			break;

		case BRICK:
			DrawRect(entity.pos, entity.collider.extents.x * 2.0f, entity.collider.extents.y * 2.0f, { 0.0f, 0.0f, 1.0f });
			DrawRect(entity.pos, entity.collider.extents.x * 2.0f, entity.collider.extents.y * 2.0f, { 0.0f, 1.0f, 1.0f }, true);
			break;

		case PLAYER:
			DrawRect(entity.pos, entity.collider.extents.x * 2.0f, entity.collider.extents.y * 2.0f, { 0.0f, 1.0f, 0.0f });
			DrawRect(entity.pos, entity.collider.extents.x * 2.0f, entity.collider.extents.y * 2.0f, { 1.0f, 0.0f, 0.0f }, true);
			break;
		}
	}
}

void Render()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	DrawEntities();
	
	// Debug collision rendering
	//physics.Render(entities);
	
	// Test screen-to-world & world-to-screen
	//DrawRect(mouse, 1.0f, 1.0f, { 1.0f, 0.0f, 1.0f });
	//vec2 cursor = WorldToScreen({ mouse.x, mouse.y }, view, proj);
	//char buffer[64];
	//sprintf(buffer, "x: %f, y: %f", cursor.x, cursor.y);
	//DrawText({ -9.9f, 9.5f }, buffer, { 1.0f, 0.0f, 1.0f });
}

void Shutdown()
{
	entities.resize(0);
}

void BallCollisionHandler(Entity& entity)
{
	if (entity.tag == BRICK)
	{
		entity.disabled = true;
	}
	else if (entity.tag == WALL)
	{

	}
}
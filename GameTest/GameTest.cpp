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

// TODO 1 -- Add a Game class to handle different screens and replay
// TODO 2 -- Use kinematics to make particle systems. Spawn debris on brick coillision
Entities entities;
Physics physics;

mat4 proj;
mat4 view;
vec2 mouse{};

enum GameState
{
	PLAY,
	TITLE,
	INSTRUCTIONS,
	WIN,
	LOSS
} gameState;

int lives = 3;
int breaks = 0;
Entity* player;
constexpr float PLAYER_WIDTH = 2.0f;
constexpr float PLAYER_HEIGHT = 1.0f;

Entity* ball1, *ball2;
constexpr float BALL_RADIUS = 0.5f;

constexpr size_t BRICK_ROWS = 8;
constexpr size_t BRICK_COLS = 8;
constexpr float BRICK_WIDTH = WIDTH / (BRICK_COLS + 2);
constexpr float BRICK_HEIGHT = HEIGHT / (BRICK_ROWS + 4);

constexpr float BUTTON_WIDTH = WIDTH * 0.33f;
constexpr float BUTTON_HEIGHT = HEIGHT * 0.1f;
vec2 play = { 0.0f, TOP * 0.5f };
vec2 quit = { 0.0f, BOTTOM * 0.5f };
vec2 instructions{};

CSimpleSprite* testSprite;

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

void BallCollisionHandler(Entity& ball, Entity& other);
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
	// Simply set APP_USE_VIRTUAL_RES to false and the API will render accordingly!
	testSprite = App::CreateSprite(".\\TestData\\Test.bmp", 8, 4);
	testSprite->CreateAnimation(0, 1.0f / 15.0f, { 0,1,2,3,4,5,6,7 });
	testSprite->SetPosition(4.0f, 4.0f);
	testSprite->SetScale(0.1f);
	testSprite->SetAnimation(0);
	testSprite->SetAngle(45.0f);
	// TODO -- make a "screen to clip" function that converts from screen to NDC, then scales by proj extents.
	//vec2 dimTex{ testSprite->GetWidth(), testSprite->GetHeight() };
	//vec2 dim = ScreenToWorld(view, proj, dimTex);

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
	ball1 = &entities.back();

	entities.push_back(CreateBall({ 9.0f, 0.0f }));
	ball2 = &entities.back();

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
	App::GetMousePos(mouse.x, mouse.y);
	// TODO -- use ScreenToClip to fix mouse pos, or maybe just roll my own
	//mouse = ScreenToWorld(mouse, view, proj);
	testSprite->Update(dt);

	dt /= 1000.0f;
	physics.Update(dt, entities);

	const CController& controller = CSimpleControllers::GetInstance().GetController(0);
	float speed = 5.0f;
	vec2 direction = {};

	if (App::IsKeyPressed(VK_LBUTTON) && gameState == TITLE)
	{
		vec2 extents = vec2{ BUTTON_WIDTH, BUTTON_HEIGHT } * 0.25f;
		if (CircleRect(mouse, 1.0f, play, extents))
			gameState = PLAY;

		else if (CircleRect(mouse, 1.0f, instructions, extents))
			gameState = INSTRUCTIONS;

		else if (CircleRect(mouse, 1.0f, quit, extents))
			exit(0);
	}

	if (controller.GetLeftThumbStickX() < -0.5f)
		direction.x -= 1.0f;
	else if (controller.GetLeftThumbStickX() > 0.5f)
		direction.x += 1.0f;
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
	//vec2 mtv{};
	//bool collision = RectRect(mouse, testExtents, testPos, testExtents, &mtv);
	//Color color = collision ? Color{ 1.0f, 0.0f, 0.0f } : Color{ 0.0f, 1.0f, 0.0f };
	//testPos = testPos + mtv;
	//DrawRect(mouse, testExtents.x * 2.0f, testExtents.y * 2.0f, color);
	//DrawRect(testPos, testExtents.x * 2.0f, testExtents.y * 2.0f, color);

	switch (gameState)
	{
	case TITLE:
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		DrawRect(play, BUTTON_WIDTH, BUTTON_HEIGHT, { 0.0f, 1.0f, 0.0f });
		DrawText(play, "PLAY");
		DrawRect(instructions, BUTTON_WIDTH, BUTTON_HEIGHT, { 0.0f, 1.0f, 1.0f });
		DrawText(instructions, "INSTRUCTIONS");
		DrawRect(quit, BUTTON_WIDTH, BUTTON_HEIGHT, { 1.0f, 0.0f, 0.0f });
		DrawText(quit, "QUIT");
		break;
	
	case INSTRUCTIONS:
		break;
	
	case PLAY:
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		DrawEntities();
		char text[64];
		sprintf(text, "Lives: %i", lives);
		DrawText({ -9.9f, 9.5f }, text, { 1.0f, 0.0f, 0.0f });
		break;
	
	case WIN:
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		DrawText({ -9.9f, 9.5f }, "You win :)", { 0.0f, 1.0f, 1.0f });
		break;
	
	case LOSS:
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		DrawText({ -9.9f, 9.5f }, "You lost :(", { 1.0f, 0.0f, 0.0f });
		break;
	}

	testSprite->Draw();
	char text[64];
	sprintf(text, "%f %f\n", mouse.x, mouse.y);
	DrawText({ -9.0f, 9.0f }, text, { 1.0f, 0.0f, 1.0f });
}

void Shutdown()
{
	entities.resize(0);
}

void OnReset()
{
	ball1->pos = { -9.0f, 0.0f };
	ball2->pos = {  9.0f, 0.0f };
	ball1->vel = ball2->vel = { 0.0f, -10.0f };
	ball1->disabled = ball2->disabled = false;
	lives--;
	if (lives < 0)
		gameState = LOSS;
}

void BallCollisionHandler(Entity& ball, Entity& other)
{
	if (other.tag == BRICK)
	{
		other.disabled = true;
		breaks++;
		if (breaks >= BRICK_ROWS * BRICK_COLS)
			gameState = WIN;
	}
	else if (other.tag == WALL)
	{
		float hw = (RIGHT - LEFT) * 0.5f - BRICK_WIDTH;
		float hh = BRICK_HEIGHT;
		if (CircleRect(ball.pos, BALL_RADIUS, { 0.0f, BOTTOM }, { hw, hh }))
		{
			ball.disabled = true;
		}
		if (ball1->disabled && ball2->disabled)
			OnReset();
	}
}

#pragma once
#include "app\app.h"
#include "Math.h"
#include <vector>

constexpr float LEFT = -10.0f;
constexpr float RIGHT = 10.0f;
constexpr float TOP = 10.0f;
constexpr float BOTTOM = -10.0f;

constexpr float WIDTH = (RIGHT - LEFT);
constexpr float HEIGHT = (TOP - BOTTOM);

struct Color
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

inline vec2 ScreenToWorld(const vec2& screen, const mat4& view, const mat4& proj)
{
	vec4 clip{ screen.x, screen.y, 0.0f, 1.0f };
	APP_VIRTUAL_TO_NATIVE_COORDS(clip.x, clip.y);
	vec4 world = Invert(proj * view) * clip;
	world.x *= world.w;
	world.y *= world.w;
	world.z *= world.w;
	return { world.x, world.y };
}

inline vec2 WorldToScreen(const vec2& world, const mat4& view, const mat4& proj)
{
	vec4 clip = { world.x, world.y, 0.0f, 1.0f };
	clip = proj * view * clip;
	clip.x /= clip.w;
	clip.y /= clip.w;
	clip.z /= clip.w;
	APP_NATIVE_TO_VIRTUAL_COORDS(clip.x, clip.y);
	return { clip.x, clip.y };
}

inline void DrawCircle(vec2 pos, float radius = 0.5f, Color color = { 1.0f, 1.0f, 1.0f }, bool wireframe = false)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, 0.0f);
	glScalef(radius, radius, 1.0f);
	glColor3f(color.r, color.g, color.b);

	glBegin(wireframe ? GL_LINE_LOOP : GL_TRIANGLE_FAN);
	for (int i = 0; i < 64; i++) {
		float angle = TWO_PI * i / 64.0f;
		glVertex2f(cos(angle), sin(angle));
	}
	glEnd();
	glPopMatrix();
}

inline void DrawRect(vec2 pos, float width, float height, Color color = { 1.0f, 1.0f, 1.0f }, bool wireframe = false)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	float xMin = - width * 0.5f;
	float xMax = + width * 0.5f;
	float yMin = - height * 0.5f;
	float yMax = + height * 0.5f;

	glTranslatef(pos.x, pos.y, 0.0f);
	glColor3f(color.r, color.g, color.b);

	glBegin(wireframe ? GL_LINE_LOOP : GL_QUADS);
	glVertex2f(xMin, yMax);	// top left
	glVertex2f(xMax, yMax);	// top right
	glVertex2f(xMax, yMin);	// bot right
	glVertex2f(xMin, yMin);	// bot left
	glEnd();
	glPopMatrix();
}

#undef DrawText
inline void DrawText(vec2 pos, const char* str, Color color = { 1.0f, 1.0f, 1.0f }, void* font = GLUT_BITMAP_HELVETICA_18)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, 0.0f);

	glColor3f(color.r, color.g, color.b);
	glRasterPos2f(0.0f, 0.0f);
	int l = (int)strlen(str);
	for (int i = 0; i < l; i++)
	{
		glutBitmapCharacter(font, str[i]); // Print a character on the screen
	}
	glPopMatrix();
}

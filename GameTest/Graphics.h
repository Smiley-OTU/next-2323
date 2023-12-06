#pragma once
#include "app\app.h"
#include "Math.h"
#include <vector>

struct Color
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

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

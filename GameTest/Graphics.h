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

inline vec2 ScreenToNDC(vec2 screen)
{
	vec2 ndc;
	ndc.x = 2.0f * screen.x / APP_VIRTUAL_WIDTH - 1.0f;
	ndc.y = 2.0f * screen.y / APP_VIRTUAL_HEIGHT - 1.0f;
	return ndc;
}

// Replace with full pipeline ASAP
inline void DrawCircle(vec2 pos, float radius, Color color = {})
{
	float sx = (radius / APP_VIRTUAL_WIDTH) * 2.0f;
	float sy = (radius / APP_VIRTUAL_HEIGHT) * 2.0f;
	pos = ScreenToNDC(pos);

	glPushMatrix();
	glTranslatef(pos.x, pos.y, 0.0f);
	glScalef(sx, sy, 1.0f);
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 64; i++) {
		float angle = TWO_PI * i / 64.0f;
		glVertex2f(cos(angle), sin(angle));
	}
	glEnd();
	glPopMatrix();
}

// Testing only
inline void DrawCircleNDC(float radius = 1.0f)
{
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 64; i++) {
		float angle = TWO_PI * i / 64.0f;
		glVertex2f(cos(angle) * radius, sin(angle) * radius);
	}
	glEnd();
}

// Not working. Can't crack the fixed function pipeline matrix stack xD
inline void DrawCircleWorld(vec2 pos, float radius = 0.5f)
{
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glTranslatef(pos.x, pos.y, 0.0f);
	float s = radius * 2.0f;
	glScalef(s, s, 1.0f);
	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 64; i++) {
		float angle = TWO_PI * i / 64.0f;
		glVertex2f(cos(angle), sin(angle));
	}
	glEnd();
	glPopMatrix();
}

// TODO -- figure out gl 1.0 matrix stack so I can create vertices in NDC
/*
struct Transform2
{
	vec2 scale;
	float rotation;
	vec2 translation;
};

inline void DrawPolygon(const std::vector<vec2>& vertices, const Transform2& transform)
{
	
}
*/
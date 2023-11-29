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

inline void DrawCircle(vec2 pos, float radius = 0.5f, Color color = { 1.0f, 1.0f, 1.0f })
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, 0.0f);
	glScalef(radius, radius, 1.0f);
	glColor3f(color.r, color.g, color.b);

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 64; i++) {
		float angle = TWO_PI * i / 64.0f;
		glVertex2f(cos(angle), sin(angle));
	}
	glEnd();
	glPopMatrix();
}

// TODO -- transform class and polygon rendering?